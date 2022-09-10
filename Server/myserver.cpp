#include "myserver.hpp"
#include "ui_myserver.h"

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


    // #1 Инициализируем сервер
    mb_server = new QTcpServer(this);

    // ## Запускаем сервер на прослушивание всех айпишников на указанном порту
    if (!mb_server->listen(QHostAddress::Any, portNumber)) {
        qDebug() << "Server could not start!";
    }
    else {
        qDebug() << "Server has launched!";
        // В случае удачного запуска сервера ждём запрос на соединение от клиента.
        // Как только появляется доступное соединение, испускается СИГНАЛ, который будет ловить
        // функция-СЛОТ setConnection()
        connect(mb_server, SIGNAL(newConnection()), this, SLOT(setConnection()));
    }

    return;
}



// #### Ниже определена функция-слот, отрабатывающая в случае появления доступного соединения
// ####
void MyServer::setConnection()
{    
    QByteArray  buffer;                 // Буфер для отправки/получения данных через сокет
    qint64      portion {2048};         // Размер порций (в байтах), записываемых в буффер при получении файла


    // #1 Инициализируем новое соединение
    QTcpSocket* socket {mb_server->nextPendingConnection()};
    if (socket != 0) {
        qDebug() << "Connection with the client established!";

        // Сперва получаем имя файла
        buffer.clear();
        socket->waitForReadyRead();
        buffer = socket->readAll();
        QString qBuffer {buffer};
        qDebug() << "Received file name: " << qBuffer;

        // Если имя файла получено - создать новый с таким же именем и открыть его в режиме
        // добавления
        QFile* file = new QFile(qBuffer);
        if (file->open(QFile::Append)) {

            // В случае успеха отправляем сигнал клиенту, что имя получено и готовы
            // принять сам файл
            QString sigFileNameReceived {SIG_FILE_NAME_RECEIVED};

            buffer.clear();
            buffer.append(sigFileNameReceived.toLatin1());
            socket->write(buffer);
            socket->waitForBytesWritten();

            // Теперь записываем непосредственно файл
            while (true) {
                buffer.clear();

                socket->waitForReadyRead();
                buffer = socket->read(portion);
                if (buffer.size() == 0) {
                    break;
                }
                else {
                    file->write(buffer);
                }
            }
            file->close();


            // #### Операции по открытию файла
            // #1 Получить абсолютный путь файла
            QFileInfo fileInfo(file->fileName());
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
        delete file;

        socket->close();
    }
    else {
        qDebug() << "Couldn't establish a connection...";
    }

    return;
}
