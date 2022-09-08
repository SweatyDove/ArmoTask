#include "clientwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientWindow ClientWindow;
    ClientWindow.show();

    return a.exec();
}
