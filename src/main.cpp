#include "PawalWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PawalWidget w;
    w.show();
    return a.exec();
}
