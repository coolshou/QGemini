#ifndef I18N_H
#define I18N_H

#include <QString>
#include <QLocale>
#include <QProcessEnvironment>

struct Translations {
    QString autostart;
    QString silent;
    QString remember;
    QString reload;
    QString exit;
    QString downloadProgress;
    QString cancel;
    QString saveFile;
    QString downloadTitle;
    QString permissions;
    QString microphone;
    QString notifications;
    QString clipboard;
    QString location;
};

inline Translations getTranslations() {
    QString langEnv = qEnvironmentVariable("LANG");
    if (langEnv.isEmpty()) {
        langEnv = QLocale::system().name();
    }
    bool isZh = langEnv.toLower().startsWith("zh");

    if (isZh) {
        return Translations{
            QString::fromUtf8("開機啟動"),
            QString::fromUtf8("靜默啟動"),
            QString::fromUtf8("記憶視窗狀態"),
            QString::fromUtf8("強制重載"),
            QString::fromUtf8("退出"),
            QString::fromUtf8("下載中…"),
            QString::fromUtf8("取消"),
            QString::fromUtf8("保存文件"),
            QString::fromUtf8("文件下載"),
            QString::fromUtf8("權限設定"),
            QString::fromUtf8("麥克風"),
            QString::fromUtf8("通知"),
            QString::fromUtf8("剪貼簿"),
            QString::fromUtf8("地理位置")
        };
    } else {
        return Translations{
            QStringLiteral("Launch on Startup"),
            QStringLiteral("Silent Start"),
            QStringLiteral("Remember Window State"),
            QStringLiteral("Force Reload"),
            QStringLiteral("Quit"),
            QStringLiteral("Downloading…"),
            QStringLiteral("Cancel"),
            QStringLiteral("Save File"),
            QStringLiteral("Download"),
            QStringLiteral("Permissions"),
            QStringLiteral("Microphone"),
            QStringLiteral("Notifications"),
            QStringLiteral("Clipboard"),
            QStringLiteral("Geolocation")
        };
    }
}

#endif // I18N_H
