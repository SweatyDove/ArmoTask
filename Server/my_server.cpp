#include "my_server.hpp"
#include "ui_my_server.h"

#define   DEFAULT_PORT      9000

#include <QMessageBox>
#include <QDir>

// Конструктор класса Server
MyServer::MyServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyServer)
{
    ui->setupUi(this);
    // Связываем сигнал от нажатия клавиши @launchButton с ЭТИМ объектом (MyServer) и его
    // слотом-функцией launchServer().
    connect(ui->launchButton, SIGNAL(clicked()), this, SLOT(launchServer()));

}

MyServer::~MyServer()
{
    delete ui;
    delete server;
}


void MyServer::launchServer()
{

    // Создаём сервер
    server = new QTcpServer(this);


    QString portNumberStr   = ui->portNumberEdit->text();
    int portNumber          = portNumberStr.toInt();

    // Запускаем сервер на прослушивание
    if (!server->listen(QHostAddress::Any, portNumber)) {
        qDebug() << "Server could not start!";
    }
    else {
        qDebug() << "Server has launched!";

        // Есть объект @server класса <QTcpServer * >. У этого объекта есть метод newConnection() (библиотечный).
        // Далее есть мой объект <MyServer> и его метод setConnection(). И как только новое соединение доступно,
        // испускается СИГНАЛ и запускается функция-СЛОТ.
        connect(server, SIGNAL(newConnection()), this, SLOT(setConnection()));
    }

    return;
}



// №№№№ Ниже определена функция-слот, отрабатывающая в случае появления доступного соединения
void MyServer::setConnection()
{    
    QByteArray buffer;
    qint64 portion {1024};

    // №№№№ Определяем новое соединение
    QTcpSocket* socket {server->nextPendingConnection()};
    if (socket != 0) {
        qDebug() << "Connection with the client established!";

        QFile* image = new QFile("receivedImage");
        if (image->open(QFile::Append)) {

            while (true) {
                buffer.clear();

                socket->waitForReadyRead();
                buffer = socket->read(portion);
                if (buffer.size() == 0) {
                    break;
                }
                else {
                    image->write(buffer);
                }
            }

            image->close();

            // #### Операции по открытию файла

            // #1 Получить абсолютный путь файла
            QFileInfo fileInfo(image->fileName());
            QString absFilePath = fileInfo.absoluteFilePath();
            qDebug() << "Absolute file path: " << absFilePath;

            // #2 Конвертировать в нативный путь для данной ОС
            QString nativeAbsFilePath = QDir::toNativeSeparators(absFilePath);
            qDebug() << "Native absolute file path: " << nativeAbsFilePath;

            // #3 Получить из этого пути QURl
            QUrl fileQUrl = QUrl::fromLocalFile(nativeAbsFilePath);
            qDebug() << "QUrl of file: " << fileQUrl;

            // #4 Открыть файл стандартными средствами ОС
            QDesktopServices::openUrl(fileQUrl);

        }
        else {} // Nothing to do

        socket->close();
    }
    else {
        qDebug() << "Couldn't establish a connection...";
    }

    return;
}
