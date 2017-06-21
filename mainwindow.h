#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "openglwidget.h"
#include <QActionGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* instance;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QActionGroup* displayModeAG;
    QActionGroup* shadingModeAG;
    QActionGroup* flatShadingModeAG;
    QActionGroup* textModeAG;

    void createActionGroups();
    void connections();

};

#endif // MAINWINDOW_H
