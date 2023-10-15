
#ifndef DBTOOL_H
#define DBTOOL_H

#include <QDebug>
#include <QMutexLocker>
#include <QSqlDatabase> //数据库
#include <QSqlTableModel> //数据表模型
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSqlError>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArray>

#include "db/dbenum.h"

class DBTool
{

public:
    static DBTool* getInstance(){ return dbTool;}

    bool loginVertify(const int &id,const QString &pwd);

//    bool openTable(const Table &table); //使用的是表模型，可读可写
    bool managerQuerySQL(const Table &table,const QString &sql);
    bool querySQL(const Table &table,const QString &sql); //正常显示所有字段
    bool querySQL(const Table &table, const QString &sql,const ReviewType &type); //审批操作不同，显示字段不同
    bool querySQL(const Table &table, const QString &sql,const RecordType &type);//借阅记录类型不同，显示字段不同



    QString encryptData(const QString &pwd);

    //记录登陆着的身份和ID
    int userID;
    IdentifyType userIdentify;
    QString userIdentify_str;

    QSqlDatabase db;//数据库

    //查询模型，根据sql查询去显示具体数据，不可更改数据
    QSqlQueryModel *bookQueryModel;  //
    QSqlQueryModel *reviewQueryModel;  //
    QSqlQueryModel *recordQueryModel;  //
    QSqlQueryModel *recordQueryModel_manager;  //

    QSqlQueryModel *managerBookQueryModel;
    QSqlQueryModel *managerReaderQueryModel;
    QSqlQueryModel *managerStaffQueryModel;  //

    //选择模型
    QItemSelectionModel *reviewSelectModel; //
    QItemSelectionModel *bookSelectModel; //
    QItemSelectionModel *recordSelectModel; //
    QItemSelectionModel *managerBookSelectModel; //
    QItemSelectionModel *managerReaderSelectModel; //
    QItemSelectionModel *managerStaffSelectModel; //
    QItemSelectionModel *userSelectModel; //


private:
    void connectDB(); //连接数据库
    void updateRecordDay(); //更新借阅表中的天数
//    bool connectState; //单数据库操作

private:
    DBTool();
    ~DBTool();
    static QMutex mutex;
    static DBTool *dbTool;

};

#endif // DBTOOL_H













