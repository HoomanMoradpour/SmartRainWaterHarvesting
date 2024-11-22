#ifndef MAINWINDOW_TEMP_H
#define MAINWINDOW_TEMP_H

#include <QMainWindow>

namespace Ui {
class MainWindow_temp;
}

class MainWindow_temp : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_temp(QWidget *parent = nullptr);
    ~MainWindow_temp();

private:
    Ui::MainWindow_temp *ui;
};

#endif // MAINWINDOW_TEMP_H
