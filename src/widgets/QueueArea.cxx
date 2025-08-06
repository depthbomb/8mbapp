#include "QueueArea.hxx"

QueueArea::QueueArea(QWidget* parent)
    : QScrollArea(parent) {
    QPalette p;
    p.setColor(QPalette::Window, p.color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Mid));

    m_queueLayout.setAlignment(Qt::AlignTop);
    m_queueLayout.setContentsMargins(0, 0, 0, 0);
    m_queueLayout.setSpacing(1);
    m_queueLayout.addWidget(&m_queuePlaceholder);
    m_queueWidget.setPalette(p);
    m_queueWidget.setLayout(&m_queueLayout);

    setWidget(&m_queueWidget);
    setWidgetResizable(true);
    setPalette(p);
    setStyleSheet("QueueArea { border: 1px solid black; }");
}

void QueueArea::stop() {
    for (const auto &item : m_queueItems) {
        item->stop();
    }
}

void QueueArea::addQueueItem(QueueItem* item) {
    if (m_queueItems.contains(item)) {
        return;
    }

    connect(item, &QueueItem::finished, this, &QueueArea::onItemFinished);

    m_queueItems.append(item);
    m_queueLayout.addWidget(item);
    m_queuePlaceholder.hide();
}

void QueueArea::removeQueueItem(QueueItem *item) {
    if (!m_queueItems.contains(item)) {
        return;
    }

    m_queueItems.removeOne(item);
    m_queueLayout.removeWidget(item);

    item->deleteLater();

    if (m_queueItems.isEmpty()) {
        m_queuePlaceholder.show();
        emit queueEmptied();
    }
}

void QueueArea::removeAllQueueItems() {
    for (const auto &item : m_queueItems) {
        removeQueueItem(item);
    }
}

void QueueArea::onItemFinished() {
    removeQueueItem(
        qobject_cast<QueueItem*>(sender())
    );
}
