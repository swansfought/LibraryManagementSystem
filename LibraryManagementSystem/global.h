#ifndef GLOBAL_H
#define GLOBAL_H

#include <QRegExp>
#include <QRegExpValidator>

#define NUM_REG "^[0-9]+$"
#define PWD_REG "^[A-Za-z0-9]+$"
#define ZH_REG "^[\u4e00-\u9fa5]{0,}$"
#define ZH_EN_NUM_REG "^[A-Za-z\u4E00-\u9FA50-9]+$"
#define EMAIL_REG "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$"
//#define PHONE_REG "/^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/" //??有问题

#define LINE_EDIT_START QString("QLineEdit{background-color: rgb(255, 255, 255);border-radius:4px;border:2px solid gray;padding-left:2px;}")
#define LINE_EDIT_FINISH QString("QLineEdit{background-color: rgb(255, 255, 255);border-radius:4px;border:1px solid gray;padding-left:2px;}")

#endif // GLOBAL_H
