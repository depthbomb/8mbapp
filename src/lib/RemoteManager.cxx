#include "RemoteManager.hxx"

#include <QDir>
#include <QFileInfo>
#include <QJsonValue>
#include <QFinalState>
#include <QJsonDocument>
#include <QTemporaryFile>
#include <QHttpMultiPart>
#include <QRandomGenerator>

RemoteManager::RemoteManager(QObject *parent)
    : QObject(parent) {
    auto *sPreCheck = new QState;
    auto *sHostsRetrieval = new QState;
    auto *sHostPicker = new QState;
    auto *sUpload = new QState;
    auto *sSaveThumbnail = new QState;
    auto *sPoll = new QState;
    auto *sDownload = new QState;
    auto *sDone = new QFinalState;

    m_machine.addState(sPreCheck);
    m_machine.addState(sHostsRetrieval);
    m_machine.addState(sHostPicker);
    m_machine.addState(sUpload);
    m_machine.addState(sSaveThumbnail);
    m_machine.addState(sPoll);
    m_machine.addState(sDownload);
    m_machine.addState(sDone);
    m_machine.setInitialState(sPreCheck);

#pragma region State_Connections
    /**
     * Perform some checks before continuing.
     */
    connect(sPreCheck, &QState::entered, this, [this] {
        const QFileInfo fi(m_filePath);
        if (fi.size() <= m_size * 1024 * 1024) {
            emit error("File is already smaller than or equal to the desired size.");
        } else {
            emit next();
        }
    });

    /**
     * Load hosts if needed.
     */
    connect(sHostsRetrieval, &QState::entered, this, [this] {
        QMutexLocker l(&s_mutex);

        if (s_cachedHosts.isEmpty()) {
            emit progress("Loading hosts...");

            auto *reply = m_networkManager.get(QNetworkRequest(QUrl("https://8mb.video/")));

            m_pendingReplies.append(reply);

            connect(reply, &QNetworkReply::finished, this, [this, reply] {
                reply->deleteLater();

                m_pendingReplies.removeOne(reply);

                if (reply->error() == QNetworkReply::OperationCanceledError) {
                    return;
                }

                if (reply->error() != QNetworkReply::NoError) {
                    emit error(reply->errorString());
                    return;
                }

                const auto text = QString(reply->readAll());
                QRegularExpressionMatchIterator it = HOST_REGEX_PATTERN->globalMatch(text);

                while (it.hasNext()) {
                    auto match = it.next();
                    auto host = match.captured(0);

                    s_cachedHosts.append(host);
                }

                emit progress("Loaded hosts: " + s_cachedHosts.join(", "));
                emit next();
            });
        } else {
            emit progress("Using cached hosts: " + s_cachedHosts.join(", "));
            emit next();
        }
    });

    /**
     * Ping a random host to use.
     * TODO do what the website does to ping each host and use the one with the best score
     */
    connect(sHostPicker, &QState::entered, this, [this] {
        const auto idx = QRandomGenerator::global()->bounded(s_cachedHosts.size());

        m_chosenHost = s_cachedHosts.at(idx);

        emit progress("Using host: " + m_chosenHost);

        const auto hostUrl = QString("https://%1?ping").arg(m_chosenHost);
        auto *reply = m_networkManager.get(QNetworkRequest(QUrl(hostUrl)));

        m_pendingReplies.append(reply);

        emit progress("Retrieving token...");

        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            reply->deleteLater();

            m_pendingReplies.removeOne(reply);

            if (reply->error() == QNetworkReply::OperationCanceledError) {
                return;
            }

            if (reply->error() != QNetworkReply::NoError) {
                emit error(reply->errorString());
                return;
            }

            const auto doc = QJsonDocument::fromJson(reply->readAll());

            m_token = doc["token"].toString();

            emit progress("Retrieved token: " + m_token);
            emit next();
        });
    });

    /**
     * Send the file upload request.
     */
    connect(sUpload, &QState::entered, this, [this] {
        m_activeFile = new QFile(m_filePath);
        if (!m_activeFile->open(QIODevice::ReadOnly)) {
            emit error(m_activeFile->errorString());
            delete m_activeFile;
            return;
        }

#pragma region Request_Building
        const auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart filePart;
        filePart.setHeader(
            QNetworkRequest::ContentDispositionHeader,
            QVariant(R"(form-data; name="fileToUpload"; filename=")" + QFileInfo(m_filePath).fileName() + "\"")
        );
        filePart.setBodyDevice(m_activeFile);
        m_activeFile->setParent(multiPart);

        QHttpPart hqPart;
        hqPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"hq\""));
        hqPart.setBody(m_highQuality ? "true" : "false");

        QHttpPart noSoundPart;
        noSoundPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"nosound\""));
        noSoundPart.setBody(m_noSound ? "true" : "false");

        QHttpPart sizePart;
        sizePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"size\""));
        sizePart.setBody(QString::number(m_size).toUtf8());

        QHttpPart customSizePart;
        customSizePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"customsize\""));
        customSizePart.setBody(QString::number(m_size).toUtf8());

        QHttpPart tokenPart;
        tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
        tokenPart.setBody(m_token.toUtf8());

        QHttpPart videoDurationPart;
        videoDurationPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"video_duration\""));
        videoDurationPart.setBody("1");

        QHttpPart submitPart;
        submitPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"submit\""));
        submitPart.setBody("true");

        multiPart->append(filePart);
        multiPart->append(hqPart);
        multiPart->append(noSoundPart);
        multiPart->append(sizePart);
        multiPart->append(customSizePart);
        multiPart->append(tokenPart);
        multiPart->append(videoDurationPart);
        multiPart->append(submitPart);
#pragma endregion

        const QUrl url(QString("https://%1").arg(m_chosenHost));
        const QNetworkRequest request(url);

        auto *reply = m_networkManager.post(request, multiPart);

        m_pendingReplies.append(reply);

        multiPart->setParent(reply);

        emit progress("Uploading...");

        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            reply->deleteLater();

            m_pendingReplies.removeOne(reply);
            m_activeFile = nullptr;

            if (reply->error() == QNetworkReply::OperationCanceledError) {
                return;
            }

            if (reply->error() != QNetworkReply::NoError) {
                emit error(reply->errorString());
                return;
            }

            emit progress("File successfully uploaded");

            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto thumbnailPath = doc["thumb"].toString();

            m_thumbnailUrl = QString("https://%1%2").arg(m_chosenHost, thumbnailPath);

            emit next();
        });
    });

    /**
     * Saves the video thumbnail to disk to display as a taskbar button overlay.
     */
    connect(sSaveThumbnail, &QState::entered, this, [this] {
        const QNetworkRequest request(m_thumbnailUrl);

        auto *reply = m_networkManager.get(request);

        m_pendingReplies.append(reply);

        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            reply->deleteLater();

            m_pendingReplies.removeOne(reply);

            if (reply->error() == QNetworkReply::OperationCanceledError) {
                return;
            }

            if (reply->error() != QNetworkReply::NoError) {
                qWarning() << reply->errorString();
                emit next();
                return;
            }

            if (QTemporaryFile tempFile; tempFile.open()) {
                tempFile.write(reply->readAll());
                tempFile.close();

                emit thumbnailSaved(tempFile.fileName());
            }

            emit next();
        });
    });

    /**
     * Check the transcoding progress.
     */
    connect(sPoll, &QState::entered, this, [this] {
        if (!m_pollTimer) {
            m_pollTimer = new QTimer(this);
        }

        m_pollTimer->setInterval(500);

        emit progress("Transcoding...");

        connect(m_pollTimer, &QTimer::timeout, this, [this] {
            const auto pollUrl = QString("https://%1/check-progress?token=%2").arg(m_chosenHost, m_token);
            const QNetworkRequest req(pollUrl);
            auto *reply = m_networkManager.get(req);

            m_pendingReplies.append(reply);

            connect(reply, &QNetworkReply::finished, this, [this, reply] {
                reply->deleteLater();

                m_pendingReplies.removeOne(reply);

                if (reply->error() == QNetworkReply::OperationCanceledError) {
                    return;
                }

                if (reply->error() != QNetworkReply::NoError) {
                    emit error(reply->errorString());
                    return;
                }

                const auto doc = QJsonDocument::fromJson(reply->readAll());
                if (const auto status = doc["status"].toString(); status == "Done") {
                    emit transcodeFinished();
                    emit next();
                    m_pollTimer->stop();
                } else {
                    const auto percent = doc["percent"].toDouble();
                    emit transcodeProgress(percent);
                }
            });
        });

        m_pollTimer->start();
    });

    /**
     * Download the transcoded file.
     */
    connect(sDownload, &QState::entered, this, [this] {
        const auto downloadUrl = QString("https://%1/8mb.video-%2.mp4").arg(m_chosenHost, m_token);
        const QNetworkRequest req(downloadUrl);

        auto *reply = m_networkManager.get(req);

        m_pendingReplies.append(reply);

        emit progress("Downloading file...");

        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            reply->deleteLater();

            m_pendingReplies.removeOne(reply);

            if (reply->error() == QNetworkReply::OperationCanceledError) {
                return;
            }

            if (reply->error() != QNetworkReply::NoError) {
                emit error(reply->errorString());
                return;
            }

            const QDir outputPath(m_outputPath);
            auto outFile = QFile(outputPath.filePath(QString("8mb.video-%1.mp4").arg(m_token)));
            if (outFile.open(QIODevice::WriteOnly)) {
                outFile.write(reply->readAll());
                outFile.close();

                emit progress("File downloaded!");
            } else {
                emit error("Failed to write file.");
            }

            emit next();
            emit done();
        });
    });
#pragma endregion

    sPreCheck->addTransition(this, &RemoteManager::next, sHostsRetrieval);
    sHostsRetrieval->addTransition(this, &RemoteManager::next, sHostPicker);
    sHostPicker->addTransition(this, &RemoteManager::next, sUpload);
    sUpload->addTransition(this, &RemoteManager::next, sSaveThumbnail);
    sSaveThumbnail->addTransition(this, &RemoteManager::next, sPoll);
    sPoll->addTransition(this, &RemoteManager::next, sDownload);
    sDownload->addTransition(this, &RemoteManager::next, sDone);
}

bool RemoteManager::isWorking() const {
    return m_machine.isRunning();
}

void RemoteManager::start(const QString &filePath, const QString &outputPath, const bool &highQuality, const bool &noSound, const int &size) {
    if (isWorking()) {
        return;
    }

    m_filePath = filePath;
    m_outputPath = outputPath;
    m_highQuality = highQuality;
    m_noSound = noSound;
    m_size = size;

    m_machine.start();
}

void RemoteManager::stop() {
    if (!isWorking()) {
        return;
    }

    for (auto *reply : m_pendingReplies) {
        if (reply && reply->isRunning()) {
            reply->abort();
        }
    }

    m_pendingReplies.clear();

    if (m_pollTimer) {
        m_pollTimer->stop();
    }

    if (m_activeFile) {
        if (m_activeFile->isOpen()) {
            m_activeFile->close();
        }

        delete m_activeFile;
        m_activeFile = nullptr;
    }

    m_machine.stop();

    emit progress("Cancelled");
    emit done();
}
