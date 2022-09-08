#ifndef SERVER_HPP
#define SERVER_HPP

#include <QMainWindow>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDesktopServices>

//#include <QGraphicsScene>
//#include <QGraphicsView>
//#include <QGraphicsPixmapItem>



QT_BEGIN_NAMESPACE
namespace Ui { class MyServer; }
QT_END_NAMESPACE




class MyServer : public QMainWindow {
    // Объевляем макрос, к-й нужен для работы с Сигналами/Слотами
    Q_OBJECT

public:
    int portNumber;

private:
    Ui::MyServer    *ui;
    QTcpServer      *server;

public:
    MyServer(QWidget *parent = nullptr);
    ~MyServer();

    //QString MyServer::convertUrlToNativeFilePath(const QUrl& urlStylePath) const;


private slots:
    void launchServer();
    void setConnection();

};



#endif // SERVER_HPP
