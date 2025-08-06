#include "LinkLabel.hxx"
#include "tabs/AboutTab.hxx"
#include "SettingsWindow.hxx"
#include "../lib/Settings.hxx"
#include "tabs/SettingsTab.hxx"

#include <QTabWidget>
#include <QVBoxLayout>

SettingsWindow::SettingsWindow(const bool intro, QWidget *parent)
    : QDialog(parent),
    m_isIntro(intro) {
    const auto mainLayout = new QVBoxLayout;
    {
        const auto tabs = new QTabWidget;
        const auto settingsTab = new SettingsTab;

        tabs->addTab(settingsTab, QIcon(":images/settings.svg"), "Settings");

        // In "intro" mode, the About tab is not added and saving settings will close the dialog. This is intended to
        // onboard the user in a way and show them what settings are available before they actually use the app.
        if (!m_isIntro) {
            tabs->addTab(new AboutTab, QIcon(":images/help.svg"), "About");
        } else {
            connect(settingsTab->settingsButton(), &QPushButton::clicked, this, [this] {
                accept();
            });

            setWindowFlag(Qt::WindowCloseButtonHint, false);
        }

        mainLayout->addWidget(tabs);
    }

    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/icons/icon.ico"));
    setLayout(mainLayout);
}

void SettingsWindow::closeEvent(QCloseEvent* event) {
    if (m_isIntro) {
        // Send the Reject code in case the user ALT+F4s the dialog, which is handled in main.cpp and will exit the
        // application.
        reject();
    }

    QDialog::closeEvent(event);
}
