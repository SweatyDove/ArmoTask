#ifndef SERVER_HPP
#define SERVER_HPP

#include <QMainWindow>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDesktopServices>
#include <QUrl>

#define  SIG_FILE_NAME_RECEIVED     "SIG_FILE_NAME_RECEIVED"


#include <QMessageBox>
#include <QDir>



// #### Отельное пространство имен для графического интерфейса
QT_BEGIN_NAMESPACE
namespace Ui { class MyServer; }
QT_END_NAMESPACE



// #### Класс реализующий функционал по инициализации сервера и принятию им файла
// #### (с последующим автоматическим открытием)
class MyServer : public QMainWindow {

// Объявляем макрос, к-й нужен для работы с Сигналами/Слотами
Q_OBJECT

public:

private:
    Ui::MyServer*   mb_userInterface;
    QTcpServer*     mb_server;
    //int             mb_portNumber;

public:
    MyServer(QWidget *parent = nullptr);
    ~MyServer();


private slots:
    void launchServer();
    void setConnection();

};



#endif // SERVER_HPP
