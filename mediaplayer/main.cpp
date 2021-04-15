#include "mediaplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mediaplayer w;
    w.show();
    return a.exec();
}
