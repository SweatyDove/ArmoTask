#include "clientwindow.hpp"
#include "ui_clientwindow.h"
#include "sendtaskwindow.hpp"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow)
{

    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL (clicked()), this, SLOT(connectToServer()));

}

ClientWindow::~ClientWindow()
{
    delete ui;
}




// Действия при нажатии клавиши присоединения к серверу
void ClientWindow::connectToServer()
{
    QString portNumberStr {ui->portLineEdit->text()};
    int     portNumber    {portNumberStr.toInt()};              // Номер порта

    QString IPv4Str       {ui->IPv4LineEdit->text()};           // IP (семейство IPv4)



    socket = new QTcpSocket(this);

    socket->connectToHost(IPv4Str, portNumber);     // Операция неблокирующая

    // Ждём 3 сек на получение подтверждения соединения
    if (socket->waitForConnected(3000)) {
        qDebug() << "\nSuccessfully connected to server [" << IPv4Str << ':' << portNumber << "].";

          // Инициализируем новое окно
        SendTaskWindow sendTaskWindow {nullptr, socket};
        sendTaskWindow.exec();
//        socket->waitForReadyRead(3000);
//        qDebug() << "Reading: " << socket->bytesAvailable();
//        qDebug() << socket->readAll();

//        socket->write("Hello from client!");
//        socket->waitForBytesWritten(1000);

        socket->close();        
    }
    else {
        qDebug() << "\nCouldn't connect to server [" << IPv4Str << ':' << portNumber << "].";
    }

    return;

}

