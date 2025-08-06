#include "MainWindow.hxx"
#include "SettingsWindow.hxx"
#include "../lib/Settings.hxx"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_settingsLabel("<a href=\"#\">Settings</a>") {

    const auto mainWidget = new QWidget;
    const auto mainLayout = new QVBoxLayout;
    {
        const auto buttonsLayout = new QHBoxLayout;
        {
            buttonsLayout->addWidget(&m_mainButton);
            buttonsLayout->addWidget(&m_cancelButton);
            buttonsLayout->addStretch();
            buttonsLayout->addWidget(&m_settingsLabel);
            buttonsLayout->setContentsMargins(0, 0, 0, 0);
        }

        mainLayout->addWidget(&m_queueArea);
        mainLayout->addLayout(buttonsLayout);
    }

    mainWidget->setLayout(mainLayout);

    m_mainButton.setText("&Browse...");

    m_cancelButton.setText("&Cancel");
    m_cancelButton.setEnabled(false);

    connect(&m_queueArea, &QueueArea::queueEmptied, this, &MainWindow::onQueueAreaQueueEmptied);
    connect(&m_mainButton, &QPushButton::clicked, this, &MainWindow::onMainButtonClicked);
    connect(&m_cancelButton, &QPushButton::clicked, this, [&] {
        m_queueArea.stop();
    });
    connect(&m_settingsLabel, &QLabel::linkActivated, this, &MainWindow::onSettingsLinkClicked);

    setWindowIcon(QIcon(":/icons/icon.ico"));
    setCentralWidget(mainWidget);
    setMinimumSize(500, 300);
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    restoreGeometry(Settings::instance().get<QByteArray>(SettingKey::WindowGeometry, QByteArray()));
    restoreState(Settings::instance().get<QByteArray>(SettingKey::WindowState, QByteArray()));
}

void MainWindow::closeEvent(QCloseEvent* event) {
    Settings::instance().set(SettingKey::WindowGeometry, saveGeometry());
    Settings::instance().set(SettingKey::WindowState, saveState());
    Settings::instance().save();

    m_queueArea.stop();

    QMainWindow::closeEvent(event);
}

void MainWindow::onQueueAreaQueueEmptied() {
    m_cancelButton.setEnabled(false);
}

void MainWindow::onMainButtonClicked() {
    QFileDialog fp(this);
    fp.setFilter(QDir::Files);
    fp.setFileMode(QFileDialog::ExistingFiles);
    fp.setNameFilter("Video Files (*.mp4 *.avi *.mov *.mkv *.webm *.flv)");
    fp.setWindowTitle("Choose a video file");
    if (fp.exec() == QDialog::Accepted) {
        const auto fileNames = fp.selectedFiles();
        if (fileNames.size() >= 10) {
            QMessageBox mb;
            mb.setWindowTitle("Warning");
            mb.setIcon(QMessageBox::Warning);
            mb.setText("Selecting too many files may result in rate limiting or failed requests.\nYou should select"
                       " only a few files at a time.\nWould you like to continue?");
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if (mb.exec() == QMessageBox::No) {
                return;
            }
        }

        const auto downloadDir = Settings::instance().get<QString>(SettingKey::DownloadDir, "");
        const auto hq = Settings::instance().get<bool>(SettingKey::HighQuality, true);
        const auto noSound = Settings::instance().get<bool>(SettingKey::NoSound, false);
        const auto size = Settings::instance().get<int>(SettingKey::Size, 10);

        for (auto &fileName : fileNames) {
            m_queueArea.addQueueItem(
                new QueueItem(fileName, downloadDir, hq, noSound, size)
            );
        }

        m_cancelButton.setEnabled(true);
    }
}

void MainWindow::onSettingsLinkClicked() {
    SettingsWindow settingsWindow;
    settingsWindow.exec();
}
