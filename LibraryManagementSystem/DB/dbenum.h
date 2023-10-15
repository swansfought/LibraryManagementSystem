#ifndef DBENUM_H
#define DBENUM_H


// @brief: 人员类型
enum class IdentifyType
{
    Student = 0,
    Teacher,
    Bookmanager,
    Manager,
    Visitor
};

//针对图书管理员操作去屏蔽的显示主要数据
enum class ReviewType{
    Manager = 0,
    All,
    Borrow,
    Return,
    Renew,
    Loss,
    Refuse
};

enum class RecordType{
    Manager = 0,
    Current,
    Overdue,
    Loss,
    History
};

enum class Table{
    Apply = 0,
    Books,
    Records,
    Managers,
    Users
};



#endif // DBENUM_H
