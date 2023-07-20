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
        QString file_name = "logs/logFile-" + QDateTime::currentDateTime().toString("dd-MM-yyyy_hh_mm_ss") + ".txt";
        m_logFile.reset(new QFile(file_name));
        // Open the file logging
        if(!m_logFile.data()->open(QFile::Append | QFile::Text)){
            qDebug() << m_logFile->fileName() << m_logFile->errorString();
        }
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
