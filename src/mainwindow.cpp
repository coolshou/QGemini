#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QWebEngineSettings>

GeminiMainWindow::GeminiMainWindow(bool silentMode, QWidget *parent)
    : QMainWindow(parent),
      m_settings(new QSettings(QStringLiteral("gemini-qt"),
                               QStringLiteral("gemini-qt"), this)),
      m_startConfig(new StartConfig(m_settings)),
      m_permConfig(new PermissionConfig(m_settings)),
      m_winConfig(new WindowConfig(m_settings)), m_lang(getTranslations()),
      m_webProfile(new QWebEngineProfile(QStringLiteral("Default"), this)),
      m_webPage(nullptr), m_webView(nullptr), m_trayIcon(nullptr),
      m_rememberAction(nullptr) {

  setWindowTitle(QStringLiteral("Gemini"));
  resize(1200, 800);
  setWindowIcon(getAppIcon());

  m_webProfile->setPersistentCookiesPolicy(
      QWebEngineProfile::ForcePersistentCookies);
  updateWebSettings();
  connect(m_webProfile, &QWebEngineProfile::downloadRequested, this,
          &GeminiMainWindow::onDownloadRequested);

  m_webPage = new PermissiveWebEnginePage(m_webProfile, this, m_permConfig);
  m_webView = new QWebEngineView(this);
  m_webView->setPage(m_webPage);
  setCentralWidget(m_webView);
  m_webView->load(QUrl(QStringLiteral("https://gemini.google.com/")));

  initTray();

  if (m_winConfig->getBool(QStringLiteral("rememberWindow")) &&
      !m_winConfig->getByteArray(QStringLiteral("geometry")).isEmpty()) {
    restoreGeometry(m_winConfig->getByteArray(QStringLiteral("geometry")));
  }

  if (!silentMode) {
    show();
  }
}

GeminiMainWindow::~GeminiMainWindow() {
  delete m_startConfig;
  delete m_permConfig;
  delete m_winConfig;
}

QIcon GeminiMainWindow::getAppIcon() const {
  const QString sysIcon =
      QStringLiteral("/usr/share/icons/hicolor/24x24/apps/qgemini.svg");
  if (QFile::exists(sysIcon)) {
    return QIcon(sysIcon);
  }
  return QIcon(QStringLiteral(":/icons/qgemini.svg"));
}

void GeminiMainWindow::updateWebSettings() {
  m_webProfile->settings()->setAttribute(
      QWebEngineSettings::JavascriptCanAccessClipboard,
      m_permConfig->getBool(QStringLiteral("clipboard")));
}

void GeminiMainWindow::initTray() {
  m_trayIcon = new QSystemTrayIcon(getAppIcon(), this);
  m_trayIcon->setToolTip(QStringLiteral("QGemini"));
  m_trayIcon->setVisible(true);

  auto *trayMenu = new QMenu(this);

  m_rememberAction = new QAction(m_lang.remember, this);
  m_rememberAction->setCheckable(true);
  m_rememberAction->setChecked(
      m_winConfig->getBool(QStringLiteral("rememberWindow")));
  connect(m_rememberAction, &QAction::toggled, this, [this](bool checked) {
    m_winConfig->set(QStringLiteral("rememberWindow"), checked);
  });
  trayMenu->addAction(m_rememberAction);

  trayMenu->addSeparator();

  auto *silentAction = new QAction(m_lang.silent, this);
  silentAction->setCheckable(true);
  silentAction->setChecked(m_startConfig->getBool(QStringLiteral("silent")));
  connect(silentAction, &QAction::toggled, this, [this](bool checked) {
    toggleStartSetting(QStringLiteral("silent"), checked);
  });
  trayMenu->addAction(silentAction);

  auto *autostartAction = new QAction(m_lang.autostart, this);
  autostartAction->setCheckable(true);
  autostartAction->setChecked(
      m_startConfig->getBool(QStringLiteral("autostart")));
  connect(autostartAction, &QAction::toggled, this, [this](bool checked) {
    toggleStartSetting(QStringLiteral("autostart"), checked);
  });
  trayMenu->addAction(autostartAction);

  trayMenu->addSeparator();

  auto *permMenu = new QMenu(m_lang.permissions, this);

  auto addPermAction = [this, permMenu](const QString &text,
                                        const QString &key) {
    auto *act = new QAction(text, this);
    act->setCheckable(true);
    act->setChecked(m_permConfig->getBool(key));
    connect(act, &QAction::toggled, this,
            [this, key](bool checked) { setPermission(key, checked); });
    permMenu->addAction(act);
  };

  addPermAction(m_lang.microphone, QStringLiteral("microphone"));
  addPermAction(m_lang.notifications, QStringLiteral("notifications"));
  addPermAction(m_lang.clipboard, QStringLiteral("clipboard"));
  addPermAction(m_lang.location, QStringLiteral("location"));

  trayMenu->addMenu(permMenu);

  trayMenu->addSeparator();

  auto *reloadAction = new QAction(m_lang.reload, this);
  connect(reloadAction, &QAction::triggered, m_webView,
          &QWebEngineView::reload);
  trayMenu->addAction(reloadAction);

  auto *quitAction = new QAction(m_lang.exit, this);
  connect(quitAction, &QAction::triggered, this, &GeminiMainWindow::quitApp);
  trayMenu->addAction(quitAction);

  m_trayIcon->setContextMenu(trayMenu);
  connect(m_trayIcon, &QSystemTrayIcon::activated, this,
          &GeminiMainWindow::onTrayActivated);
}

void GeminiMainWindow::toggleStartSetting(const QString &key, bool checked) {
  m_startConfig->set(key, checked);
  if (key == QStringLiteral("autostart")) {
    if (checked) {
      DesktopUtils::createAutostartLink();
    } else {
      DesktopUtils::removeAutostartLink();
    }
  } else if (key == QStringLiteral("silent")) {
    DesktopUtils::updateUserDesktopExec(checked ? QStringLiteral("--silent")
                                                : QString());
  }
}

void GeminiMainWindow::setPermission(const QString &key, bool checked) {
  m_permConfig->set(key, checked);
  if (key == QStringLiteral("clipboard")) {
    updateWebSettings();
  }
}

void GeminiMainWindow::onTrayActivated(
    QSystemTrayIcon::ActivationReason reason) {
  if (reason == QSystemTrayIcon::Trigger) {
    if (isVisible()) {
      hide();
    } else {
      show();
      raise();
      activateWindow();
    }
  }
}

void GeminiMainWindow::onDownloadRequested(
    QWebEngineDownloadRequest *download) {
  QString path = QFileDialog::getSaveFileName(this, m_lang.saveFile,
                                              download->downloadFileName());
  if (!path.isEmpty()) {
    QFileInfo fi(path);
    download->setDownloadDirectory(fi.absolutePath());
    download->setDownloadFileName(fi.fileName());
    download->accept();
  } else {
    download->cancel();
  }
}

void GeminiMainWindow::closeEvent(QCloseEvent *event) {
  if (m_winConfig->getBool(QStringLiteral("rememberWindow"))) {
    m_winConfig->set(QStringLiteral("geometry"), saveGeometry());
  }
  hide();
  event->ignore();
}

void GeminiMainWindow::quitApp() { QApplication::quit(); }
