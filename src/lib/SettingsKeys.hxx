#pragma once
#include <QMap>
#include <QString>

enum class SettingKey {
    FirstRun,
    WindowGeometry,
    WindowState,
    DownloadDir,
    Size,
    HighQuality,
    NoSound,
};

static const QMap<SettingKey, QString> SettingKeyStrings = {
    { SettingKey::FirstRun,       QStringLiteral("first_run") },
    { SettingKey::WindowGeometry, QStringLiteral("window_geometry") },
    { SettingKey::WindowState,    QStringLiteral("window_state") },
    { SettingKey::DownloadDir,    QStringLiteral("download_dir") },
    { SettingKey::Size,           QStringLiteral("size") },
    { SettingKey::HighQuality,    QStringLiteral("hq") },
    { SettingKey::NoSound,        QStringLiteral("no_sound") },
};

inline QString toString(const SettingKey key) {
    return SettingKeyStrings.value(key);
}
