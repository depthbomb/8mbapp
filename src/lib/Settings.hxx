#pragma once
#include "SettingsKeys.hxx"

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVariant>

class Settings final : public QObject {
    Q_OBJECT

public:
    static Settings& instance();

    void load();
    void save();

    template<typename T>
    T get(const SettingKey key, const T& defaultValue) const {
        if (settings.contains(toString(key))) {
            if (const QVariant val = settings.value(toString(key)); val.canConvert<T>()) {
                return val.value<T>();
            }
        }

        return defaultValue;
    }

    void set(SettingKey key, const QVariant& value);
    void setDefault(SettingKey key, const QVariant& value);

    signals:
        void saved();

private:
    explicit Settings(QObject* parent = nullptr);
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    QMap<QString, QVariant> settings;
    mutable QMutex mutex;
};
