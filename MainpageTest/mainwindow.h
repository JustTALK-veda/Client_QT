#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QMainWindow>
#include "ui_mainwindow.h"

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QStackedWidget *pagesStack;
    void createNewPage();
    void goToPreviousPage();
};
#endif // MAINWINDOW_H
