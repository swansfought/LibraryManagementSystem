#include "dbtool.h"

QMutex DBTool::mutex;
DBTool* DBTool::dbTool = new DBTool; //保证只获取一个db实例

DBTool::DBTool()
{
    connectDB();//去连接数据库
}

DBTool::~DBTool()
{
    db.close();//关闭数据库
}

//用户登录验证，返回值为是否登陆成功
bool DBTool::loginVertify(const int &id, const QString &pwd)
{
    QString sql;
    QSqlQuery query;

    QString md5 = encryptData(pwd);

    //1.判断是否为已完善信息的用户(学生/老师)
    sql = QString("SELECT usr_id,usr_password,usr_identify FROM users WHERE usr_id=%1;").arg(QString::number(id));
    query.exec(sql);
    if(query.next() && query.value(1) == md5){
        if("教师" == query.value(2)){
            userIdentify = IdentifyType::Teacher;
            userIdentify_str = "教师";
        }else if("学生" == query.value(2)){
            userIdentify = IdentifyType::Student;
            userIdentify_str = "学生";
        }
        userID = id;
        return true;
    }

    //2.判断是否为图书管理员/管理员
    sql = QString("SELECT man_id,man_password,man_identify FROM managers WHERE man_id=%1;").arg(QString::number(id));
    query.exec(sql);
    if(query.next() && query.value(1) == md5){
        if("图书管理员" == query.value(2)){
            userIdentify = IdentifyType::Bookmanager;
            userIdentify_str = "图书管理员";
        }else if("管理员" == query.value(2)){
            userIdentify = IdentifyType::Manager;
            userIdentify_str = "管理员";
        }
        userID = id;
        return true;
    }
    return false;
}

//图书管理员、管理员查询指定sql语句的数据
bool DBTool::managerQuerySQL(const Table &table, const QString &sql)
{
    switch (table) {
    case Table::Books:{
        managerBookQueryModel->setQuery(sql); //设置查询语句

        //字段显示名
        managerBookQueryModel->setHeaderData(0,Qt::Horizontal,"书号");
        managerBookQueryModel->setHeaderData(1,Qt::Horizontal,"书名");
        managerBookQueryModel->setHeaderData(2,Qt::Horizontal,"作者");
        managerBookQueryModel->setHeaderData(3,Qt::Horizontal,"出版社");
        managerBookQueryModel->setHeaderData(4,Qt::Horizontal,"出版时间");
        managerBookQueryModel->setHeaderData(5,Qt::Horizontal,"余量(本)");
        managerBookQueryModel->setHeaderData(6,Qt::Horizontal,"总数(本)");
        managerBookQueryModel->setHeaderData(7,Qt::Horizontal,"是否可借");
        managerBookQueryModel->setHeaderData(8,Qt::Horizontal,"分类");
        managerBookQueryModel->setHeaderData(9,Qt::Horizontal,"价格");
        managerBookQueryModel->setHeaderData(10,Qt::Horizontal,"录入时间");
        return true;
    }
    case Table::Managers:{
        managerStaffQueryModel->setQuery(sql); //设置查询语句

        //字段显示名
        managerStaffQueryModel->setHeaderData(0,Qt::Horizontal,"编号");
        managerStaffQueryModel->setHeaderData(1,Qt::Horizontal,"姓名");
        managerStaffQueryModel->setHeaderData(2,Qt::Horizontal,"性别");
        managerStaffQueryModel->setHeaderData(3,Qt::Horizontal,"身份");
        managerStaffQueryModel->setHeaderData(4,Qt::Horizontal,"薪水");
        managerStaffQueryModel->setHeaderData(5,Qt::Horizontal,"联系方式");
        managerStaffQueryModel->setHeaderData(6,Qt::Horizontal,"入职时间");
        break;
    }
    case Table::Users:{
        managerReaderQueryModel->setQuery(sql); //设置查询语句

        //字段显示名
        managerReaderQueryModel->setHeaderData(0,Qt::Horizontal,"编号");
        managerReaderQueryModel->setHeaderData(1,Qt::Horizontal,"姓名");
        managerReaderQueryModel->setHeaderData(2,Qt::Horizontal,"性别");
        managerReaderQueryModel->setHeaderData(3,Qt::Horizontal,"身份");
        managerReaderQueryModel->setHeaderData(4,Qt::Horizontal,"超期数");
        managerReaderQueryModel->setHeaderData(5,Qt::Horizontal,"遗失数");
        managerReaderQueryModel->setHeaderData(6,Qt::Horizontal,"学院");
        managerReaderQueryModel->setHeaderData(7,Qt::Horizontal,"专业");
        managerReaderQueryModel->setHeaderData(8,Qt::Horizontal,"班级");
        managerReaderQueryModel->setHeaderData(9,Qt::Horizontal,"联系方式");
        managerReaderQueryModel->setHeaderData(10,Qt::Horizontal,"邮箱");
        managerReaderQueryModel->setHeaderData(11,Qt::Horizontal,"录入时间");
        break;
    }
    default:
        break;
    }
    return false;
}

//查询指定sql语句的数据
bool DBTool::querySQL(const Table &table, const QString &sql)
{
    switch (table) {
    case Table::Books:{
        bookQueryModel->setQuery(sql); //设置查询语句

        //字段显示名
        bookQueryModel->setHeaderData(0,Qt::Horizontal,"书号");
        bookQueryModel->setHeaderData(1,Qt::Horizontal,"书名");
        bookQueryModel->setHeaderData(2,Qt::Horizontal,"作者");
        bookQueryModel->setHeaderData(3,Qt::Horizontal,"出版社");
        bookQueryModel->setHeaderData(4,Qt::Horizontal,"出版时间");
        bookQueryModel->setHeaderData(5,Qt::Horizontal,"余量(本)");
        bookQueryModel->setHeaderData(6,Qt::Horizontal,"总数(本)");
        bookQueryModel->setHeaderData(7,Qt::Horizontal,"是否可借");
        bookQueryModel->setHeaderData(8,Qt::Horizontal,"分类");
        bookQueryModel->setHeaderData(9,Qt::Horizontal,"价格");
        bookQueryModel->setHeaderData(10,Qt::Horizontal,"录入时间");
        return true;
    }
    default:
        break;
    }
    return false;
}


// 连接数据库
void DBTool::connectDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE"); //添加 SQL LITE数据库驱动
    db.setDatabaseName("../DB/data.db");//设置数据库名称，这里采用相对路径
    if(db.open()){
//        connectState = true;
        qDebug()<<"数据库连接成功...";
        updateRecordDay(); //更新借阅表中的天数
    }
}

//更新借阅表中的天数
void DBTool::updateRecordDay()
{
    //获取当前日期
    QString currStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    int currYear = currStr.mid(0,4).toInt();
    int currMonth = currStr.mid(5,2).toInt();
    int currDay = currStr.mid(8,2).toInt();
    QDate currDate(currYear,currMonth,currDay); //当前日期

    QString sql;
    QSqlQuery query,subQuery;
    QDate recordDate; // 记录表中的日期
    int year=0;
    int month=0;
    int day=0;
    int newRemainder=0;

    query.exec("select * from records");
    while (query.next()) {
        //1.过滤遗失图书记录和已还图书记录
        if("是" == query.value("is_return").toString() || "已遗失" == query.value("book_status").toString())
            continue;

        //2.获取借阅记录中的借书日期
        QString borrowDate = query.value("borrow_date").toString();
        year = borrowDate.mid(0,4).toInt();
        month = borrowDate.mid(5,2).toInt();
        day = borrowDate.mid(8,2).toInt();
        recordDate.setDate(year,month,day);

        //3.当天借阅不需要更新
        if(currYear == year && currMonth == month && currDay == day)
            continue;

        //4.计算剩余天数 ==> 剩余天数 = 持书时间 - 已过天数
        QString holdTime = query.value("holdTime").toString();
        newRemainder = holdTime.left(holdTime.length()-1).toInt() - recordDate.daysTo(currDate);

        //5.过滤相同借阅天数
        QString recordRemainder = query.value("remainder").toString();
        if(recordRemainder.left(recordRemainder.length()-1).toInt() == newRemainder)
            continue;

//        qDebug()<<newRemainder;
        //6.正式的开始更新剩余天数
        if(newRemainder < 0){
            newRemainder = qAbs(newRemainder); //取绝对值，存储正数形式，但属于超期借阅
            sql = QString("update records set remainder='%1天',is_overdue='是',book_status='超期中'   "
                          "where reader_id=%2 and book_id=%3;"
                          ).arg(QString::number(newRemainder),
                                QString::number(query.value("reader_id").toInt() ),
                                QString::number(query.value("book_id").toInt()) );

        }else{
            sql = QString("update records set remainder='%1天'  " // ,is_overdue='否'
                          "where reader_id=%2 and book_id=%3;"
                          ).arg(QString::number(newRemainder),
                                QString::number(query.value("reader_id").toInt() ),
                                QString::number(query.value("book_id").toInt()) );
        }
//        qDebug()<<sql;
        if(!subQuery.exec(sql))
            qDebug()<<"借阅天数更新失败...";
    }
}

bool DBTool::querySQL(const Table &table, const QString &sql,const ReviewType &type)
{
    if(Table::Apply == table) {
        QSqlQueryModel *tmpQueryModel;
        if(ReviewType::Refuse == type)
            tmpQueryModel = recordQueryModel; //需要显示在借阅记录视图中
        else if(ReviewType::Manager == type)
            tmpQueryModel = recordQueryModel_manager;
        else
            tmpQueryModel = reviewQueryModel;

        tmpQueryModel->setQuery(sql);
        //字段显示名
        tmpQueryModel->setHeaderData(0,Qt::Horizontal,"读者编号");
        tmpQueryModel->setHeaderData(1,Qt::Horizontal,"读者姓名");
        tmpQueryModel->setHeaderData(2,Qt::Horizontal,"图书编号");
        tmpQueryModel->setHeaderData(3,Qt::Horizontal,"图书名称");
        tmpQueryModel->setHeaderData(4,Qt::Horizontal,"申请时间");
        tmpQueryModel->setHeaderData(5,Qt::Horizontal,"申请天数");

        //不一样的地方
        if(ReviewType::Manager == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"申请类型");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"还书时间");
            tmpQueryModel->setHeaderData(8,Qt::Horizontal,"续借天数");
            tmpQueryModel->setHeaderData(9,Qt::Horizontal,"读者身份");
        }else if(ReviewType::All == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"申请类型");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"还书时间");
            tmpQueryModel->setHeaderData(8,Qt::Horizontal,"续借天数");
        }else if(ReviewType::Return == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"申请类型");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"还书时间");
        }else if(ReviewType::Renew == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"申请类型");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"续借天数");
        }else if(ReviewType::Refuse == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"拒绝原因");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"续借天数");
        }
        return true;
    }
    return false;
}

bool DBTool::querySQL(const Table &table, const QString &sql, const RecordType &type)
{
    if(Table::Records == table) {
        QSqlQueryModel *tmpQueryModel;
        if(RecordType::Manager == type)
            tmpQueryModel = recordQueryModel_manager; //需要显示在借阅记录视图中
        else
            tmpQueryModel = recordQueryModel;
        tmpQueryModel->setQuery(sql);

        //字段显示名
        tmpQueryModel->setHeaderData(0,Qt::Horizontal,"读者编号");
        tmpQueryModel->setHeaderData(1,Qt::Horizontal,"读者姓名");
        tmpQueryModel->setHeaderData(2,Qt::Horizontal,"书籍编号");
        tmpQueryModel->setHeaderData(3,Qt::Horizontal,"书籍名称");
        tmpQueryModel->setHeaderData(4,Qt::Horizontal,"借书时间");
        tmpQueryModel->setHeaderData(5,Qt::Horizontal,"持有时间");

        if(RecordType::Manager == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"剩余时间");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"是否已还");
            tmpQueryModel->setHeaderData(8,Qt::Horizontal,"还书时间");
            tmpQueryModel->setHeaderData(9,Qt::Horizontal,"图书状态");
            tmpQueryModel->setHeaderData(10,Qt::Horizontal,"历史续借");
            tmpQueryModel->setHeaderData(11,Qt::Horizontal,"是否超期");
            tmpQueryModel->setHeaderData(12,Qt::Horizontal,"读者身份");
        }else if(RecordType::Current == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"历史续借");
        }else if(RecordType::Overdue == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"是否已还");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"还书时间");
            tmpQueryModel->setHeaderData(8,Qt::Horizontal,"历史续借");
        }else if(RecordType::Loss == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"历史续借");
        }else if(RecordType::History == type){
            tmpQueryModel->setHeaderData(6,Qt::Horizontal,"是否已还");
            tmpQueryModel->setHeaderData(7,Qt::Horizontal,"还书时间");
            tmpQueryModel->setHeaderData(8,Qt::Horizontal,"图书状态");
            tmpQueryModel->setHeaderData(9,Qt::Horizontal,"历史续借");
            tmpQueryModel->setHeaderData(10,Qt::Horizontal,"是否超期");
        }
        return true;
    }
    return false;
}

QString DBTool::encryptData(const QString &pwd)
{
    QByteArray md5 = QCryptographicHash::hash(pwd.toUtf8(),QCryptographicHash::Md5);
    return QString::fromUtf8(md5.toHex());
}


#if 0
void DBTool::initUserType()
{
    QSqlQuery query;
    QString sql;

    uint32_t num;
    sql = QString("SELECT new_id FROM new_ids;");
    query.exec(sql);
    if(query.last())
        num = query.at() + 1;
    query.first();

     //插入数据
    int id;
    for (int i = 0; i < num; i++)
    {
        QSqlQuery _query;
        QString _sql;

        id = query.value("new_id").toInt();
        query.next();

        //前面两千多为教师
        if(i < 2460)
            _sql = QString("insert into id_type(usr_id,usr_type) values(%1,'教师');").arg(QString::number(id));
        else
            _sql = QString("insert into id_type(usr_id,usr_type) values(%1,'学生');").arg(QString::number(id));
        if (!_query.exec(_sql))
        {
            qDebug()<<"-----------------";
            return;
        }
    }
}
#endif



