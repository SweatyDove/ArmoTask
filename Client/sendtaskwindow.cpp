#include "sendtaskwindow.hpp"
#include "ui_sendtaskwindow.h"

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
    qint64  portion {1024};
    QByteArray  buffer;

    // Открываем файл для чтения и отправляем файл
    if (image->open(QFile::ReadOnly)) {

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
    else {
        qDebug() << "Can't open file: " << fileName;
        return false;
    }

}
