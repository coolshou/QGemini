#include "webpage.h"

#include <QDesktopServices>
#include <QUrl>

PermissiveWebEnginePage::PermissiveWebEnginePage(QWebEngineProfile *profile, QObject *parent, PermissionConfig *permConfig)
    : QWebEnginePage(profile, parent), m_permConfig(permConfig) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    connect(this, &QWebEnginePage::permissionRequested, this, &PermissiveWebEnginePage::onPermissionRequested);
#else
    connect(this, &QWebEnginePage::featurePermissionRequested, this, &PermissiveWebEnginePage::onFeaturePermissionRequested);
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
void PermissiveWebEnginePage::onPermissionRequested(QWebEnginePermission request) {
    auto type = request.permissionType();
    bool grant = false;

    if (type == QWebEnginePermission::PermissionType::MediaAudioCapture && m_permConfig->getBool(QStringLiteral("microphone"))) {
        grant = true;
    } else if (type == QWebEnginePermission::PermissionType::Notifications && m_permConfig->getBool(QStringLiteral("notifications"))) {
        grant = true;
    } else if (type == QWebEnginePermission::PermissionType::Geolocation && m_permConfig->getBool(QStringLiteral("location"))) {
        grant = true;
    } else if (type == QWebEnginePermission::PermissionType::ClipboardReadWrite && m_permConfig->getBool(QStringLiteral("clipboard"))) {
        grant = true;
    }

    if (grant) {
        request.grant();
    } else {
        request.deny();
    }
}
#else
void PermissiveWebEnginePage::onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
    bool grant = false;

    if (feature == QWebEnginePage::MediaAudioCapture && m_permConfig->getBool(QStringLiteral("microphone"))) {
        grant = true;
    } else if (feature == QWebEnginePage::Notifications && m_permConfig->getBool(QStringLiteral("notifications"))) {
        grant = true;
    } else if (feature == QWebEnginePage::Geolocation && m_permConfig->getBool(QStringLiteral("location"))) {
        grant = true;
    }

    setFeaturePermission(securityOrigin, feature,
                         grant ? QWebEnginePage::PermissionGrantedByUser
                               : QWebEnginePage::PermissionDeniedByUser);
}
#endif

QWebEnginePage *PermissiveWebEnginePage::createWindow(QWebEnginePage::WebWindowType type) {
    Q_UNUSED(type);
    auto *newPage = new QWebEnginePage(profile(), this);
    connect(newPage, &QWebEnginePage::urlChanged, this, [newPage](const QUrl &url) {
        if (!url.isEmpty() && url.isValid()) {
            QDesktopServices::openUrl(url);
        }
        newPage->deleteLater();
    });
    return newPage;
}
