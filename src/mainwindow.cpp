#include "mainwindow.h"

#include <QHBoxLayout>
#include <QWindow>
#include <QCursor>
#include <QPainter>
#include <iostream>

MainWindow::MainWindow()
{
  glRenderer = new GLRenderer{};

  QHBoxLayout *container = new QHBoxLayout{};
//  setCursor(Qt::BlankCursor);
  container->addWidget(glRenderer);

  this->setLayout(container);

}

void MainWindow::finish()
{
  glRenderer->finish();
}


