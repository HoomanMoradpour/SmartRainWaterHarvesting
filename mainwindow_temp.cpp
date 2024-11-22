#include "mainwindow_temp.h"
#include "ui_mainwindow_temp.h"

MainWindow_temp::MainWindow_temp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow_temp)
{
    ui->setupUi(this);
}

MainWindow_temp::~MainWindow_temp()
{
    delete ui;
}
