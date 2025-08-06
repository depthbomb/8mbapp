#include "AccentColor.hxx"

#include <QSettings>

QColor getAccentColor() {
    static std::optional<QColor> cachedColor;

    if (cachedColor.has_value()) {
        return *cachedColor;
    }

    const QSettings settings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\DWM)", QSettings::NativeFormat);

    if (const QVariant colorVar = settings.value("AccentColor"); colorVar.isValid()) {
        bool ok;
        const uint colorValue = colorVar.toUInt(&ok);

        if (ok) {
            const int r = colorValue & 0x000000FF;
            const int g = (colorValue & 0x0000FF00) >> 8;
            const int b = (colorValue & 0x00FF0000) >> 16;
            const int a = (colorValue & 0xFF000000) >> 24;

            cachedColor = QColor(r, g, b, a);

            return *cachedColor;
        }
    }

    qWarning() << "Failed to read accent color from registry";

    cachedColor = QColor();

    return *cachedColor;
}
