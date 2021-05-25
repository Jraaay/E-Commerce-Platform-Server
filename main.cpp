#include "tcpserver.h"
#include "log.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer w;
    w.show();
    return a.exec();
}
