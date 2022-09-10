#include "sendtaskwindow.hpp"
#include "ui_sendtaskwindow.h"

#define  SIG_FILE_NAME_RECEIVED     "SIG_FILE_NAME_RECEIVED"

SendTaskWindow::SendTaskWindow(QWidget *parent, QTcpSocket* socket) :
    QDialog(parent),
    ui(new Ui::SendTaskWindow),
    mb_socket {socket}
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));

}

SendTaskWindow::~SendTaskWindow()
{
    delete ui;
}


void SendTaskWindow::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

    if (!fileName.isEmpty()) {
       qDebug() << "You choose " << fileName;
       sendImage(fileName);
    }
    else {}
}


// Отправка файла (с минимальным контролем ошибок на данный момент)
bool SendTaskWindow::sendImage(QString fileName)
{

    QFile* image = new QFile(fileName);
    qint64  portion {2048};
    QByteArray  buffer;

    QString sigFileNameReceived {SIG_FILE_NAME_RECEIVED};


    // Открываем файл для чтения и отправляем файл
    if (image->open(QFile::ReadOnly)) {

        buffer.clear();

        // Отправляем имя файла
        QFileInfo fileInfo(image->fileName());
        QString localFileName {fileInfo.fileName()};

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
        // Если имя сервером получено, отправляем сам файл
        else {
            qint64 imageSize {image->size()};
            qint64  totalBytesRead {0};

            while (totalBytesRead < imageSize) {
               buffer.clear();

               buffer = image->read(portion);
               totalBytesRead += buffer.size();

               mb_socket->write(buffer);
               mb_socket->waitForBytesWritten();
            }
            qDebug() << "File successfully transfered!" << fileName;
            return true;
        }
    }
    else {
        qDebug() << "Can't open file: " << fileName;
        return false;
    }

}
