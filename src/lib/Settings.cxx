#include "Globals.hxx"
#include "Settings.hxx"

#include <QDir>
#include <QDataStream>

static const QFileInfo SETTINGS_FILE_PATH(Globals::SETTINGS_FILE_PATH);

Settings& Settings::instance() {
    static Settings instance;
    return instance;
}

Settings::Settings(QObject* parent) : QObject(parent) {}

void Settings::load() {
    if (!SETTINGS_FILE_PATH.exists()) {
        QDir().mkpath(SETTINGS_FILE_PATH.absolutePath());
        settings.clear();
        save();
        return;
    }

    QFile file(SETTINGS_FILE_PATH.filePath());
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> settings;
    }
}

void Settings::save() {
    QMutexLocker l(&mutex);
    QFile file(SETTINGS_FILE_PATH.filePath());
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << settings;
        emit saved();
    }
}

void Settings::set(const SettingKey key, const QVariant& value) {
    QMutexLocker l(&mutex);
    settings[toString(key)] = value;
}

void Settings::setDefault(const SettingKey key, const QVariant& value) {
    QMutexLocker l(&mutex);
    if (!settings.contains(toString(key))) {
        settings[toString(key)] = value;
    }
}
