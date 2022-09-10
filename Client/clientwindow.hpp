#ifndef ClientWindow_HPP
#define ClientWindow_HPP


#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>


// #### Пространство имён для графического интерфейса
QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE


// #### Класс описывает функционал главного окна для клиентской части приложения
// ####
class ClientWindow : public QMainWindow {

// #### Объявляем макрос, к-й нужен для работы с Сигналами/Слотами
Q_OBJECT

private:
    Ui::ClientWindow    *mb_userInterface;           // Главное окно клиента
    QTcpSocket          *mb_socket;                  // Сокет для связи с клиентами

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

    // #### Функция для отправки файла на сервер
    bool sendImage(QString fileName);

private slots:
    // ### Функция-слот, срабатывающая при нажатии на кнопку подсоединения к серверу
    void connectToServer();
};
#endif // ClientWindow_HPP
