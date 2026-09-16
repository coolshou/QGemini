#include "config.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>

Config::Config(QSettings *settings, const QString &group, const QMap<QString, QVariant> &defaults)
    : m_settings(settings), m_group(group), m_defaults(defaults) {
    initDefaults();
}

void Config::initDefaults() {
    m_settings->beginGroup(m_group);
    for (auto it = m_defaults.cbegin(); it != m_defaults.cend(); ++it) {
        if (!m_settings->contains(it.key())) {
            m_settings->setValue(it.key(), it.value());
        }
    }
    m_settings->endGroup();
}

QVariant Config::get(const QString &key) const {
    m_settings->beginGroup(m_group);
    QVariant def = m_defaults.value(key);
    QVariant val = m_settings->value(key, def);
    m_settings->endGroup();
    return val;
}

bool Config::getBool(const QString &key) const {
    QVariant val = get(key);
    if (val.userType() == QMetaType::Bool) {
        return val.toBool();
    }
    return val.toString().compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0;
}

QByteArray Config::getByteArray(const QString &key) const {
    return get(key).toByteArray();
}

void Config::set(const QString &key, const QVariant &value) {
    m_settings->beginGroup(m_group);
    m_settings->setValue(key, value);
    m_settings->endGroup();
}

StartConfig::StartConfig(QSettings *settings)
    : Config(settings, QStringLiteral("Start"), {
        {QStringLiteral("autostart"), false},
        {QStringLiteral("silent"), false}
    }) {}

PermissionConfig::PermissionConfig(QSettings *settings)
    : Config(settings, QStringLiteral("Permission"), {
        {QStringLiteral("microphone"), true},
        {QStringLiteral("notifications"), true},
        {QStringLiteral("clipboard"), true},
        {QStringLiteral("location"), true}
    }) {}

WindowConfig::WindowConfig(QSettings *settings)
    : Config(settings, QStringLiteral("Window"), {
        {QStringLiteral("geometry"), QByteArray()},
        {QStringLiteral("rememberWindow"), false}
    }) {}

namespace DesktopUtils {
    const QString SYSTEM_DESKTOP_FILE = QStringLiteral("/usr/share/applications/gemini-qt.desktop");

    QString getUserDesktopFile() {
        return QDir::homePath() + QStringLiteral("/.local/share/applications/gemini-qt.desktop");
    }

    QString getAutostartLink() {
        return QDir::homePath() + QStringLiteral("/.config/autostart/gemini-qt.desktop");
    }

    void ensureUserDesktopFile() {
        QString userFile = getUserDesktopFile();
        if (!QFile::exists(userFile) && QFile::exists(SYSTEM_DESKTOP_FILE)) {
            QDir().mkpath(QFileInfo(userFile).dir().path());
            QFile::copy(SYSTEM_DESKTOP_FILE, userFile);
        }
    }

    void updateUserDesktopExec(const QString &extraArg) {
        QString userFile = getUserDesktopFile();
        if (!QFile::exists(userFile)) {
            return;
        }

        QFile readFile(userFile);
        if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        QStringList lines;
        QTextStream in(&readFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(QStringLiteral("Exec="))) {
                QString execCmd = line.mid(5).trimmed();
                QStringList parts = execCmd.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
                parts.removeAll(QStringLiteral("--silent"));
                if (!extraArg.isEmpty() && !parts.contains(extraArg)) {
                    parts.append(extraArg);
                }
                line = QStringLiteral("Exec=") + parts.join(QStringLiteral(" "));
            }
            lines.append(line);
        }
        readFile.close();

        QFile writeFile(userFile);
        if (writeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&writeFile);
            for (const QString &l : lines) {
                out << l << "\n";
            }
            writeFile.close();
        }
    }

    void createAutostartLink() {
        QString link = getAutostartLink();
        QString target = getUserDesktopFile();
        QDir().mkpath(QFileInfo(link).dir().path());
        QFileInfo fi(link);
        if (!fi.isSymLink() && !QFile::exists(link) && QFile::exists(target)) {
            QFile::link(target, link);
        }
    }

    void removeAutostartLink() {
        QString link = getAutostartLink();
        QFileInfo fi(link);
        if (fi.isSymLink() || QFile::exists(link)) {
            QFile::remove(link);
        }
    }
}
