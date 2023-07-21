
#include "mainwindow.h"

#include <QApplication>
#include "logger.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(RELEASE)
    qSetMessagePattern("[%{time yyyy.MM.dd hh:mm:ss:zzz} %{message}");
    Logger::create();
#else
    qSetMessagePattern("[%{time yyyy.MM.dd hh:mm:ss:zzz} %{type}] %{appname} %{function} - %{message}");
#endif

    MainWindow w;
    w.show();
    return a.exec();
}
