#include "clientwindow.hpp"
#include "ui_clientwindow.h"
#include "sendtaskwindow.hpp"


// #### Конструктор класса
// ####
ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow {parent},
    mb_userInterface {new Ui::ClientWindow},
    mb_socket {nullptr}
{
    // #1 Запускаем отображение главного клиентского окна
    mb_userInterface->setupUi(this);

    // #2 Связываем СИГНАЛ от нажатия клавиши на соединение с сервером с функцией-СЛОТОМ,
    // ## которая непосредственно это соединение осуществляет.
    connect(mb_userInterface->connectButton, SIGNAL (clicked()), this, SLOT(connectToServer()));

}

// #### Деструктор класса
// ####
ClientWindow::~ClientWindow()
{
    delete mb_userInterface;
}




// #### Функция-СЛОТ, которая осуществляет алгоритм соединения с сервером
// ####
void ClientWindow::connectToServer()
{
    QString     portNumberStr   {mb_userInterface->portLineEdit->text()};     // Берём строку, содержащюю номер порта в виде строки...
    int         portNumber      {portNumberStr.toInt()};                      // ...и конвертируем эту строку в целое число

    QString     IPv4Str         {mb_userInterface->IPv4LineEdit->text()};                   // IP-адресс же берём в текстовом виде (семейство IPv4)


    // #1 Выделяем под сокет память и устанавливаем через него соединение с сервером
    mb_socket = new QTcpSocket(this);
    mb_socket->connectToHost(IPv4Str, portNumber);

    // #2 Ждём 3 сек на получение подтверждения соединения
    if (mb_socket->waitForConnected(3000)) {
        qDebug() << "\n[CLIENT]: successfully connected to server [" << IPv4Str << ':' << portNumber << "].";

        // В случае успеха - инициализируем объект класса @SendTaskWindow (т.е. новое окно),
        // которое обрабатывает передачу файла на сервер
        SendTaskWindow sendTaskWindow {nullptr, mb_socket};
        //sendTaskWindow.open();
        sendTaskWindow.exec();

        mb_socket->close();
        delete mb_socket;
    }
    else {
        qDebug() << "\nCouldn't connect to server [" << IPv4Str << ':' << portNumber << "].";
    }

    return;

}

