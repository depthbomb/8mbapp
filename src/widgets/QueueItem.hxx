#pragma once
#include "../lib/RemoteManager.hxx"

#include <QLabel>
#include <QWidget>
#include <QHBoxLayout>
#include <QProgressBar>

class QueueItem final : public QWidget {
    Q_OBJECT

public:
    explicit QueueItem(const QString &filePath, const QString &outputPath, const bool &highQuality, const bool &noSound, const int &size);

    void stop();

signals:
    void finished();

private slots:
    void onManagerProgress(const QString& message);
    void onManagerThumbnailSaved(const QString& thumbnailPath);
    void onManagerTranscodeProgress(const double& percent);
    void onManagerTranscodeFinished();
    void onManagerError(const QString& message);
    void onManagerDone();

private:
    static constexpr int THUMBNAIL_SIZE = 48;

    QHBoxLayout m_layout;
    QLabel m_thumbnail;
    QLabel m_fileName;
    QLabel m_status;
    QProgressBar m_progressBar;
    RemoteManager m_manager;

    void setThumbnail(const QString& thumbnailPath);
};
