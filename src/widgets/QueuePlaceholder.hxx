#pragma once
#include <QWidget>

class QueuePlaceholder final : public QWidget {
    Q_OBJECT

public:
    explicit QueuePlaceholder(QWidget *parent = nullptr);
};
