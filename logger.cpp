#include "logger.h"


Q_LOGGING_CATEGORY(logInfo, "Info")


void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Open stream file writes
    Logger & logger = Logger::create();
    QTextStream out(logger.getData());
    // Write the date of recording
    out << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]");
    // By type determine to what level belongs message
    switch (type)
    {
    case QtInfoMsg:
//        out << " INFO ";
//        out << context.function << " : " << msg << Qt::endl;
        out << msg << Qt::endl;
        break;
    case QtDebugMsg:
//        out << " DEBUG ";
        break;
    case QtWarningMsg:
//        out << " WARNING ";
        break;
    case QtCriticalMsg:
//        out << " CRITICAL ";
        break;
    case QtFatalMsg:
//        out << " FATAL ";
        break;
    }
    // Write to the output category of the message and the message itself

    out.flush();    // Clear the buffered data
}
