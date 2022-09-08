#ifndef ClientWindow_HPP
#define ClientWindow_HPP


#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>

#include <QAbstractSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow {
    // Объевляем макрос, к-й нужен для работы с Сигналами/Слотами
    Q_OBJECT

public:
    QTcpSocket *socket;         // Объект типа Сокет

private:
    Ui::ClientWindow *ui;           // Главное окно клиента



public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

    bool sendImage(QString fileName);

public slots:
    // Nothing

private slots:
    void connectToServer();
};
#endif // ClientWindow_HPP
