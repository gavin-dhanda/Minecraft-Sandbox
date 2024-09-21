#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <iostream>

int main(int argc, char *argv[])
{
  // Create a Qt application
  QApplication a(argc, argv);
  QCoreApplication::setApplicationName("Lab 11: Textures & FBOs");
  QCoreApplication::setOrganizationName("CS 1230");
  QCoreApplication::setApplicationVersion(QT_VERSION_STR);

  // Set OpenGL version to 4.1 and context to Core
  QSurfaceFormat fmt;
  fmt.setVersion(4, 1);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  // Create a GUI window
  MainWindow mainWindow;
  mainWindow.resize(1000, 600); // feel free to change this



  // Optional: full-screen in the (unlikely) event that the window is too large
  float desktopArea = QGuiApplication::primaryScreen()->size().width() *
                      QGuiApplication::primaryScreen()->size().height();
  float widgetArea = mainWindow.width() * mainWindow.height();
  if ((widgetArea / desktopArea) < 0.95f) {
    mainWindow.show();
  } else {
    mainWindow.showMaximized();
  }

  // Call cleanup function; we do not use destructors (see: rule of 3/5/0)
  int returnValue = a.exec();
  mainWindow.finish();

  return returnValue;
}
