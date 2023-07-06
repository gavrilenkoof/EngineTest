
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialport.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void serialListHandler();
    void btnConnectionSerial(bool state);
    void consoleInfo(QString const message);

    void resourceErrorHandler();

private:
    Ui::MainWindow *ui;

    SerialPort *m_pserial = nullptr;
};

#endif // MAINWINDOW_H
