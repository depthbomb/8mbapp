#include "AboutTab.hxx"

#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>

#include "../../Globals.hxx"

AboutTab::AboutTab(QWidget* parent)
    : QWidget(parent) {
    const auto mainLayout = new QHBoxLayout;
    {
        const auto logo = new QLabel;
        logo->setScaledContents(true);
        logo->setFixedSize(64, 64);
        logo->setPixmap(QPixmap(":/images/icon.png"));

        const auto contentLayout = new QVBoxLayout;
        {
            const auto versionGroupBox = new QGroupBox("Application Info");
            {
                const auto vbox = new QVBoxLayout;
                const auto label = new QLabel(
                    QString(R"(v%1 (<a href="%2">%3</a>) &bull; <a href="%4">Source Code</a> &bull; Compiled %5)").arg(
                        Globals::APP_VERSION_STRING,
                        Globals::APP_COMMIT_URL,
                        Globals::APP_GIT_HASH,
                        Globals::APP_REPO_URL,
                        Globals::APP_BUILD_DATE.toString()
                    )
                );
                label->setOpenExternalLinks(true);

                vbox->addWidget(label);
                versionGroupBox->setLayout(vbox);
            }

            const auto disclaimerGroupBox = new QGroupBox("Disclaimer");
            {
                const auto vbox = new QVBoxLayout;
                const auto label = new QLabel(R"(This is an unofficial application and is not associated with <i>8mb.video</i>.<br>If you enjoy the service then consider <a href="https://www.patreon.com/bklug">supporting its creator.</a>)");
                label->setOpenExternalLinks(true);

                vbox->addWidget(label);
                disclaimerGroupBox->setLayout(vbox);
            }

            const auto technologiesGroupBox = new QGroupBox("Technologies");
            {
                const auto hbox = new QHBoxLayout;
                const auto link = new QLabel("<a href=\"#\">Qt Framework</a>");

                connect(link, &QLabel::linkActivated, this, [this] {
                    QMessageBox::aboutQt(this);
                });

                hbox->addWidget(link);
                hbox->addStretch();
                technologiesGroupBox->setLayout(hbox);
            }

            contentLayout->addWidget(versionGroupBox);
            contentLayout->addWidget(disclaimerGroupBox);
            contentLayout->addWidget(technologiesGroupBox);
        }

        mainLayout->addWidget(logo, 0, Qt::AlignTop);
        mainLayout->addLayout(contentLayout);
    }

    setLayout(mainLayout);
}
