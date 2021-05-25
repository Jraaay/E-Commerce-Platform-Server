#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    show();
    tcpServer = new QTcpServer();
    if(!tcpServer->listen(QHostAddress::Any, 8848))
    {
        //若出错，则输出错误信息
        qDebug()<<tcpServer->errorString();
        return;
    }
    connect(tcpServer,&QTcpServer::newConnection,this,&TcpServer::server_New_Connect);
    ifstream infile;
    string sellerJson = "";
    try // 读取消费者、商家和最大uid文件
    {
        infile.open("sellerFile.json");
        infile >> sellerJson;
        infile.close();
    }
    catch (exception &e) // 错误读取处理
    {
        qDebug() << e.what() << endl;
    }
    if (sellerJson == "")
    {
        QString tmp = "";
        QJsonObject object;
        object.insert("data", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("sellerFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
    string consumerJson = "";
    try
    {
        infile.open("consumerFile.json");
        infile >> consumerJson;
        infile.close();
    }
    catch (exception &e) // 错误读取处理
    {
        qDebug() << e.what() << endl;
    }

    if (consumerJson == "")
    {
        QString tmp = "";
        QJsonObject object;
        object.insert("data", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("consumerFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
    string uidMaxJson = "";
    try
    {
        infile.open("uidMaxFile.json");
        infile >> uidMaxJson;
        infile.close();
    }
    catch (exception &e) // 错误读取处理
    {
        qDebug() << e.what() << endl;
    }

    if (uidMaxJson == "")
    {
        uidMaxJson = "";
        const int tmp = 0;
        QJsonObject object;
        object.insert("uid", tmp);
        QJsonDocument document;
        document.setObject(object);
        QByteArray array = document.toJson(QJsonDocument::Compact);
        ofstream outFile;
        outFile.open("uidMaxFile.json");
        outFile << array.toStdString();
        outFile.close();
    }
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
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " connected");
}


void TcpServer::socket_Read_Data()
{
    QByteArray buffer;
    buffer = tcpSocket->readAll();
    while(this->tcpSocket->waitForReadyRead(30))
    {
        buffer.append(this->tcpSocket->readAll());
    }
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Got buffer");
    RequestsProcess::process(buffer.toStdString(), this, ui);
}

void TcpServer::socket_Disconnected()
{
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Disconnected");
}

void TcpServer::sendData(QByteArray a)
{
    QString current_date =QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz");
    ui->textBrowser->append(current_date + " Send data");
    tcpSocket->write(a);
}
