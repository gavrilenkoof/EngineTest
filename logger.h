#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

#include <QLoggingCategory>
#include <QFile>
#include <QDir>
#include <QScopedPointer>


Q_DECLARE_LOGGING_CATEGORY(logInfo)

class Logger : public QObject
{
    Q_OBJECT
public:
//    explicit Logger(QObject *parent = nullptr);
    static Logger & create()
    {
        static Logger log;
        qInstallMessageHandler(log.messageHandler);
        return log;
    }
private:
    Logger()
    {
        // Set the logging file
        // check which a path to file you use
        std::filesystem::create_directory("logs/");
        QString file_name = "logs/logFile-" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
        m_logFile.reset(new QFile(file_name));
        // Open the file logging
        m_logFile.data()->open(QFile::Append | QFile::Text);
    }
    ~Logger()
    {
        m_logFile.data()->close();
    }

    Logger(Logger const&);
    Logger& operator=(Logger const&);

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    QFile* getData() {return static_cast<QFile*>(m_logFile.data());}

    QScopedPointer<QFile>   m_logFile ;

signals:

};

#endif // LOGGER_H
