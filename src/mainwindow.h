#pragma once

#include <QMainWindow>

#include "glrenderer.h"

class MainWindow : public QWidget
{
    Q_OBJECT

  public:
    MainWindow();
    void finish();

  private:
    GLRenderer *glRenderer;
};
