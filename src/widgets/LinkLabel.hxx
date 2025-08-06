#pragma once
#include <QLabel>
#include <QColor>
#include <QMouseEvent>

class LinkLabel : public QLabel {
    Q_OBJECT

public:
    explicit LinkLabel(const QString& text, QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool m_hovered = false;
    bool m_clicked = false;
    QColor m_initialColor;
    QColor m_hoveredColor;
    QColor m_clickedColor;
    QString m_initialStyle;
    QString m_hoveredStyle;
    QString m_clickedStyle;

    void applyStyles();
};

