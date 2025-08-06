#include "lib/Settings.hxx"
#include "widgets/MainWindow.hxx"
#include "widgets/SettingsWindow.hxx"

#include <QStyleHints>
#include <QApplication>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");
    QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);

    {
        Settings::instance().load();
        Settings::instance().setDefault(SettingKey::FirstRun, true);
        Settings::instance().setDefault(SettingKey::DownloadDir, QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        Settings::instance().setDefault(SettingKey::HighQuality, true);
        Settings::instance().setDefault(SettingKey::NoSound, false);
        Settings::instance().setDefault(SettingKey::Size, 10);
        Settings::instance().save();
    }

    if (Settings::instance().get<bool>(SettingKey::FirstRun, false)) {
        if (SettingsWindow sw(true); sw.exec() == QDialog::Rejected) {
            return 0;
        }

        Settings::instance().set(SettingKey::FirstRun, false);
        Settings::instance().save();
    }

    MainWindow w;
    w.show();

    return QApplication::exec();
}
