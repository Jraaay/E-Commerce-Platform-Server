#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <requestsprocess.h>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    int a;

public slots:
    void server_New_Connect();
    void socket_Read_Data();
    void socket_Disconnected();
    void sendData(QByteArray a);

private:
    Ui::TcpServer *ui;
};
#endif // TCPSERVER_H
