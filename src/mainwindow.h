#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>
#include <QCloseEvent>
#include "config.h"
#include "i18n.h"
#include "webpage.h"

class GeminiMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GeminiMainWindow(bool silentMode = false, QWidget *parent = nullptr);
    ~GeminiMainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void toggleStartSetting(const QString &key, bool checked);
    void setPermission(const QString &key, bool checked);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onDownloadRequested(QWebEngineDownloadRequest *download);
    void quitApp();

private:
    void updateWebSettings();
    void initTray();
    QIcon getAppIcon() const;

    QSettings *m_settings;
    StartConfig *m_startConfig;
    PermissionConfig *m_permConfig;
    WindowConfig *m_winConfig;
    Translations m_lang;

    QWebEngineProfile *m_webProfile;
    PermissiveWebEnginePage *m_webPage;
    QWebEngineView *m_webView;
    QSystemTrayIcon *m_trayIcon;
    QAction *m_rememberAction;
};

#endif // MAINWINDOW_H
