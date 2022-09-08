#ifndef SENDTASKWINDOW_HPP
#define SENDTASKWINDOW_HPP

#include <QDialog>
#include <QFileDialog>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include "clientwindow.hpp"

namespace Ui {
class SendTaskWindow;
}

class SendTaskWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SendTaskWindow(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~SendTaskWindow();

    bool sendImage(QString fileName);

private:
    Ui::SendTaskWindow *ui;
    QTcpSocket* mb_socket;

private slots:
    void browse();
};

#endif // SENDTASKWINDOW_HPP
