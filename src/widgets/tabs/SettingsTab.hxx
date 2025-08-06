#pragma once
#include <QObject>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

class SettingsTab final : public QWidget {
    Q_OBJECT

public:
    explicit SettingsTab(QWidget* parent = nullptr);

    QPushButton *settingsButton();

private:
    static constexpr int CONTROL_MIN_WIDTH = 200;

    QPushButton m_dirBrowseButton;
    QSpinBox m_sizeSpinBox;
    QCheckBox m_hqCheckBox;
    QCheckBox m_noSoundCheckBox;
    QPushButton m_saveButton;

private slots:
    void onSaveButtonClicked();
};
