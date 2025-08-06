#pragma once
#include "QueueItem.hxx"
#include "QueuePlaceholder.hxx"


#include <QObject>
#include <QScrollArea>
#include <QVBoxLayout>

class QueueArea final : public QScrollArea {
    Q_OBJECT

public:
    explicit QueueArea(QWidget *parent = nullptr);

    void stop();
    void addQueueItem(QueueItem *item);
    void removeQueueItem(QueueItem *item);
    void removeAllQueueItems();

signals:
    void queueEmptied();

public slots:
    void onItemFinished();

private:
    QList<QueueItem*> m_queueItems;
    QWidget m_queueWidget;
    QVBoxLayout m_queueLayout;
    QueuePlaceholder m_queuePlaceholder;
};
