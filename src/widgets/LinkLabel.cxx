#include "LinkLabel.hxx"
#include "../lib/AccentColor.hxx"

LinkLabel::LinkLabel(const QString& text, QWidget* parent)
    : QLabel(parent),
    m_initialColor(getAccentColor()),
    m_hoveredColor(getAccentColor().lighter(125)),
    m_clickedColor(getAccentColor().darker(200)),
    m_initialStyle(QString("QLabel { color: %1; }").arg(m_initialColor.name())),
    m_hoveredStyle(QString("QLabel { color: %1; }").arg(m_hoveredColor.name())),
    m_clickedStyle(QString("QLabel { color: %1; }").arg(m_clickedColor.name())) {

    setText(text);
    setMouseTracking(true);
    setCursor(Qt::CursorShape::PointingHandCursor);

    applyStyles();
}

void LinkLabel::mouseMoveEvent(QMouseEvent* event) {
    if (!m_hovered) {
        m_hovered = true;
        applyStyles();
    }

    QLabel::mouseMoveEvent(event);
}

void LinkLabel::leaveEvent(QEvent* event) {
    if (m_hovered) {
        m_hovered = false;
        applyStyles();
    }

    QLabel::leaveEvent(event);
}

void LinkLabel::mousePressEvent(QMouseEvent* event) {
    if (!m_clicked) {
        m_clicked = true;
        applyStyles();
    }

    QLabel::mousePressEvent(event);
}

void LinkLabel::mouseReleaseEvent(QMouseEvent* event) {
    if (m_clicked) {
        m_clicked = false;
        applyStyles();

        if (rect().contains(event->pos())) {
            emit clicked();
        }
    }

    QLabel::mouseReleaseEvent(event);
}

void LinkLabel::applyStyles() {
    QString styleToApply;
    if (m_clicked) {
        styleToApply = m_clickedStyle;
    } else if (m_hovered) {
        styleToApply = m_hoveredStyle;
    } else {
        styleToApply = m_initialStyle;
    }

    if (styleToApply != styleSheet()) {
        setStyleSheet(styleToApply);
    }
}
