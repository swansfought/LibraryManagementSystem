#include "ui/login.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置全局字体
    QFont font;
    font.setFamily("微软雅黑");;
    a.setFont(font);

    Login w;
    w.show();

    return a.exec();
}
