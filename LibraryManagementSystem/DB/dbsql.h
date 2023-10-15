
#ifndef DBSQL_H
#define DBSQL_H

#include <QObject>
#include <QRegExpValidator>
#include <QVector>
#include <QString>
#include <QTime>
#include <QDebug>

#include "db/dbenum.h"
#include "db/dbtool.h"

class DbSQL : public QObject
{
    Q_OBJECT

public:
    explicit DbSQL(QObject *parent = nullptr);

    //图书浏览-实时搜索+过滤字段的SQL语句
    QString bookBrowseSQL(const QString &input,const QString &bookType,const QString &isBorrowable);

    //借阅记录-实时搜索+过滤字段的SQL语句
    QString readerRecordSQL(const IdentifyType &identifyType,const QString &recordType,const QString &readerType);

    //管理图书-实时搜索+过滤字段的SQL语句
    QString manageBookSQL(const QString &input,const QString &bookType);

    //管理读者-实时搜索+过滤字段的SQL语句
    QString manageReaderSQL(const QString &input,const QString &identify,const QString &sex);

    //管理员工-实时搜索的SQL语句
    QString manageStaffSQL(const QString &input);

};

#endif // DBSQL_H
