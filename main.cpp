# TextEdit
By Qt5.3.
#include "txtedit.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TxtEdit w;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    w.resize(700,800);
    w.show();

    return a.exec();
}
