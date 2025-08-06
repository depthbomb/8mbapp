#include "SettingsTab.hxx"
#include "lib/Settings.hxx"

#include <QLabel>
#include <QStyle>
#include <QTimer>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

SettingsTab::SettingsTab(QWidget* parent)
    : QWidget(parent) {
    m_saveButton.setText("&Save");

    connect(&m_saveButton, &QPushButton::clicked, this, &SettingsTab::onSaveButtonClicked);

    const auto mainLayout = new QVBoxLayout;
    {
        const auto downloadDirLayout = new QHBoxLayout;
        {
            downloadDirLayout->setContentsMargins(0, 0, 0, 0);

            m_dirBrowseButton.setText(Settings::instance().get<QString>(SettingKey::DownloadDir, ""));
            m_dirBrowseButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DirIcon));
            m_dirBrowseButton.setFocusPolicy(Qt::FocusPolicy::NoFocus);
            m_dirBrowseButton.setMinimumWidth(CONTROL_MIN_WIDTH);

            connect(&m_dirBrowseButton, &QPushButton::clicked, this, [&] {
                QFileDialog fd(this);
                fd.setFilter(QDir::Dirs);
                fd.setFileMode(QFileDialog::Directory);
                fd.setWindowTitle("Choose a download directory");
                if (fd.exec() == QDialog::Accepted) {
                    const QString downloadDir = fd.selectedFiles().first();
                    if (const QDir dir(downloadDir); !dir.exists()) {
                        QMessageBox mb;
                        mb.setIcon(QMessageBox::Critical);
                        mb.setWindowTitle("Directory not found");
                        mb.setText("The chosen directory does not exist.");
                        mb.exec();
                    } else {
                        Settings::instance().set(SettingKey::DownloadDir, downloadDir);
                        Settings::instance().save();
                        m_dirBrowseButton.setText(downloadDir);
                    }
                }
            });

            downloadDirLayout->addWidget(new QLabel("Download directory:"));
            downloadDirLayout->addStretch();
            downloadDirLayout->addWidget(&m_dirBrowseButton);
        }

        m_hqCheckBox.setText("High quality (slower)");
        m_hqCheckBox.setChecked(
            Settings::instance().get<bool>(SettingKey::HighQuality, true)
        );

        m_noSoundCheckBox.setText("No sound");
        m_noSoundCheckBox.setChecked(
            Settings::instance().get<bool>(SettingKey::NoSound, false)
        );

        const auto sizeLayout = new QHBoxLayout;
        {
            sizeLayout->setContentsMargins(0, 0, 0, 0);

            m_sizeSpinBox.setRange(10, 50);
            m_sizeSpinBox.setSuffix("MB");
            m_sizeSpinBox.setMinimumWidth(CONTROL_MIN_WIDTH);
            m_sizeSpinBox.setValue(
                Settings::instance().get<int>(SettingKey::Size, 10)
            );

            sizeLayout->addWidget(new QLabel("Size:"));
            sizeLayout->addStretch();
            sizeLayout->addWidget(&m_sizeSpinBox);
        }

        mainLayout->addLayout(downloadDirLayout);
        mainLayout->addLayout(sizeLayout);
        mainLayout->addWidget(&m_hqCheckBox);
        mainLayout->addWidget(&m_noSoundCheckBox);
        mainLayout->addStretch();
        mainLayout->addWidget(&m_saveButton, 0, Qt::AlignRight);
    }

    setLayout(mainLayout);
}

QPushButton* SettingsTab::settingsButton() {
    return &m_saveButton;
}

void SettingsTab::onSaveButtonClicked() {
    m_saveButton.setEnabled(false);

    QTimer::singleShot(2000, this, [this] {
        m_saveButton.setEnabled(true);
    });

    Settings::instance().set(SettingKey::HighQuality, m_hqCheckBox.isChecked());
    Settings::instance().set(SettingKey::Size, m_sizeSpinBox.value());
    Settings::instance().set(SettingKey::NoSound, m_noSoundCheckBox.isChecked());
    Settings::instance().save();
}
