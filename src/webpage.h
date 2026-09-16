#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineProfile>
#include "config.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QWebEnginePermission>
#endif

class PermissiveWebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    PermissiveWebEnginePage(QWebEngineProfile *profile, QObject *parent, PermissionConfig *permConfig);

protected:
    QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type) override;

private slots:
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    void onPermissionRequested(QWebEnginePermission request);
#else
    void onFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
#endif

private:
    PermissionConfig *m_permConfig;
};

#endif // WEBPAGE_H
