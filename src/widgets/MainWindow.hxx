#pragma once
#include "QueueArea.hxx"
#include "LinkLabel.hxx"

#include <QObject>
#include <QTextEdit>
#include <QPushButton>
#include <QMainWindow>

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onQueueAreaQueueEmptied();
    void onMainButtonClicked();
    void onSettingsLinkClicked();

private:
    QueueArea m_queueArea;
    QPushButton m_mainButton;
    QPushButton m_cancelButton;
    LinkLabel m_settingsLink;
};
