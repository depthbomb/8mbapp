#pragma once
#include <QDialog>

class SettingsWindow final : public QDialog {
    Q_OBJECT

public:
    explicit SettingsWindow(bool intro = false, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    bool m_isIntro = false;
};
