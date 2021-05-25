#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class log
{
public:
    log();
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str);
};

#endif // LOG_H
