#ifndef SENDTASKWINDOW_HPP
#define SENDTASKWINDOW_HPP

#include <QDialog>
#include <QFileDialog>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include "clientwindow.hpp"


#define  SIG_FILE_NAME_RECEIVED     "SIG_FILE_NAME_RECEIVED"        // Сигнал, который сервер отправляет клиенту,
                                                                    // когда получает "имя_файла"


// #### Отдельное пространство имен для графического интерфейса
namespace Ui {
class SendTaskWindow;
}

// #### Класс, реализующий действия по отправке файла на сервер
// ####
class SendTaskWindow : public QDialog
{
    Q_OBJECT

private:
    Ui::SendTaskWindow*     mb_userInterface;
    QTcpSocket*             mb_socket;

public:
    explicit SendTaskWindow(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~SendTaskWindow();

    bool sendFile(QString fileName);


private slots:
    void browse();
};

#endif // SENDTASKWINDOW_HPP
