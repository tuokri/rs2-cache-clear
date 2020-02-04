#include <QApplication>

#include "MainWidget.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWidget mw;
    mw.show();

    qRegisterMetaType<QVector<QString>>("QVector<QString>");

    return QApplication::exec();
}
