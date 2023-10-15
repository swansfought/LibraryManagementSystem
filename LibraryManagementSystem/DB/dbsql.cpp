#include "dbsql.h"

DbSQL::DbSQL(QObject *parent) : QObject{parent} { }

// 实时搜索框
QString DbSQL::bookBrowseSQL(const QString &input, const QString &bookType, const QString &isBorrowable)
{
    QString sql;
    if(input.isEmpty()){
        if("所有" == bookType ){
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_borrowable='%1'  "
                          "ORDER BY book_id;").arg(isBorrowable);
        }else {
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_borrowable='%1' AND  "
                          "book_type='%2'  "
                          "ORDER BY book_id;").arg(isBorrowable,bookType);
        }
        return sql;
    }

    //1.判断是否全为数字，即图书ID
    bool isOk;
    input.toInt(&isOk);
    if(isOk)
    {
        //模糊查询图书ID
        if("所有" == bookType ){
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_borrowable='%1' AND  "
                          "book_id like '%%2%'  "
                          "ORDER BY book_id;").arg(isBorrowable,input);
        }else {
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_borrowable='%1' AND  "
                          "book_type='%2' AND  "
                          "book_id like '%%3%'  "
                          "ORDER BY book_id;").arg(isBorrowable,bookType,input);
        }
        return sql;
    }

    //2.模糊查询书名，图书作者，图书出版社
    if("所有" == bookType ){
        sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                  "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                  "FROM books  "
                  "WHERE  "
                  "book_borrowable='%1' AND  "
                  "((book_name LIKE '%%2%') OR (book_author LIKE '%%3%') OR (book_pub LIKE '%%4%'))  "
                  "ORDER BY book_id;").arg(isBorrowable,input,input,input);
    }else {
        sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                      "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                      "FROM books  "
                      "WHERE  "
                      "book_borrowable='%1' AND  "
                      "book_type='%2'  AND  "
                      "((book_name LIKE '%%3%') OR (book_author LIKE '%%4%') OR (book_pub LIKE '%%5%'))  "
                      "ORDER BY book_id;").arg(isBorrowable,bookType,input,input,input);
    }
    return sql;
}

QString DbSQL::readerRecordSQL(const IdentifyType &identifyType, const QString &recordType, const QString &readerType)
{
    QString sql = "";
    if(IdentifyType::Bookmanager == identifyType || IdentifyType::Manager == identifyType)
    {
        if("所有借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                      "remainder,is_return,return_date,book_status,history_renew_day,is_overdue,reader_identify  "
                      "FROM records;";
            }else{
               sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                     "remainder,is_return,return_date,book_status,history_renew_day,is_overdue  "
                     "FROM records  "
                     "WHERE reader_identify='%1';").arg(readerType);
            }
        }
        else if("当前借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                      "remainder,is_return,return_date,book_status,history_renew_day,is_overdue,reader_identify  "
                      "FROM records  "
                      "WHERE book_status='借阅中' OR  book_status='续借中' OR  book_status='超期中';";
            }else{
               sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                             "remainder,is_return,return_date,book_status,history_renew_day,is_overdue  "
                             "FROM records  "
                             "WHERE reader_identify = '%1' "
                             "AND (book_status='借阅中' OR  book_status='续借中' OR  book_status='超期中');").arg(readerType);
             }
        }
        else if("超期借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                      "remainder,is_return,return_date,book_status,history_renew_day,is_overdue,reader_identify  "
                      "FROM records  "
                      "WHERE book_status='超期中';";
            }else {
                sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                             "remainder,is_return,return_date,book_status,history_renew_day,is_overdue  "
                             "FROM records  "
                             "WHERE book_status='超期中' AND reader_identify = '%1';").arg(readerType);
            }
        }
        else if("遗失借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                      "remainder,is_return,return_date,book_status,history_renew_day,is_overdue,reader_identify  "
                      "FROM records  "
                      "WHERE book_status='已遗失';";
            }else {
               sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                             "remainder,is_return,return_date,book_status,history_renew_day,is_overdue  "
                             "FROM records  "
                             "WHERE book_status='已遗失' AND reader_identify='%1';").arg(readerType);
            }
        }
        else if("被拒借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,book_status,renew_day,reader_identify  "
                      "FROM apply "
                      "WHERE apply_type='拒绝';";
            }else {
               sql = QString("SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,book_status,renew_day  "
                             "FROM apply "
                             "WHERE apply_type='拒绝' AND reader_identify='%1';").arg(readerType);
            }
        }
        else if("历史借阅" == recordType){
            if("全部" == readerType){
                sql = "SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                      "remainder,is_return,return_date,book_status,history_renew_day,is_overdue,reader_identify  "
                      "FROM records  "
                      "WHERE book_status='已归还';";
            }else {
                   sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime, "
                                 "remainder,is_return,return_date,book_status,history_renew_day,is_overdue  "
                                 "FROM records  "
                                 "WHERE book_status='已归还' AND reader_identify = '%1';").arg(readerType);
            }
        }
    }
    return sql;
}

QString DbSQL::manageBookSQL(const QString &input, const QString &bookType)
{
    QString sql;
    if(input.isEmpty()){
        if("所有" == bookType ){
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "ORDER BY book_id;");
        }else {
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_type='%1'  "
                          "ORDER BY book_id;").arg(bookType);
        }
        return sql;
    }

    //1.判断是否全为数字，即图书ID
    bool isOk;
    input.toInt(&isOk);
    if(isOk)
    {
        //模糊查询图书ID
        if("所有" == bookType ){
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_id like '%%1%'  "
                          "ORDER BY book_id;").arg(input);
        }else {
            sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                          "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                          "FROM books  "
                          "WHERE  "
                          "book_type='%1' AND  "
                          "book_id like '%%2%'  "
                          "ORDER BY book_id;").arg(bookType,input);
        }
        return sql;
    }

    //2.模糊查询书名，图书作者，图书出版社
    if("所有" == bookType ){
        sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                  "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                  "FROM books  "
                  "WHERE  "
                  "book_name LIKE '%%1%' OR book_author LIKE '%%2%' OR book_pub LIKE '%%3%'  "
                  "ORDER BY book_id;").arg(input,input,input);
    }else {
        sql = QString("SELECT book_id,book_name,book_author,book_pub,book_pub_date,book_store,"
                      "book_total,book_borrowable,book_type,book_price,book_entry_date  "
                      "FROM books  "
                      "WHERE  "
                      "book_type='%1'  AND  "
                      "((book_name LIKE '%%2%') OR (book_author LIKE '%%3%') OR (book_pub LIKE '%%4%'))  "
                      "ORDER BY book_id;").arg(bookType,input,input,input);
    }
    return sql;
}

QString DbSQL::manageReaderSQL(const QString &input, const QString &identify, const QString &sex)
{
    QString sql;
    if(input.isEmpty()){
        if("所有" == identify ){
            if("所有" == sex){
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                              "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                              "FROM users  "
                              "ORDER BY usr_id;");
            }else{
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                               "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                               "FROM users  "
                               "WHERE usr_sex='%1'  "
                               "ORDER BY usr_id;").arg(sex);
            }
        }
        else {
            if("所有" == sex){
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                          "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                          "FROM users  "
                          "WHERE  "
                          "usr_identify='%1'  "
                          "ORDER BY usr_id;").arg(identify);
            }else{
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                               "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                               "FROM users  "
                               "WHERE  "
                               "usr_identify='%1' AND usr_sex='%2'  "
                               "ORDER BY usr_id;").arg(identify,sex);
            }
        }
        return sql;
    }

    //1.判断是否全为数字，即图书ID
    bool isOk;
    input.toInt(&isOk);
    if(isOk)
    {
        //模糊查询图书ID
        if("所有" == identify ){
            if("所有" == sex){
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                              "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                              "FROM users  "
                              "WHERE usr_id like '%%1%'"
                              "ORDER BY usr_id;").arg(input);
            }else{
                 sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                               "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                               "FROM users  "
                               "WHERE usr_sex='%1'  AND  usr_id like '%%2%'"
                               "ORDER BY usr_id;").arg(sex,input);
            }
        }
        else {
           if("所有" == sex){
                sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                              "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                              "FROM users  "
                              "WHERE usr_id like '%%1%' AND usr_identify='%2'"
                              "ORDER BY usr_id;").arg(input,identify);
           }
           else{
                sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                              "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                              "FROM users  "
                              "WHERE usr_sex='%1'  AND  usr_id like '%%2%' AND usr_identify='%3'"
                              "ORDER BY usr_id;").arg(sex,input,identify);
           }
        }
        return sql;
    }

    //2.模糊查询
    if("所有" == identify ){
        if("所有" == sex){
            sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                          "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                          "FROM users  "
                          "WHERE "
                          "(usr_name like '%%1%') OR (usr_sex like '%%1%') OR (usr_identify like '%%1%') OR (usr_college like '%%1%')  "
                          "OR (usr_major like '%%1%') OR (usr_class like '%%1%') OR (usr_phone like '%%1%') OR (usr_email like '%%1%')  "
                          "ORDER BY usr_id;").arg(input);
        }else{
            sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                          "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                          "FROM users  "
                          "WHERE usr_sex='%1' AND  ("
                          "(usr_name like '%%2%') OR (usr_sex like '%%2%') OR (usr_identify like '%%2%') OR (usr_college like '%%2%')  "
                          "OR (usr_major like '%%2%') OR (usr_class like '%%2%') OR (usr_phone like '%%2%') OR (usr_email like '%%2%'))  "
                          "ORDER BY usr_id;").arg(sex,input);
        }
    }
    else {
        if("所有" == sex){
            sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                          "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                          "FROM users  "
                          "WHERE usr_identify='%1' AND ("
                          "(usr_name like '%%2%') OR (usr_sex like '%%2%') OR (usr_identify like '%%2%') OR (usr_college like '%%2%')  "
                          "OR (usr_major like '%%2%') OR (usr_class like '%%2%') OR (usr_phone like '%%2%') OR (usr_email like '%%2%'))  "
                          "ORDER BY usr_id;").arg(identify,input);

        }else{
            sql = QString("SELECT usr_id,usr_name,usr_sex,usr_identify,usr_overdue_num,"
                          "usr_loss_num,usr_college,usr_major,usr_class,usr_phone,usr_email,usr_register_date  "
                          "FROM users  "
                          "WHERE usr_sex='%1' AND  usr_identify='%2' AND ("
                          "(usr_name like '%%3%') OR (usr_sex like '%%3%') OR (usr_identify like '%%3%') OR (usr_college like '%%3%')  "
                          "OR (usr_major like '%%3%') OR (usr_class like '%%3%') OR (usr_phone like '%%3%') OR (usr_email like '%%3%'))  "
                          "ORDER BY usr_id;").arg(sex,identify,input);
        }
    }
    return sql;
}

QString DbSQL::manageStaffSQL(const QString &input)
{
    QString sql;
    if(input.isEmpty()){
        sql = "SELECT man_id,man_name,man_sex,man_identify,man_salary,man_phone,man_entry_date  "
              "FROM managers WHERE man_identify='图书管理员';";
    }else{
        sql = QString("SELECT man_id,man_name,man_sex,man_identify,man_salary,man_phone,man_entry_date "
                      "FROM managers "
                      "WHERE man_identify='图书管理员' AND ("
                      "(man_id like '%%1%') OR (man_name like '%%1%') OR (man_sex like '%%1%') OR (man_salary like '%%1%')   "
                      "OR (man_phone like '%%1%') OR (man_entry_date like '%%1%') )  "
                      ).arg(input);
    }
    return sql;
}




