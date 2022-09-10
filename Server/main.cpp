#include "myserver.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyServer myServer;
    myServer.show();
    return a.exec();
}
