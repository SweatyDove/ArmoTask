#include "myserver.hpp"
#include "ui_myserver.h"
#include "displayimage.hpp"


namespace my_signals {

QString    FILE_SIZE_RECEIVED          {"FILE_SIZE_RECEIVED"};
QString    FILE_EXTENSION_RECEIVED     {"FILE_EXTENSION_RECEIVED"};
QString    FILE_RECEIVED               {"FILE_RECEIVED"};

}

// #### Конструктор класса Server
// ####
MyServer::MyServer(QWidget *parent) :
    QMainWindow         {parent},
    mb_userInterface    {new Ui::MyServer}
{
    // ## Инициализируем графический интерфейс сервера (окно с выбором порта) и связываем
    // ## СИГНАЛ от нажатия клавиши [Launch] с функцией-СЛОТОМ @launchServer()
    mb_userInterface->setupUi(this);
    connect(mb_userInterface->launchButton, SIGNAL(clicked()), this, SLOT(launchServer()));
}

// #### Деструктор класса
// ####
MyServer::~MyServer()
{
    delete mb_userInterface;
    if (mb_server != nullptr) {
        delete mb_server;
    }
    else {} // Nothing to do
}

// #### Функция-СЛОТ, запускающая сервер
// ####
void MyServer::launchServer()
{
    QString     portNumberStr   {mb_userInterface->portNumberEdit->text()};
    int         portNumber      {portNumberStr.toInt()};

    // Номер порта на время отладки
    // portNumber = 1234;


    // #1 Инициализируем сервер
    mb_server = new QTcpServer(this);

    // ## Запускаем сервер на прослушивание всех айпишников на указанном порту
    if (!mb_server->listen(QHostAddress::Any, portNumber)) {
        qDebug() << "[SERVER]: could not start!";
    }
    else {
        qDebug() << "[SERVER]: has successfully launched!";
        // В случае удачного запуска сервера ждём запрос на соединение от клиента.
        // Как только появляется доступное соединение, испускается СИГНАЛ, который будет ловить
        // функция-СЛОТ setConnection()
        connect(mb_server, SIGNAL(newConnection()), this, SLOT(setConnection()));
    }

    return;
}



// #### Ниже определена функция-СЛОТ, отрабатывающая в случае появления доступного соединения
// ####
void MyServer::setConnection()
{    

    // ## Инициализируем новое соединение
    QTcpSocket* socket {mb_server->nextPendingConnection()};
    if (socket != 0) {

        QString peerIPv4 {socket->peerAddress().toString()};
        QString peerPort {QString::number(socket->peerPort())};
        qDebug() << "[SERVER]: Connection with the client: [" << peerIPv4 + " : " + peerPort << "] has established!";

        // Тут нужно как-то закрыть сокет.
        while (QHostAddress::Null != socket->peerAddress()) {
            if (socket->waitForReadyRead(1000)) {
                MyServer::getImage(socket);
            }
            else {}
        }
        qDebug() << "[SERVER]: Connection with the client: [" << peerIPv4 + " : " + peerPort << "] has closed!";

    }
    else {
        qDebug() << "[SERVER]: couldn't establish a connection...";
    }

    return;
}



void MyServer::getImage(QTcpSocket* socket)
{
    QByteArray  buffer;                 // Буфер для отправки/получения данных через сокет

    // #1 Получить размер, который нужно выделить для файла
    buffer.clear();
    //socket->waitForReadyRead();
    buffer = socket->readAll();

    qint64 fileSize {buffer.toLongLong()};
    if (fileSize == 0) {
        qDebug() << "[SERVER]: didn't get correct file size.";
        return;
    }
    else {
        // В случае успеха, отправляем клиенту сигнал, что размер файла получен
        qDebug() << "[SERVER]: has received file size:" << fileSize << "bytes";
        buffer.clear();
        buffer.append(my_signals::FILE_SIZE_RECEIVED.toLatin1());
        socket->write(buffer);
        socket->waitForBytesWritten();
    }

    // #2 Получить расширение файла
    buffer.clear();
    socket->waitForReadyRead();
    buffer = socket->readAll();
    QString fileExtension {QString::fromLatin1(buffer)};
    if (fileExtension.size() == 0) {
        qDebug() << "[SERVER]: didn't get file extension.";
        return;
    }
    else {
        // В случае успеха, отправляем клиенту сигнал, что расширение файла получено
        qDebug() << "[SERVER]: has received file extension: " << fileExtension;
        buffer.clear();
        buffer.append(my_signals::FILE_EXTENSION_RECEIVED.toLatin1());
        socket->write(buffer);
        socket->waitForBytesWritten();
    }

    // #3 Создать массив QByteArray указанной длины
    QByteArray fileBuffer;

    // #4 Заполнить массив
    qint64 totalBytesRead {0};


    while (totalBytesRead < fileSize) {

        if (true == socket->waitForReadyRead()) {
            fileBuffer.append(socket->readAll());
            totalBytesRead = fileBuffer.size();
        }
        else {} // Nothing to do

    }

    if (totalBytesRead < fileSize) {
        qDebug() << "[SERVER]: didn't receive whole file!";
        qDebug() << "          Declared file size = " << fileSize << "bytes.";
        qDebug() << "          Received file size = " << totalBytesRead << "bytes.";
        return;
    }
    else {
        // В случае успеха, отправляем клиенту сигнал, что файл получен
        qDebug() << "[SERVER]: the file successfully received!";

        buffer.clear();
        buffer.append(my_signals::FILE_RECEIVED.toLatin1());
        socket->write(buffer);
        socket->waitForBytesWritten();
    }

    // #5 Отображаем картинку в новом окне
    DisplayImage image {nullptr, fileBuffer, fileExtension.toStdString().c_str()};
    image.exec();


}
