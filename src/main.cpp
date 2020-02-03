#include <QtWidgets>

#include "MainWidget.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QMainWindow window;

    MainWidget mw;
    mw.show();

    return QApplication::exec();
}
