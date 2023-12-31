
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "serialport.h"
#include "settingsdialog.h"


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

    void handleError(QSerialPort::SerialPortError error);

    void newDataTableHandler(QVector<double> data);

private:
    Ui::MainWindow *ui;

    SerialPort *m_pserial = nullptr;
    SettingsDialog *m_settings = nullptr;

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
