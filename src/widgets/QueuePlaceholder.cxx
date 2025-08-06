#include "QueuePlaceholder.hxx"

#include <QLabel>
#include <QVBoxLayout>

QueuePlaceholder::QueuePlaceholder(QWidget* parent)
    : QWidget(parent) {
    const auto mainLayout = new QVBoxLayout;

    mainLayout->addWidget(new QLabel("<h2>Click the <i>Browse...</i> button to select video files.</h2><p>You can add videos while the queue is already active.</p>"));
    mainLayout->setAlignment(Qt::AlignCenter);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(mainLayout);
}

