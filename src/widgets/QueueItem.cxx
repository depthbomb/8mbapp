#include "QueueItem.hxx"

#include <QLabel>

QueueItem::QueueItem(const QString &filePath, const QString &outputPath, const bool &highQuality, const bool &noSound, const int &size) {
    m_thumbnail.setFixedSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);

    QFont fileNameFont;
    fileNameFont.setBold(true);
    m_fileName.setFont(fileNameFont);
    m_fileName.setText(QUrl(filePath).fileName());
    m_fileName.setWordWrap(true);

    QFont statusFont;
    statusFont.setPointSize(7);
    m_status.setFont(statusFont);

    m_progressBar.setFixedSize(100, 16);
    m_progressBar.setRange(0, 0);
    m_progressBar.setVisible(false);

    const auto textLayout = new QVBoxLayout;
    {
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(0);

        textLayout->addWidget(&m_fileName);
        textLayout->addWidget(&m_status);
    }

    m_layout.setSpacing(6);
    m_layout.addWidget(&m_thumbnail);
    m_layout.addLayout(textLayout);
    m_layout.addStretch();
    m_layout.addWidget(&m_progressBar);

    setThumbnail(":images/video.png");
    setLayout(&m_layout);
    setFixedHeight(64);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setPalette(palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Midlight));
    setAutoFillBackground(true);

    connect(&m_manager, &RemoteManager::progress, this, &QueueItem::onManagerProgress);
    connect(&m_manager, &RemoteManager::error, this, &QueueItem::onManagerError);
    connect(&m_manager, &RemoteManager::thumbnailSaved, this, &QueueItem::onManagerThumbnailSaved);
    connect(&m_manager, &RemoteManager::transcodeProgress, this, &QueueItem::onManagerTranscodeProgress);
    connect(&m_manager, &RemoteManager::transcodeFinished, this, &QueueItem::onManagerTranscodeFinished);
    connect(&m_manager, &RemoteManager::done, this, &QueueItem::onManagerDone);

    m_manager.start(filePath, outputPath, highQuality, noSound, size);
}

void QueueItem::stop() {
    m_manager.stop();
}

void QueueItem::onManagerProgress(const QString& message) {
    if (!m_progressBar.isVisible()) {
        m_progressBar.setVisible(true);
    }

    m_status.setText(message);
}

void QueueItem::onManagerThumbnailSaved(const QString& thumbnailPath) {
    setThumbnail(thumbnailPath);
}

void QueueItem::onManagerTranscodeProgress(const double& percent) {
    if (m_progressBar.maximum() == 0) {
        m_progressBar.setRange(0, 100);
    }

    m_progressBar.setValue(static_cast<int>(percent));
}

void QueueItem::onManagerTranscodeFinished() {
    m_progressBar.setRange(0, 0);
}

void QueueItem::onManagerError(const QString& message) {
    m_status.setText(QString(R"(<p style="color: red;">Error: %1</p>)").arg(message));
    m_progressBar.setVisible(false);

    QTimer::singleShot(5000, this, [this]{
        emit finished();
    });
}

void QueueItem::onManagerDone() {
    m_progressBar.setVisible(false);
    emit finished();
}

void QueueItem::setThumbnail(const QString& thumbnailPath) {
    QPixmap pixmap(thumbnailPath);
    if (pixmap.isNull()) {
        return;
    }

    pixmap = pixmap.scaled(THUMBNAIL_SIZE, THUMBNAIL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_thumbnail.setPixmap(pixmap);
}

