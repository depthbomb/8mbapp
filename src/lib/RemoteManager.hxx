#pragma once
#include <QFile>
#include <QTimer>
#include <QMutex>
#include <QObject>
#include <QStateMachine>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QRegularExpressionMatchIterator>

/**
 * @brief Manages uploading, monitoring, and downloading files through 8mb.video
 */
class RemoteManager final : public QObject
{
    Q_OBJECT

public:
    explicit RemoteManager(QObject *parent = nullptr);

    bool isWorking() const;
    void start(const QString &filePath, const QString &outputPath, const bool &highQuality, const bool &noSound, const int &size);
    void stop();

signals:
    void next();
    void error(const QString &error);
    void progress(const QString &message);
    void thumbnailSaved(const QString &thumbnailPath);
    void transcodeProgress(const double &progress);
    void transcodeFinished();
    void done();

private:
    inline static const auto HOST_REGEX_PATTERN = new QRegularExpression(R"(transcode-[a-fA-F0-9]{6}\.8mb\.video)");

    static inline QStringList s_cachedHosts;
    static inline QMutex s_mutex;

    bool m_highQuality = true;
    bool m_noSound = false;
    int m_size = 0;
    QString m_filePath;
    QString m_outputPath;
    QList<QNetworkReply*> m_pendingReplies;
    QFile* m_activeFile = nullptr;
    QString m_chosenHost;
    QString m_token;
    QString m_thumbnailUrl;
    QStateMachine m_machine;
    QTimer *m_pollTimer = nullptr;
    QNetworkAccessManager m_networkManager;
};
