#include "log.h"

log::log()
{

}

void log::customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") + " Debug: ");
        break;
    case QtWarningMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") + " Warning: ");
        break;
    case QtCriticalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") + " Critical: ");
        break;
    case QtFatalMsg:
        text = QString(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") + " Fatal: ");
        break;
    default:
        break;
    }
    text += str;
    QFile outFile("debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << text << endl;
}
