#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer();
//    qDebug()<< tcpServer->listen(QHostAddress::Any, 8848);
    if(!tcpServer->listen(QHostAddress::Any, 8848))
    {
        //若出错，则输出错误信息
        qDebug()<<tcpServer->errorString();
        return;
    }
    connect(tcpServer,&QTcpServer::newConnection,this,&TcpServer::server_New_Connect);

}

TcpServer::~TcpServer()
{
    delete ui;
    tcpServer->close();
    delete tcpServer;
    tcpSocket->abort();
    delete tcpSocket;
}

void TcpServer::server_New_Connect()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &TcpServer::socket_Read_Data);
    QObject::connect(tcpSocket, &QTcpSocket::disconnected, this, &TcpServer::socket_Disconnected);
    qDebug() << "connected";
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " connected");
}


void TcpServer::socket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpSocket->readAll();
    while(this->tcpSocket->waitForReadyRead(30))
    {
        qDebug()<<"Data receiving...";
        buffer.append(this->tcpSocket->readAll());
    }
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Got buffer");
    RequestsProcess::process(buffer.toStdString(), this, ui);
}

void TcpServer::socket_Disconnected()
{
    qDebug() << "Disconnected";
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Disconnected");
}

void TcpServer::sendData(QByteArray a)
{
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Send data");
    tcpSocket->write(a);
}
