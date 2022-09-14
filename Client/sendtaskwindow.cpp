#include "sendtaskwindow.hpp"
#include "ui_sendtaskwindow.h"


namespace my_signals {

QString    FILE_SIZE_RECEIVED          {"FILE_SIZE_RECEIVED"};
//QString    FILE_SIZE_NOT_RECEIVED      {"FILE_SIZE_NOT_RECEIVED"};
QString    FILE_EXTENSION_RECEIVED     {"FILE_EXTENSION_RECEIVED"};
//QString    FILE_EXTENSION_NOT_RECEIVED {"FILE_EXTENSION_NOT_RECEIVED"};
QString    FILE_RECEIVED               {"FILE_RECEIVED"};
//QString    FILE_NOT_RECEIVED           {"FILE_NOT_RECEIVED"};

}

// #### Конструктор класса
// ####
SendTaskWindow::SendTaskWindow(QWidget *parent, QTcpSocket* socket) :
    QDialog             {parent},
    mb_userInterface    {new Ui::SendTaskWindow},
    mb_socket           {socket}
{
    // #1 Инициализируем окно интерфейса и связываем СИГНАЛ от нажатия на клавишу "Browse"
    // ## с соответствующей функцией-СЛОТОМ
    mb_userInterface->setupUi(this);
    connect(mb_userInterface->browseButton, SIGNAL(clicked()), this, SLOT(browse()));    

}

// #### Деструктор класса
// ####
SendTaskWindow::~SendTaskWindow()
{
    delete mb_userInterface;
    if (mb_socket != nullptr) {
        delete mb_socket;
    }
    else {} // Nothing to do
}


// #### Функция-СЛОТ, осуществляющая выбор файла из файловой системы для отправки
// ####
void SendTaskWindow::browse()
{
    QString fileName {QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath())};

    // Отправляем файл
    if (!fileName.isEmpty()) {
       qDebug() << "[CLIENT]: you choose file: " << fileName;
       sendFile(fileName);
    }
    else {} // Nothing to do

    //SendTaskWindow::close();
    return;
}


// Отправка файла (с минимальным контролем ошибок на данный момент)
bool SendTaskWindow::sendFile(QString fileName)
{

    QFile*      file    {new QFile(fileName)};      // Указатель на файл
    qint64      portion {2048};                     // Размер порции для отправки через сокет (в байтах)
    QByteArray  buffer;                             // Буффер, который будет отправляться

    //QString     sigFileNameReceived {SIG_FILE_NAME_RECEIVED};   // Сигнал, говорящий о том, что имя файла сервером получено
                                                                // и он готов к принятию самого файла


    // ## Если удалось открыть файл для чтения
    if (file->open(QFile::ReadOnly)) {

        QFileInfo   fileInfo {file->fileName()};


        // #1 Отправляем размер файла на сервер
        qint64  fileSize {fileInfo.size()};
        QString fileSizeStr {QString::number(fileSize)};

        buffer.clear();
        buffer.append(fileSizeStr.toLatin1());
        mb_socket->write(buffer);

        if (true == mb_socket->waitForBytesWritten()) {
            qDebug() << "[CLIENT]: sent file size to the server: " << fileSize;

            // Ждем сигнал от сервера, что им получен размер файла
            buffer.clear();
            mb_socket->waitForReadyRead();
            buffer = mb_socket->readAll();

            QString qStrBuffer {QString::fromLatin1(buffer)};
            //qDebug() << "[CLIENT]: received signal from server: " << qStrBuffer;
            if (0 == QString::compare(qStrBuffer, my_signals::FILE_SIZE_RECEIVED, Qt::CaseSensitive)) {
                qDebug() << "[CLIENT]: server reported, that it had received the file size!";
            }
            else {
                qDebug() << "[CLIENT]: the file size wasn't received by the server!";
                return false;
            }
        }
        else {
            qDebug() << "[CLIENT]: couldn't send file size to the server";
            return false;
        }




        // #2 Отправляем расширение файла
        QString fileExtension {fileInfo.completeSuffix()};


        buffer.clear();
        buffer.append(fileExtension.toLatin1());
        mb_socket->write(buffer);

        if (true == mb_socket->waitForBytesWritten()) {
            qDebug() << "[CLIENT]: sent file extension to the server: " << fileExtension;

            // Ждем сигнал от сервера, что им получено расширение файла
            buffer.clear();
            mb_socket->waitForReadyRead();
            buffer = mb_socket->readAll();

            QString qStrBuffer {QString::fromLatin1(buffer)};
            //qDebug() << "[CLIENT]: received signal from server: " << qStrBuffer;
            if (0 == QString::compare(qStrBuffer, my_signals::FILE_EXTENSION_RECEIVED, Qt::CaseSensitive)) {
                qDebug() << "[CLIENT]: server reported, that it had received the file extension.";
            }
            else {
                qDebug() << "[CLIENT]: the file extension wasn't received by the server!";
                return false;
            }
        }
        else {
            qDebug() << "[CLIENT]: couldn't send file extension to the server";
            return false;
        }

        // #3 Отправляем сам файл как массив байтов
        qint64  writeSize  {0};
        int     attempt {0};
        int     maxAttempt {10};

        while (writeSize < fileSize && attempt < maxAttempt) {
           buffer.clear();

           buffer = file->read(portion);
           mb_socket->write(buffer);
           mb_socket->waitForBytesWritten();

           writeSize += buffer.size();
           attempt = (buffer.size() > 0) ? 0 : attempt + 1;

        }
        qDebug() << "[CLIENT]: file size  = " << fileSize << "bytes.";
        qDebug() << "          bytes sent = " << writeSize << "bytes.";


        // #4 Ждем подтверждение от сервера, что он получил файл
        buffer.clear();
        mb_socket->waitForReadyRead();
        buffer = mb_socket->readAll();

        QString qStrBuffer {QString::fromLatin1(buffer)};
        qDebug() << "[CLIENT]: received signal from server: " << qStrBuffer;
        if (0 == QString::compare(qStrBuffer, my_signals::FILE_RECEIVED, Qt::CaseSensitive)) {
            qDebug() << "[CLIENT]: server reported, that it had successfully received the file!";
        }
        else {
            qDebug() << "[CLIENT]: server didn't report about the file-receiving-status.";
            return false;
        }
    }
    else {
        qDebug() << "[CLIENT]:Can't open the file: " << fileName;
        return false;
    }

    return true;
}
