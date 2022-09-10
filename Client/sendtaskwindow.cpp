#include "sendtaskwindow.hpp"
#include "ui_sendtaskwindow.h"

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
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath());

    // Отправляем файл
    if (!fileName.isEmpty()) {
       qDebug() << "You choose " << fileName;
       sendFile(fileName);
    }
    else {} // Nothing to do
}


// Отправка файла (с минимальным контролем ошибок на данный момент)
bool SendTaskWindow::sendFile(QString fileName)
{

    QFile*      file    {new QFile(fileName)};      // Указатель на файл
    qint64      portion {2048};                     // Размер порции для отправки через сокет (в байтах)
    QByteArray  buffer;                             // Буффер, который будет отправляться

    QString     sigFileNameReceived {SIG_FILE_NAME_RECEIVED};   // Сигнал, говорящий о том, что имя файла сервером получено
                                                                // и он готов к принятию самого файла


    // ## Если удалось открыть файл для чтения
    if (file->open(QFile::ReadOnly)) {


        // Отправляем имя файла (пока поддерживаются только имена в виде латиницы)
        QFileInfo   fileInfo {file->fileName()};
        QString     localFileName {fileInfo.fileName()};

        buffer.clear();
        buffer.append(localFileName.toLatin1());
        qDebug() << "Buffer name as C-string: " << buffer;

        mb_socket->write(buffer);
        mb_socket->waitForBytesWritten();

        // Ждём сигнал от сервера, что имя получено
        buffer.clear();
        mb_socket->waitForReadyRead();
        buffer = mb_socket->readAll();

        QString qBuffer {buffer};
        qDebug() << "Received signal from server: " << qBuffer;
        if (0 != QString::compare(qBuffer, sigFileNameReceived, Qt::CaseSensitive)) {
            qDebug() << "File name didn't received by server";
            return false;
        }
        else {} // Nothing to do

        // Если имя сервером получено, отправляем сам файл
        qint64  fileSize        {file->size()};
        qint64  totalBytesRead  {0};

        // Цикл осуществляет чтение-отправку файла порциями.
        while (totalBytesRead < fileSize) {
           buffer.clear();

           buffer = file->read(portion);
           totalBytesRead += buffer.size();

           mb_socket->write(buffer);
           mb_socket->waitForBytesWritten();
        }
        qDebug() << "File successfully transfered!";
        return true;
    }
    else {
        qDebug() << "Can't open file: " << fileName;
        return false;
    }

}
