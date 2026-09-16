#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>
#include <QVariant>
#include <QMap>
#include <QByteArray>

class Config {
public:
    Config(QSettings *settings, const QString &group, const QMap<QString, QVariant> &defaults);
    virtual ~Config() = default;

    QVariant get(const QString &key) const;
    bool getBool(const QString &key) const;
    QByteArray getByteArray(const QString &key) const;
    void set(const QString &key, const QVariant &value);

protected:
    void initDefaults();

    QSettings *m_settings;
    QString m_group;
    QMap<QString, QVariant> m_defaults;
};

class StartConfig : public Config {
public:
    explicit StartConfig(QSettings *settings);
};

class PermissionConfig : public Config {
public:
    explicit PermissionConfig(QSettings *settings);
};

class WindowConfig : public Config {
public:
    explicit WindowConfig(QSettings *settings);
};

namespace DesktopUtils {
    extern const QString SYSTEM_DESKTOP_FILE;
    QString getUserDesktopFile();
    QString getAutostartLink();

    void ensureUserDesktopFile();
    void updateUserDesktopExec(const QString &extraArg);
    void createAutostartLink();
    void removeAutostartLink();
}

#endif // CONFIG_H
