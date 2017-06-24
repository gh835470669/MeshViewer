#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QAction>

MainWindow* MainWindow::instance = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    instance = this;
    setCentralWidget(ui->openGLWidget);
    createActionGroups();
    connections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActionGroups()
{
    displayModeAG = new QActionGroup(this);
    displayModeAG->addAction(ui->actionFill);
    displayModeAG->addAction(ui->actionFilllines);
    displayModeAG->addAction(ui->actionWireframe);
    ui->actionFill->setChecked(true);
    ui->openGLWidget->displayMode = OpenGLWidget::FILL;

    shadingModeAG = new QActionGroup(this);
    shadingModeAG->addAction(ui->actionGouraud);
    shadingModeAG->addAction(ui->actionPhong);
//    ui->actionGouraud->setChecked(true);
//    ui->openGLWidget->shadingMode = OpenGLWidget::GOURAUD;
    ui->actionPhong->setChecked(true);
    ui->openGLWidget->shadingMode = OpenGLWidget::PHONG;

    flatShadingModeAG = new QActionGroup(this);
    flatShadingModeAG->addAction(ui->actionSmooth);
    flatShadingModeAG->addAction(ui->actionFlat);
//    ui->actionFlat->setChecked(true);
//    ui->openGLWidget->flat_flag = true;
    ui->actionSmooth->setChecked(true);
    ui->openGLWidget->flat_flag = false;

    textModeAG = new QActionGroup(this);
    textModeAG->addAction(ui->actionTexture);
    textModeAG->addAction(ui->actionColor);
//    ui->actionColor->setChecked(true);
//    ui->openGLWidget->textureMode = OpenGLWidget::COLOR;
    ui->actionTexture->setChecked(true);
    ui->openGLWidget->textureMode = OpenGLWidget::TEXTURE;
}

void MainWindow::connections()
{
   connect(ui->actionOpenFile, &QAction::triggered, ui->openGLWidget, &OpenGLWidget::openfile);
   connect(displayModeAG, &QActionGroup::triggered, ui->openGLWidget, &OpenGLWidget::onDisplayModeChanged);
   connect(shadingModeAG, &QActionGroup::triggered, ui->openGLWidget, &OpenGLWidget::onShadingModeChanged);
   connect(flatShadingModeAG, &QActionGroup::triggered, ui->openGLWidget, &OpenGLWidget::onFlatShadingModeChanged);
   connect(textModeAG, &QActionGroup::triggered, ui->openGLWidget, &OpenGLWidget::onTextureModeChanged);
}
