
#include "mainwindow.h"

#include <QApplication>
#include "logger.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    qSetMessagePattern("[%{time yyyy.MM.dd hh:mm:ss:zzz} %{type}] %{appname} %{function} - %{message}");
    qSetMessagePattern("[%{time yyyy.MM.dd hh:mm:ss:zzz} %{message}");
//    Logger::create();

    MainWindow w;
    w.show();
    return a.exec();
}
