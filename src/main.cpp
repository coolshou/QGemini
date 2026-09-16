#include "config.h"
#include "mainwindow.h"
#include <QApplication>
#include <QStringList>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("qgemini"));
  app.setOrganizationName(QStringLiteral("qgemini"));
  app.setQuitOnLastWindowClosed(false);

  DesktopUtils::ensureUserDesktopFile();

  bool silentMode = false;
  const QStringList args = app.arguments();
  for (const QString &arg : args) {
    if (arg == QStringLiteral("--silent")) {
      silentMode = true;
      break;
    }
  }

  GeminiMainWindow window(silentMode);

  return app.exec();
}
