
#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent) : QWidget(parent),
    dbTool(nullptr),
    dbSql(nullptr),
    isEditingInfo(false),
    isEditingPwd(false),
    isEditingReader(false),
    allowDrag (false),
    movie(nullptr),
    ui(new Ui::Home)
{
    ui->setupUi(this);

    //----------------设置窗口属性---------------
    this->setWindowFlags(Qt::FramelessWindowHint );//| Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowTitle("LibrarySystem");

    //---------------设置窗口阴影---------------
    QGraphicsDropShadowEffect *shadow =  new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);               // 设置阴影距离
    shadow->setColor(QColor(55, 55, 55));  // 设置阴影颜色
    shadow->setBlurRadius(7);              // 设置阴影圆角
    ui->widget_background->setGraphicsEffect(shadow);

    //---------------加载样式表---------------
    QFile file(":/qss/home.qss");
    if(file.open(QFile::ReadOnly)){
        QTextStream stream(&file);
        QString styleSheet = file.readAll();
        this->setStyleSheet(styleSheet);
    }

    ui->widget_drag->installEventFilter(this);//安装事件过滤器，目的是去设置拖动栏
    ui->btn_newPwdVisible->installEventFilter(this);
    ui->btn_suerNewPwdVisible->installEventFilter(this);

    //---------------数据库工具和数据库查询语句工具---------------
    dbTool = DBTool::getInstance();
    dbSql = new DbSQL(this);

    //---------------页面切换信号---------------
    //谁发出的信号，发出的信号是什么，谁接受这个信号，执行相应的操作(不操作也都可以)
    connect(ui->btn_home,&QPushButton::clicked,this,[=]{
        ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->btn_bookBrowse,&QPushButton::clicked,this,[=]{ ui->stackedWidget->setCurrentIndex(1); });
    connect(ui->btn_bookAnalysis,&QPushButton::clicked,this,[=]{  ui->stackedWidget->setCurrentIndex(2); });
    connect(ui->btn_readerRecord,&QPushButton::clicked,this,[=]{  ui->stackedWidget->setCurrentIndex(3); });
    connect(ui->btn_bookReview,&QPushButton::clicked,this,[=]{ ui->stackedWidget->setCurrentIndex(4); });
    connect(ui->btn_manageBook,&QPushButton::clicked,this,[=]{  ui->stackedWidget->setCurrentIndex(5); });
    connect(ui->btn_manageReader,&QPushButton::clicked,this,[=]{ ui->stackedWidget->setCurrentIndex(6); });
    connect(ui->btn_manageStaff,&QPushButton::clicked,this,[=]{ ui->stackedWidget->setCurrentIndex(7); });
    connect(ui->btn_userInfo,&QPushButton::clicked,this,[=]{  ui->stackedWidget->setCurrentIndex(8); });

    //---------------初始化查询数据模型，只读---------------
    dbTool->bookQueryModel = new QSqlQueryModel(this);
    dbTool->reviewQueryModel = new QSqlQueryModel(this);
    dbTool->recordQueryModel = new QSqlQueryModel(this);
    dbTool->recordQueryModel_manager = new QSqlQueryModel(this);//不需要选择模型，只是查看
    dbTool->managerBookQueryModel = new QSqlQueryModel(this);
    dbTool->managerReaderQueryModel = new QSqlQueryModel(this);
    dbTool->managerStaffQueryModel = new QSqlQueryModel(this);

    //---------------初始化查询模型的选择模型---------------
    dbTool->bookSelectModel = new QItemSelectionModel(dbTool->bookQueryModel);
    dbTool->reviewSelectModel = new QItemSelectionModel(dbTool->reviewQueryModel);
    dbTool->recordSelectModel = new QItemSelectionModel(dbTool->recordQueryModel);
    dbTool->managerBookSelectModel = new QItemSelectionModel(dbTool->managerBookQueryModel);
    dbTool->managerReaderSelectModel = new QItemSelectionModel(dbTool->managerReaderQueryModel);
    dbTool->managerStaffSelectModel = new QItemSelectionModel(dbTool->managerStaffQueryModel);


    //---------------绑定tableView的数据模型---------------
    ui->tableView_bookBorrow->setModel(dbTool->bookQueryModel);
    ui->tableView_bookReview->setModel(dbTool->reviewQueryModel);
    ui->tableView_readerRecord->setModel(dbTool->recordQueryModel);
    ui->tableView_readerRecord_manager->setModel(dbTool->recordQueryModel_manager);//不需要选择模型，只是查看
    ui->tableView_manageBook->setModel(dbTool->managerBookQueryModel);
    ui->tableView_manageReader->setModel(dbTool->managerReaderQueryModel);
    ui->tableView_manageStaff->setModel(dbTool->managerStaffQueryModel);

    //---------------设置tableView的选择模型---------------
    ui->tableView_bookBorrow->setSelectionModel(dbTool->bookSelectModel);
    ui->tableView_bookReview->setSelectionModel(dbTool->reviewSelectModel);
    ui->tableView_readerRecord->setSelectionModel(dbTool->recordSelectModel);
    ui->tableView_manageBook->setSelectionModel(dbTool->managerBookSelectModel);
    ui->tableView_manageReader->setSelectionModel(dbTool->managerReaderSelectModel);
    ui->tableView_manageStaff->setSelectionModel(dbTool->managerStaffSelectModel);

    //---------------选择模型的当前项改变---------------
    connect(dbTool->bookSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_bookSelectModel);
    connect(dbTool->reviewSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_reviewSelectModel);
    connect(dbTool->recordSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_recordSelectModel);
    connect(dbTool->managerBookSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_managerBookSelectModel);
    connect(dbTool->managerReaderSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_managerReaderSelectModel);
    connect(dbTool->managerStaffSelectModel,&QItemSelectionModel::currentChanged,this,&Home::on_managerStaffSelectModel);

    //---------------设置home的tableView显示属性---------------
    ui->tableView_bookBorrow->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_bookBorrow->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_bookBorrow->setAlternatingRowColors(true);

    ui->tableView_bookReview->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_bookReview->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_bookReview->setAlternatingRowColors(true);

    ui->tableView_readerRecord->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_readerRecord->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_readerRecord->setAlternatingRowColors(true);

    ui->tableView_readerRecord_manager->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_readerRecord_manager->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_readerRecord_manager->setAlternatingRowColors(true);

    ui->tableView_manageBook->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_manageBook->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_manageBook->setAlternatingRowColors(true);

    ui->tableView_manageReader->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_manageReader->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_manageReader->setAlternatingRowColors(true);

    ui->tableView_manageStaff->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView_manageStaff->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_manageStaff->setAlternatingRowColors(true);

    //---------------图书浏览界面控件数据加载---------------
    typeList << "所有"<< "文学"<< "医学"<< "政治"<< "科幻"<< "经济"
             << "科学"<< "军事"<< "教育"<< "体育"<< "哲学"<< "艺术"
             << "历史"<< "地理"<< "工农业"<< "航空航天"<< "交通运输"
             << "综合图书"<<"其他";
    ui->cBox_bookType->addItems(typeList);//图书浏览的图书类型选择

    //图书浏览的查询过滤条件
    connect(ui->lineEdit_searchBook,&QLineEdit::textChanged,this,&Home::on_bookBrowseQueryChanged);
    connect(ui->cBox_bookType,&QComboBox::currentTextChanged,this,&Home::on_bookBrowseQueryChanged);
    connect(ui->cBox_bookIsBorrowable,&QComboBox::currentTextChanged,this,&Home::on_bookBrowseQueryChanged);

    //---------------审批借阅的查询过滤条件---------------
    connect(ui->radioBtn_reviewAll,&QRadioButton::toggled,this,&Home::on_bookReviewQueryChanged);
    connect(ui->radioBtn_reviewBorrow,&QRadioButton::toggled,this,&Home::on_bookReviewQueryChanged);
    connect(ui->radioBtn_reviewReturn,&QRadioButton::toggled,this,&Home::on_bookReviewQueryChanged);
    connect(ui->radioBtn_reviewRenew,&QRadioButton::toggled,this,&Home::on_bookReviewQueryChanged);
    connect(ui->radioBtn_reviewLoss,&QRadioButton::toggled,this,&Home::on_bookReviewQueryChanged);

    //---------------借阅记录中用户的不同操作---------------
    connect(ui->btn_recordReturnBook,&QPushButton::clicked,this,[=]{ on_recordUserOP("还书"); });
    connect(ui->btn_recordBookApply,&QPushButton::clicked,this,[=]{ on_recordUserOP("续借"); });
    connect(ui->btn_recordBookLoss,&QPushButton::clicked,this,[=]{ on_recordUserOP("遗失"); });
    //借阅记录，管理员页面字段过滤
    connect(ui->cBox_recordType_manager,&QComboBox::currentTextChanged,this,&Home::on_manageRecordQueryChanged);
    connect(ui->cBox_recordReaderType_manager,&QComboBox::currentTextChanged,this,&Home::on_manageRecordQueryChanged);

    //---------------图书管理---------------
    ui->cBox_manageBookType->addItems(typeList); //管理图书的图书类型选择
    ui->cBox_managerbookType->addItems(typeList); //添加图书的类型选择
    ui->cBox_managerbookType->removeItem(0);

    //图书管理的查询过滤条件
    connect(ui->lineEdit_manageSearchBook,&QLineEdit::textChanged,this,&Home::on_manageBookQueryChanged);
    connect(ui->cBox_manageBookType,&QComboBox::currentTextChanged,this,&Home::on_manageBookQueryChanged);

    //---------------读者管理的查询过滤条件---------------
    connect(ui->lineEdit_searchReader,&QLineEdit::textChanged,this,&Home::on_manageReaderQueryChanged);
    connect(ui->cBox_readerIdentify,&QComboBox::currentTextChanged,this,&Home::on_manageReaderQueryChanged);
    connect(ui->cBox_readerSex,&QComboBox::currentTextChanged,this,&Home::on_manageReaderQueryChanged);


    //----------------初始化图表---------------
    //总数的柱状图
    totalChart = new QChart; //创建chart
    totalChart->setAnimationOptions(QChart::SeriesAnimations);
    ui->chartViewBar_totalBook->setChart(totalChart); //为ChartView设置chart
    ui->chartViewBar_totalBook->setRenderHint(QPainter::Antialiasing);

    //单本数的柱状图
    singleChart = new QChart; //创建chart
    singleChart->setAnimationOptions(QChart::SeriesAnimations);
    ui->chartViewBar_singleBook->setChart(singleChart); //为ChartView设置chart
    ui->chartViewBar_singleBook->setRenderHint(QPainter::Antialiasing);

    //图书类型饼状图
    typeChart = new QChart; //创建chart
    ui->chartViewBar_pieBookType->setChart(typeChart); //为ChartView设置chart
    ui->chartViewBar_pieBookType->setRenderHint(QPainter::Antialiasing);

    //---------------其他设置---------------
    ui->stackedWidget->setCurrentIndex(0); //设置默认为第一页
    setInputRegExp(); //设置输入的正则表达式

    // 设置左上角gif图
    movie = new QMovie(":/img/book-1.gif");
    ui->lab_icon->setMovie(movie);
    movie->start();

    //---------------设置主页html---------------
    // 如果想主页显示html就把这条语句取消注释
    ui->webShow->setUrl(QUrl(QStringLiteral("https://jjxy.ujs.edu.cn/"))); //江苏大学京江学院网站
}

Home::~Home()
{
    delete ui;
}

bool Home::eventFilter(QObject *watched, QEvent *event)
{

    if( event->type() == QEvent::Enter) {
        if(dynamic_cast<QWidget*>(watched) == ui->widget_drag){  //设置拖动栏拖动
            allowDrag = true;
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_newPwdVisible){ // 密码可见
            ui->btn_newPwdVisible->setIcon(QIcon(":/img/pwdShow.png"));
            ui->lineEdit_newPwd->setEchoMode(QLineEdit::Normal);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_suerNewPwdVisible){ // 密码可见
            ui->btn_suerNewPwdVisible->setIcon(QIcon(":/img/pwdShow.png"));
            ui->lineEdit_sureNewPwd->setEchoMode(QLineEdit::Normal);
        }
    }
    if(event->type() == QEvent::Leave){
        if(dynamic_cast<QWidget*>(watched) == ui->widget_drag){  //离开拖动栏不可拖动
            allowDrag = false;
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_newPwdVisible){ // 密码不可见
            ui->btn_newPwdVisible->setIcon(QIcon(":/img/pwdClose.png"));
            ui->lineEdit_newPwd->setEchoMode(QLineEdit::Password);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_suerNewPwdVisible){ // 密码不可见
            ui->btn_suerNewPwdVisible->setIcon(QIcon(":/img/pwdClose.png"));
            ui->lineEdit_sureNewPwd->setEchoMode(QLineEdit::Password);
        }
    }

    return QWidget::eventFilter(watched,event);
}

 //@brief: 圆角实现
void Home::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//抗锯齿
    painter.setBrush(QColor(57, 61, 73)); //170,170,170
    painter.setPen(Qt::transparent);//设置透明画笔
    painter.drawRoundedRect(this->rect(), 7, 7);//绘制圆角矩形
    QWidget::paintEvent(event);
}

void Home::mousePressEvent(QMouseEvent *event)
{
    //    setCursor(Qt::ClosedHandCursor);
    winOffset = event->globalPos() - this->pos();
    QWidget::mouseMoveEvent(event);
}

void Home::mouseReleaseEvent(QMouseEvent *event)
{
    //    setCursor(Qt::ArrowCursor);
    winOffset = QPoint();
    QWidget::mouseMoveEvent(event);
}

void Home::mouseMoveEvent(QMouseEvent *event)
{
    //防止按钮拖动导致的奇怪问题
    if(QPoint() == winOffset || !allowDrag)
        return;
    if(this->windowState() == Qt::WindowMaximized){
        this->showNormal();
        winOffset = QPoint();
    }else
        this->move(event->globalPos() - winOffset);
    QWidget::mouseMoveEvent(event);
}

void Home::on_functionLimit()
{
    switch (dbTool->userIdentify) {
    case IdentifyType::Teacher:
    case IdentifyType::Student:{
        if(dbTool->userIdentify == IdentifyType::Teacher)
            ui->lab_currIdentify->setText("当前身份：教师");
        else
            ui->lab_currIdentify->setText("当前身份：学生");

        //dock栏隐藏不必要的
        ui->btn_bookAnalysis->setVisible(false);
        ui->btn_bookReview->setVisible(false);
        ui->btn_manageBook->setVisible(false);
        ui->btn_manageStaff->setVisible(false);
        ui->btn_manageReader->setVisible(false);

        //dock栏显示被其他身份隐藏的
        ui->btn_readerRecord->setVisible(true);
        ui->widget_applyRight->setVisible(true);
        ui->widget_recordRight->setVisible(true);
        ui->btn_recordRefuseLook->setVisible(true);
        ui->lab_recordRefuseLookTip->setVisible(true);

        //个人中心隐藏
        ui->widget_salary->setVisible(false);
        ui->widget_date->setVisible(false);

        //个人中心显示被其他身份隐藏的
        ui->widget_college->setVisible(true);
        ui->widget_major->setVisible(true);
        ui->widget_class->setVisible(true);
        ui->widget_email->setVisible(true);
        ui->gBox_record->setVisible(true);
        ui->btn_userInfo->setVisible(true);
        break;
    }
    case IdentifyType::Bookmanager:{
        ui->lab_currIdentify->setText("当前身份：图书管理员");

        //dock栏隐藏不必要的
        ui->btn_manageStaff->setVisible(false);
        ui->widget_applyRight->setVisible(false);
        ui->widget_recordRight->setVisible(false);
        ui->btn_recordRefuseLook->setVisible(false);
        ui->lab_recordRefuseLookTip->setVisible(false);

        //dock栏显示被其他身份隐藏的
        ui->btn_readerRecord->setVisible(true);
        ui->btn_bookAnalysis->setVisible(true);
        ui->btn_bookReview->setVisible(true);
        ui->btn_manageBook->setVisible(true);
        ui->btn_manageReader->setVisible(true);
        ui->btn_userInfo->setVisible(true);

        //个人中心隐藏
        ui->widget_college->setVisible(false);
        ui->widget_major->setVisible(false);
        ui->widget_class->setVisible(false);
        ui->widget_email->setVisible(false);
        ui->gBox_record->setVisible(false);

        //个人中心显示被其他身份隐藏的
        ui->widget_salary->setVisible(true);
        ui->widget_date->setVisible(true);

        break;
    }
    case IdentifyType::Manager:{
        ui->lab_currIdentify->setText("当前身份：管理员");
         //dock栏隐藏不必要的
//        ui->btn_bookReview->setVisible(false);
        ui->widget_applyRight->setVisible(false);
        ui->widget_recordRight->setVisible(false);
        ui->btn_recordRefuseLook->setVisible(false);
        ui->lab_recordRefuseLookTip->setVisible(false);

        //dock栏显示被其他身份隐藏的
        ui->btn_bookReview->setVisible(true);
        ui->btn_readerRecord->setVisible(true);
        ui->btn_bookAnalysis->setVisible(true);
        ui->btn_manageBook->setVisible(true);
        ui->btn_manageStaff->setVisible(true);
        ui->btn_manageReader->setVisible(true);
        ui->btn_userInfo->setVisible(true);

        //个人中心隐藏
        ui->widget_college->setVisible(false);
        ui->widget_major->setVisible(false);
        ui->widget_class->setVisible(false);
        ui->widget_email->setVisible(false);
        ui->widget_salary->setVisible(false);
        ui->widget_date->setVisible(false);
        ui->gBox_record->setVisible(false);
        break;
    }
    case IdentifyType::Visitor:{ //游客模式
        ui->lab_currIdentify->setText("当前身份：游客");

        //dock栏隐藏不必要的
        ui->btn_readerRecord->setVisible(false);
        ui->btn_bookAnalysis->setVisible(false);
        ui->btn_bookReview->setVisible(false);
        ui->btn_manageBook->setVisible(false);
        ui->btn_manageStaff->setVisible(false);
        ui->btn_manageReader->setVisible(false);
        ui->btn_userInfo->setVisible(false);
        break;
    }
    default:
        break;
    }
}

//图书浏览选择模型的当前项改变响应槽函数
void Home::on_bookSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    ui->spinBox_borrowDay->setValue(0);//清楚已选择天数

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->bookQueryModel->record(current.row());

    // 2.加载数据
    QString bookAuthor = currRecoed.value("book_author").toString();
    int bookId = currRecoed.value("book_id").toInt();
    QString bookName = currRecoed.value("book_name").toString();
    QString bookStore = currRecoed.value("book_store").toString();
    QString bookBorrowable = currRecoed.value("book_borrowable").toString();

    //设置拿到的数据
    ui->lineEdit_borrowBookId->setText(QString::number(bookId));
    ui->lineEdit_borrowBookName->setText(bookName);
    ui->lineEdit_borrowBookAuthor->setText(bookAuthor);
    ui->lineEdit_borrowBookStore->setText(bookStore);
    ui->lineEdit_borrowIsBorrow->setText(bookBorrowable);

    //断书籍余量是否还够
    int store = bookStore.leftRef(bookStore.length() - 1).toInt();
    if(store == 0){
        ui->spinBox_borrowDay->setEnabled(false);
        ui->lab_applyTip->setText("图书已无余量！");
    }else{
        ui->spinBox_borrowDay->setEnabled(true);
        ui->lab_applyTip->clear();
    }
    // 加载图片和图书简介，需要去数据库查
    QString sql = QString("select book_image,book_intro from books where book_id=%1").arg(currRecoed.value( "book_id").toInt() ) ;
    QSqlQuery query;
    if(query.exec(sql) && query.next()){
        // 没有图片就显示默认图片
        QByteArray imageArray = query.value("book_image").toByteArray();
        if(imageArray.isEmpty()){
            ui->lab_borrowBookImage->setPixmap(QPixmap(":img/bookImage.png"));
        }else{
            QPixmap pixmap;
            pixmap.loadFromData(imageArray); //加载二进制数据
            ui->lab_borrowBookImage->setPixmap(pixmap);
        }
        ui->plainText_bookIntro->setPlainText( query.value("book_intro").toString() );
    }
}

void Home::on_reviewSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->reviewQueryModel->record(current.row());
    QString applyType = currRecoed.value("apply_type").toString();
    QString dateTime = currRecoed.value("apply_date").toDateTime().toString("yyyy年MM月dd日");
    ui->lineEdit_reviewApplyDate->setText(dateTime);//申请时间
    ui->lineEdit_reviewApplyType->setText(applyType);//申请类型
    ui->lineEdit_reviewApplyDay->setText(currRecoed.value("apply_day").toString());//申请天数

    QSqlQuery query;
    QString sql;
    int readerId = currRecoed.value("reader_id").toInt();

    //获取剩余天数和持有天数，借书操作无该数据
    if("借书" != applyType){
        sql = QString("SELECT remainder,holdTime FROM records WHERE reader_id=%1;").arg(QString::number(readerId));
        query.exec(sql);
        if(query.next()){
            ui->lineEdit_reviewRemainderDay->setText(query.value("remainder").toString());//剩余天数
            ui->lineEdit_reviewHoldTimeDay->setText(query.value("holdTime").toString());//持有天数
        }
    }
    ui->lineEdit_reviewReaderId->setText(QString::number(readerId));
    ui->lineEdit_reviewReaderName->setText(currRecoed.value("reader_name").toString());

    //获取超期次数和历史遗失
    sql = QString("SELECT usr_overdue_num,usr_loss_num FROM users WHERE usr_id=%1;").arg(QString::number(readerId));
    query.exec(sql);
    if(query.next()){
        ui->lineEdit_reviewOverdueNum->setText( QString::number(query.value("usr_overdue_num").toInt() ));//读者总超期数
        ui->lineEdit_reviewTotalLossNum->setText( QString::number(query.value("usr_loss_num").toInt() ));//读者总遗失数
    }

    //选中项之后才开启同意和拒绝操作
    ui->btn_reviewBookAgree->setEnabled(true);
    if("借书" == applyType || "续借" == applyType)
        ui->btn_reviewBookRefuse->setEnabled(true);
    else
        ui->btn_reviewBookRefuse->setEnabled(false);
}

void Home::on_recordSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    QString sql;
    QSqlQuery query;

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->recordQueryModel->record(current.row());

    // 2.加载数据
    QString bookId = QString::number(currRecoed.value("book_id").toInt());
    QString readerId = QString::number(currRecoed.value("reader_id").toInt());

    ui->lineEdit_recordBookId->setText(bookId);
    ui->lineEdit_recordBookName->setText(currRecoed.value("book_name").toString());
    ui->lineEdit_recordOldDay->setText(currRecoed.value("holdTime").toString());

    //因为限制了显示内容，但是有些数据需要显示到面板，所以需要再次查询
    sql = QString("SELECT * FROM records WHERE reader_id=%1 AND book_id=%2; ").arg(readerId,bookId);
    if(query.exec(sql) && query.next()){
        QString remainder = query.value("remainder").toString();

        //判断读者借阅是为超期
        QString isOverdue = query.value("is_overdue").toString();
        QString bookStatus = query.value("book_status").toString();
        if("是" == isOverdue || "续借中" == bookStatus){
            ui->groupBox_bookRenew->setVisible(false);
            ui->lab_recordIsOverdue->setText( QString("已超%1").arg(query.value("remainder").toString()) );//超期提示
        }else{
            ui->lineEdit_recordStoreDay->setText(remainder);
            ui->groupBox_bookRenew->setVisible(true);
            ui->lab_recordIsOverdue->setText(isOverdue);//超期提示
        }
        ui->lab_recordBookStateTip->setText("图书" + bookStatus); //图书状态提示

    }
    //使能按键
    ui->spinBox_recordAddNewDay->setEnabled(true);
    ui->btn_recordReturnBook->setEnabled(true);
    ui->btn_recordBookLoss->setEnabled(true);
}

void Home::on_managerBookSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->managerBookQueryModel->record(current.row());

    // 2.加载数据
    ui->lineEdit_bookId->setText( QString::number(currRecoed.value("book_id").toInt()) );
    QString bookStore = currRecoed.value("book_store").toString();
    ui->spinBox_bookStore->setValue( bookStore.left(bookStore.length() - 1).toInt() );
    QString bookTotal = currRecoed.value("book_total").toString();
    ui->spinBox_bookTotal->setValue( bookTotal.left(bookTotal.length() - 1).toInt() );

    ui->lineEdit_bookName->setText( currRecoed.value("book_name").toString() );
    ui->lineEdit_bookPub->setText( currRecoed.value("book_pub").toString() );
    ui->lineEdit_bookPubDate->setText( currRecoed.value("book_pub_date").toString() );

    ui->lineEdit_bookAuthor->setText( currRecoed.value("book_author").toString() );
    ui->cBox_managerbookIsBorrow->setCurrentText( currRecoed.value("book_borrowable").toString() );
    ui->cBox_managerbookType->setCurrentText(currRecoed.value("book_type").toString() );

    QString bookPrice = currRecoed.value("book_price").toString();
    if("未知" == bookPrice)
        ui->doubleSpinBox_bookPrice->setValue(0);
    else{
        ui->doubleSpinBox_bookPrice->setValue(  bookPrice.left(bookPrice.length()-1).toDouble() );
    }

    // 加载图片和图书简介，需要去数据库查
    QString sql = QString("select book_image,book_intro from books where book_id=%1").arg(currRecoed.value( "book_id").toInt() ) ;
    QSqlQuery query;
    if(query.exec(sql) && query.next()){
        // 没有图片就显示默认图片
        QByteArray imageArray = query.value("book_image").toByteArray();
        if(imageArray.isEmpty()){
            ui->lab_bookImage->setPixmap(QPixmap(":img/bookImage.png"));
        }else{
            QPixmap pixmap;
            pixmap.loadFromData(imageArray); //加载二进制数据
            ui->lab_bookImage->setPixmap(pixmap);
        }
        ui->plainText_manageBookIntro->setPlainText( query.value("book_intro").toString() );
    }

    ui->btn_manageDelete->setEnabled(true);//允许删除
    ui->btn_manageSave->setEnabled(true);//允许保存

}

void Home::on_managerReaderSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->managerReaderQueryModel->record(current.row());

    // 2.加载数据
    ui->lineEdit_readerId->setText(QString::number(currRecoed.value("usr_id").toInt()) );
    ui->lineEdit_readerName->setText(currRecoed.value("usr_name").toString());
    ui->cBox_managerReaderIdentify->setCurrentText(currRecoed.value("usr_identify").toString());

    QString sex = currRecoed.value("usr_sex").toString();
    if("未知" == sex)
        ui->radioButton_null->setChecked(true);
    else if("女" == sex)
        ui->radioButton_woman->setChecked(true);
    else if("男" == sex)
        ui->radioButton_man->setChecked(true);

    QString lossNum = currRecoed.value("usr_loss_num").toString();
    ui->spinBox_readerLossNum->setValue( lossNum.left(lossNum.length()-1).toInt() );

    QString overdueNum = currRecoed.value("usr_overdue_num").toString();
    ui->spinBox_readerOverdueNum->setValue( overdueNum.left(overdueNum.length()-1).toInt() );

    ui->lineEdit_readerPhone->setText(currRecoed.value("usr_phone").toString());
    ui->lineEdit_readerEmail->setText(currRecoed.value("usr_email").toString());
    ui->lineEdit_readerCollege->setText(currRecoed.value("usr_college").toString());
    ui->lineEdit_readerMajor->setText(currRecoed.value("usr_major").toString());
    ui->lineEdit_readerClass->setText(currRecoed.value("usr_class").toString());
    ui->lineEdit_readerRegisterDate->setText(currRecoed.value("usr_register_date").toString());

    // 加载图片，需要去数据库查
    QString sql = QString("select usr_image from users where usr_id=%1").arg(currRecoed.value( "usr_id").toInt() ) ;
    QSqlQuery query;
    if(query.exec(sql) && query.next()){
        // 没有图片就显示默认图片
        QByteArray imageArray = query.value("usr_image").toByteArray();
        if(imageArray.isEmpty()){
            ui->lab_readerPicture->setPixmap(QPixmap(":img/userImage.png"));
        }else{
            QPixmap pixmap;
            pixmap.loadFromData(imageArray); //加载二进制数据
            ui->lab_readerPicture->setPixmap(pixmap);
        }
    }
    ui->btn_readerSave->setEnabled(true);//允许保存
    ui->btn_readerDelete->setEnabled(true);//允许删除
}

void Home::on_managerStaffSelectModel(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    // 1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->managerStaffQueryModel->record(current.row());

    // 2.加载数据
    ui->lineEdit_staffId->setText( QString::number(currRecoed.value("man_id").toInt()) );
    ui->lineEdit_staffName->setText( currRecoed.value("man_name").toString() );
    QString sex = currRecoed.value("man_sex").toString();
    if("男" == sex)
        ui->cBox_staffSex->setCurrentText("男");
    else if("女" == sex)
        ui->cBox_staffSex->setCurrentText("女");
    else
        ui->cBox_staffSex->setCurrentText("未知");
    ui->doubleSpinBox_salary->setValue( currRecoed.value("man_salary").toDouble() );
    ui->lineEdit_staffPhone->setText( currRecoed.value("man_phone").toString() );
    ui->dateEdit_staffEntryDate->setDateTime( currRecoed.value("man_entry_date").toDateTime() );

    // 加载图片，需要去数据库查
    QString sql = QString("select man_image from managers where man_id=%1").arg(currRecoed.value( "man_id").toInt() ) ;
    QSqlQuery query;
    if(query.exec(sql) && query.next()){
        // 没有图片就显示默认图片
        QByteArray imageArray = query.value("man_image").toByteArray();
        if(imageArray.isEmpty()){
            ui->lab_staffPicture->setPixmap(QPixmap(":img/userImage.png"));
        }else{
            QPixmap pixmap;
            pixmap.loadFromData(imageArray); //加载二进制数据
            ui->lab_staffPicture->setPixmap(pixmap);
        }
    }

    ui->btn_staffDelete->setEnabled(true);//允许删除
    ui->btn_staffSave->setEnabled(true);//允许保存
}

void Home::on_btn_close_clicked()
{
    QMessageBox::StandardButton res;
    res = QMessageBox::question(this,"确认窗口","请确认是否退出图书系统",QMessageBox::Yes|QMessageBox::No,QMessageBox::NoButton);
    if(QMessageBox::Yes == res)
        this->close();
}

void Home::on_btn_max_clicked()
{
    if (this->windowState() == Qt::WindowMaximized)
        this->showNormal();
     else
        this->showMaximized();
}

void Home::on_btn_min_clicked() { this->showMinimized(); }

void Home::on_btn_help_clicked() {}

void Home::on_btn_returnLogin_clicked()
{
    emit userExit();
    ui->btn_home->setChecked(true); //恢复dock栏按钮状态
    ui->stackedWidget->setCurrentIndex(0);//返回主页
    restoreUI();;//恢复UI初始状态
}

/************************************************************************************************************************
 * @brief: 部分2.图书浏览界面，在此借书
 ************************************************************************************************************************/
void Home::on_btn_bookBrowse_clicked()
{
    clearBookBrowse(); //清除历史选择
    on_bookBrowseQueryChanged();
}

void Home::on_spinBox_borrowDay_valueChanged(int arg1)
{
    if(arg1 > 0)
        ui->btn_borrowApply->setEnabled(true);
    else
        ui->btn_borrowApply->setEnabled(false);
}

// 申请借书
void Home::on_btn_borrowApply_clicked()
{
    QString bookId = ui->lineEdit_borrowBookId->text();
    QString readerId = QString::number(dbTool->userID);

    QString sql;
    QSqlQuery query;

    //1.查看读者是否已申请这本书
    sql = QString("select * from apply where reader_id=%1 and book_id=%2;").arg(readerId,bookId);
    query.exec(sql);
    if(query.next()){
        QString bookStatus = query.value("book_status").toString();
        QMessageBox::information(this,"借书提示","借书失败！\n原因是：" + bookStatus,QMessageBox::Ok);
         ui->spinBox_borrowDay->setEnabled(false);
         ui->btn_borrowApply->setEnabled(false);
        return;
    }

    //2.查看读者是否已已借阅这本书
    sql = QString("select * from records where reader_id=%1 and book_id=%2;").arg(readerId,bookId);
    query.exec(sql);
    if(query.next()){
        QString bookStatus = query.value("book_status").toString();
        if("借阅中" == bookStatus || "超期中" == bookStatus){
            QMessageBox::information(this,"借书提示","借书失败！\n原因是：" + bookStatus,QMessageBox::Ok);
            ui->spinBox_borrowDay->setEnabled(false);
            ui->btn_borrowApply->setEnabled(false);
            return;
        }
    }

    //3.真正的开始申请借书
    QString readerName;
    sql = QString("SELECT usr_name FROM users WHERE usr_id=%1;").arg(readerId);
    query.exec(sql);
    if(query.next())
        readerName = query.value("usr_name").toString();//拿到读者姓名
    QString bookName = ui->lineEdit_borrowBookName->text();
    QString applyDay = ui->spinBox_borrowDay->text().remove(QRegExp("\\s"));
    QString applyType = "借书";
    QString bookStatus = "借书审核中";
    sql = QString("INSERT INTO apply(reader_id,reader_name,book_id,book_name,apply_day,apply_type,book_status,reader_identify)  "
                  "VALUES (%1,'%2',%3,'%4','%5','%6','%7','%8');"
                  ).arg(readerId,readerName,bookId,bookName,applyDay,applyType,bookStatus,dbTool->userIdentify_str);
    if(query.exec(sql)){
        QMessageBox::information(this,"借书提示", QString("借书成功！\n图书状态：%1").arg(bookStatus),QMessageBox::Ok);
        ui->spinBox_borrowDay->setEnabled(false);
        ui->btn_borrowApply->setEnabled(false);

    }else{
        qDebug()<<"申请表插入数据失败！！";
        return;
    }

    //4.更新book表
    QString bookStore = ui->lineEdit_borrowBookStore->text(); //余量
    int store = bookStore.leftRef(bookStore.length()-1).toInt() - 1; //计算新的余量
    sql = QString("update books set book_store='%1本' where book_id=%2;").arg(QString::number(store),bookId);
    if(!query.exec(sql))
        qDebug()<<"book表剩余本书更新失败！";

    //5.刷新图书信息
    on_bookBrowseQueryChanged();
}

void Home::on_bookBrowseQueryChanged()
{
    //拿到过滤条件
    QString input = ui->lineEdit_searchBook->text();
    QString isBorrowable = ui->cBox_bookIsBorrowable->currentText();
    QString bookType = ui->cBox_bookType->currentText();

    QString sql = dbSql->bookBrowseSQL(input,bookType,isBorrowable);
    dbTool->querySQL(Table::Books,sql);

    //显示图书数量提示
    int rows = dbTool->bookQueryModel->rowCount();
    ui->lab_bookNumTip->setText(QString("%1条图书记录").arg(QString::number(rows)));
}

void Home::clearBookBrowse()
{
    ui->lineEdit_borrowBookId->clear();
    ui->lineEdit_borrowBookName->clear();
    ui->lineEdit_borrowBookAuthor->clear();
    ui->lineEdit_borrowBookStore->clear();
    ui->lineEdit_borrowIsBorrow->clear();
    ui->spinBox_borrowDay->setValue(0);
    ui->spinBox_borrowDay->setEnabled(false);
    ui->btn_borrowApply->setEnabled(false);
    ui->plainText_bookIntro->clear();
    QPixmap pixmap;
    pixmap.load(":img/bookImage.png");
    ui->lab_borrowBookImage->setPixmap(pixmap); //设置默认图片
}

void Home::clearBookReview()
{
    ui->lineEdit_reviewApplyDate->clear();
    ui->lineEdit_reviewApplyType->clear();
    ui->lineEdit_reviewApplyDay->clear();
    ui->lineEdit_reviewRemainderDay->clear();
    ui->lineEdit_reviewHoldTimeDay->clear();
    ui->lineEdit_reviewReaderId->clear();
    ui->lineEdit_reviewReaderName->clear();
    ui->lineEdit_reviewOverdueNum->clear();
    ui->lineEdit_reviewTotalLossNum->clear();
    ui->btn_reviewBookAgree->setEnabled(false);
    ui->btn_reviewBookRefuse->setEnabled(false);
}

/************************************************************************************************************************
 * @brief: 部分3.图书分析
 ************************************************************************************************************************/
void Home::on_btn_bookAnalysis_clicked()
{
    ui->stackedWidget_analysisBook->setCurrentIndex(0);
    totalBookShow();
    bookNumShow();
    bookTypeShow();
}

void Home::on_btn_goAnalysisBookType_clicked()
{
    ui->stackedWidget_analysisBook->setCurrentIndex(1);
}

void Home::on_btn_exitAnalysisBookType_clicked()
{
     ui->stackedWidget_analysisBook->setCurrentIndex(0);
}

void Home::on_lineEdit_analysisSearchBook_returnPressed()
{
    //1.判断为空情况
    QString input = ui->lineEdit_analysisSearchBook->text();
    if(input.isEmpty())
        return;
    signleBookShow(input); //显示单本图书的借阅情况
}

void  Home::totalBookShow()
{
    //------先释放之前图表中的资源---------
    totalChart->removeAllSeries();//删除所有序列
    totalChart->removeAxis(totalChart->axisX());//删除x轴
    totalChart->removeAxis(totalChart->axisY());//删除y轴

    //创建数据集(柱子)
    QBarSet *totalTotalSet =  new QBarSet("总数");
    QBarSet *totalStoreSet = new QBarSet("余量");
    QBarSet *totalBorrowSet = new QBarSet("借出");
    QBarSet *totalLossSet = new QBarSet("遗失");

    QBarSeries *totalSeries = new QBarSeries; //柱状图序列
    QBarCategoryAxis *totalAxisX = new QBarCategoryAxis;//初始化x轴
    QValueAxis *totalAxisY = new QValueAxis;//初始化y轴

    //从数据库获取图书数据
    QString sql;
    QSqlQuery query,subQuery;

    QStringList bookNameList; //用于x坐标在显示的字符
    query.exec("select * from books;"); //获取数据库中的每一本书籍数据
    while (query.next()) {
        int total = 0, store = 0, borrow = 0, loss = 0;
        QString  bookTotal = query.value("book_total").toString();
        total = bookTotal.leftRef(bookTotal.length() - 1).toInt();//去掉本，即拿到纯数字

        QString  bookStore = query.value("book_store").toString();
        store = bookStore.leftRef(bookStore.length() - 1).toInt();//去掉本，即拿到纯数字

        //查询借阅记录的遗失数量
        sql = QString("select COUNT(book_id) from records  "
                      "where book_id=%1 AND book_status='已遗失';").arg( query.value("book_id").toString() );
        if(subQuery.exec(sql) && subQuery.next())
            loss = subQuery.value(0).toInt();

        borrow = total - store - loss; //计算借出量

        //将总数、余量、借出和遗失数添加到柱状中
        totalTotalSet->append(total);
        totalStoreSet->append(store);
        totalBorrowSet->append(borrow);
        totalLossSet->append(loss);

        bookNameList << query.value("book_name").toString();  //添加用于x轴显示的图书名称
//        qDebug()<<"bookName:"<<query.value("book_name").toString().data()
//                 <<"  total="<<total<<"  store="<<store<<"  borrow="<<borrow<<"  loss="<<loss;
    }

    //加载用于显示的数据
    totalSeries->append(totalTotalSet); //柱状图序列 QBarSeries, 添加三个数据集
    totalSeries->append(totalStoreSet);
    totalSeries->append(totalBorrowSet);
    totalSeries->append(totalLossSet);
//    totalSeries->setLabelsVisible(true);//将值显示到柱子
    totalSeries->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);

    totalChart->addSeries(totalSeries); //添加柱状图序列

    //x轴：用于柱状图的坐标轴
    totalAxisX->setTitleText("图书名称");
    totalAxisX->append(bookNameList); //添加x坐标显示的图书名称列表
    totalChart->setAxisX(totalAxisX, totalSeries); //添加x坐标显示的柱状图序列
    totalAxisX->setRange(bookNameList.at(0), bookNameList.at(7)); //x坐标轴范围，先显示8个，其他的拖动下面的横条就行

    //y轴：数值型坐标作为纵轴
    totalAxisY->setTitleText("图书本数");
    totalAxisY->setTickCount(8);
    totalAxisY->setRange(0, 32);
    totalAxisY->setLabelFormat("%d"); //标签格式
    totalChart->setAxisY(totalAxisY, totalSeries); //y坐标

    totalChart->legend()->setVisible(true); //显示图例
    totalChart->setTheme(QChart::ChartThemeLight);
    totalChart->legend()->setAlignment(Qt::AlignRight); //图例显示在右侧

    //获取书籍总数
    int bookTotal;
    if(query.exec("select count(book_id) from books;") && query.next())
        bookTotal = query.value(0).toInt();

    //滑动栏滑动显示其他数据
    connect(ui->scrollBar_totalBook, &QScrollBar::valueChanged, [=](int value){
        int maxVisible = 7;  // 每次显示的柱状图数量8
        int maxCount = bookTotal-1;

        int min = value;
        int max = min + maxVisible;
        if (max > maxCount) {
            max = maxCount;
            min = max - maxVisible;
        }
        totalAxisX->setRange(bookNameList.at(min), bookNameList.at(max)); //这x坐标轴的范围
    });
}

void  Home::signleBookShow(const QString &input)
{
    //------先释放之前图表中的资源---------
    singleChart->removeAllSeries();//删除所有序列
    singleChart->removeAxis(singleChart->axisX());//删除x轴
    singleChart->removeAxis(singleChart->axisY());//删除y轴

    QString sql;
    QSqlQuery query;

    //输入为纯数字情况
    bool isOk;
    input.toInt(&isOk);
    if(isOk)
        sql = QString("select * from books where book_id=%1;").arg(input);
    else
        sql = QString("select * from books where book_name=%1;").arg(input);

    if(query.exec(sql) && query.next()){
        QString bookName = query.value("book_name").toString();
        ui->lab_analysisBookId->setText( query.value("book_id").toString() );
        ui->lab_analysisBookName->setText( bookName );
        ui->lab_analysisBookAuthor->setText( query.value("book_author").toString() );
        ui->lab_analysisBookType->setText( query.value("book_type").toString() );
        ui->lab_analysisBookIsBorrowble->setText( query.value("book_borrowable").toString() );
        ui->lab_analysisBookPrice->setText( query.value("book_price").toString() );
        ui->lab_analysisBookEntryDate->setText( query.value("book_entry_date").toString() );

        QString bookTotal = query.value("book_total").toString();
        QString bookStore = query.value("book_store").toString();

        //去掉本，拿到整数
        int total = bookTotal.leftRef(bookTotal.length()-1).toInt();
        int store = bookStore.leftRef(bookStore.length()-1).toInt();
        int loss = 0;

        //查询借阅记录的遗失数量
        sql = QString("select COUNT(book_id) from records  "
                      "where book_id=%1 AND book_status='已遗失';").arg( query.value("book_id").toString() );
        if(query.exec(sql) && query.next())
            loss = query.value(0).toInt();

        int borrow = total - store - loss; //计算借出量

        //初始化数据集
        QBarSet *singleTotalSet =  new QBarSet("总数");
        QBarSet *singleStoreSet = new QBarSet("余量");
        QBarSet *singleBorrowSet = new QBarSet("借出");
        QBarSet *singleLossSet = new QBarSet("遗失");

        QBarSeries *singleSeries = new QBarSeries; //柱状图序列
        QBarCategoryAxis *singleAxisX = new QBarCategoryAxis;//初始化x轴
        QValueAxis *singleAxisY = new QValueAxis;//初始化y轴

        //将总数、余量、借出和遗失数添加到柱状中
        singleTotalSet->append(total);
        singleStoreSet->append(store);
        singleBorrowSet->append(borrow);
        singleLossSet->append(loss);

        singleSeries->append(singleTotalSet);
        singleSeries->append(singleStoreSet);
        singleSeries->append(singleBorrowSet);
        singleSeries->append(singleLossSet);
        singleSeries->setLabelsVisible(true);//将值显示到柱子
        singleSeries->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);

        singleChart->addSeries(singleSeries); //添加柱状图序列
        //x轴：用于柱状图的坐标轴
//        singleAxisX->setTitleText("图书名称");
        singleAxisX->append( "" ); //添加x坐标显示的图书名称列表
        singleChart->setAxisX(singleAxisX, singleSeries); //添加x坐标显示的柱状图序列

        //y轴：数值型坐标作为纵轴
//        singleAxisY->setTitleText("图书本数");
        singleAxisY->setTickCount(8);
        singleAxisY->setRange(0, 32);
        singleAxisY->setLabelFormat("%d"); //标签格式
        singleChart->setAxisY(singleAxisY, singleSeries); //y坐标

        singleChart->legend()->setVisible(true); //显示图例
        singleChart->legend()->setAlignment(Qt::AlignRight); //图例显示在右侧
    }
}

void Home::bookNumShow()
{
    QString sql,subSql;
    QSqlQuery query,subQuery;
    //查询库中所有的剩余数量和总数
    int total = 0 ,borrow = 0, store = 0, loss = 0;
    sql = "SELECT book_id,book_store,book_total FROM books;";
    if(query.exec(sql)){
        while(query.next()){
            //查询借阅记录的遗失数量
            subSql = QString("select COUNT(book_id) from records  "
                             "where book_id=%1 AND book_status='已遗失';").arg( QString::number(query.value("book_id").toInt()) );
            if(subQuery.exec(subSql) && subQuery.next())
                loss += subQuery.value(0).toInt();

            QString bookTotal = query.value("book_total").toString();
            QString bookStore = query.value("book_store").toString();
            total += bookTotal.leftRef(bookTotal.length()-1).toInt();
            store += bookStore.leftRef(bookStore.length()-1).toInt();
        }
    }
    borrow = total - store - loss;
    //设置最上面
    ui->lineEdit_analysisTotal->setText(QString::number(total)+"本");
    ui->lineEdit_analysisStore->setText(QString::number(store)+"本");
    ui->lineEdit_analysisBorrow->setText(QString::number(borrow)+"本");
    ui->lineEdit_analysisLoss->setText(QString::number(loss)+"本");
}

//图书类型饼状图分析
void  Home::bookTypeShow()
{
    //------先释放之前图表中的资源---------
    //饼状图没有x，y轴所以不需要释放对应资源
    typeChart->removeAllSeries();

   QPieSeries *typeSeries = new QPieSeries; //饼状图序列
//    typeSeries->setPieSize(0.6); //饼状图大小
    typeSeries->setHoleSize(0.1); //饼状图中心点大小

    QString sql;
    QSqlQuery query;

    //思路：依次按照图书类型去查询数据库，然后计算该类型有多少本书
    //下面三个变量用于临时存放数据
    QString bookTotal;
    QString label;
    QPieSlice *slice;
    for(int i=1; i < typeList.length(); ++i){  //从1开始，不去那所有类型的，因为不存在，没必要检索
        int total = 0; //记录图书总数
        sql = QString("SELECT book_total FROM books WHERE book_type='%1';").arg(typeList.at(i));
        if(!query.exec(sql))
            continue;
        while (query.next()) {
            bookTotal = query.value("book_total").toString();
            total += bookTotal.leftRef(bookTotal.length()-1).toInt();//去掉本
        }

         //初始化数据集
        typeSeries->append(typeList.at(i-1),total);
        slice = typeSeries->slices().at(i-1); //拿到指定位置的扇叶
        //添加要显示的提示文本
        label = QString("%1[%2本]").arg(typeList.at(i), QString::number(total));//QString::number(slice->percentage()*100)
        slice->setLabel(label);
//        slice->setExplodeDistanceFactor(0.5); //扇叶之间的间距
        //鼠标进入扇叶时，扇叶突出显示
        connect(slice,&QPieSlice::hovered,this,[=](bool show){
            QPieSlice* slice = (QPieSlice*)sender();
            slice->setExploded(show);
        });
    }

    typeSeries->setLabelsVisible(true);
    typeChart->addSeries(typeSeries);
    typeChart->setTheme(QChart::ChartThemeQt);
    typeChart->legend()->setVisible(true); //显示图例
    typeChart->legend()->setAlignment(Qt::AlignRight); //图例显示在右侧
}

void Home::flushTotalBookShow()
{

}

/************************************************************************************************************************
 * @brief: 部分4.审批借阅界面，在此审批读者的各种申请
 ************************************************************************************************************************/
void Home::on_btn_bookReview_clicked()
{
    clearBookReview(); //清除历史选择
    on_bookReviewQueryChanged();
}

void Home::on_bookReviewQueryChanged()
{
    QString sql;
    if(ui->radioBtn_reviewAll->isChecked()){
        sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,apply_type,return_date,renew_day  "
              "FROM apply  "
              "WHERE apply_type='借书' OR apply_type='还书' OR apply_type='续借' OR apply_type='遗失';";
        dbTool->querySQL(Table::Apply,sql,ReviewType::All);
    }
    else if(ui->radioBtn_reviewBorrow->isChecked()){
        sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,apply_type  "
              "FROM apply  "
              "WHERE apply_type='借书';";
        dbTool->querySQL(Table::Apply,sql,ReviewType::Borrow);
    }
    else if(ui->radioBtn_reviewReturn->isChecked()){
        sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,apply_type,return_date  "
              "FROM apply  "
              "WHERE apply_type='还书';";
        dbTool->querySQL(Table::Apply,sql,ReviewType::Return);
    }
    else if(ui->radioBtn_reviewRenew->isChecked()){
        sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,apply_type,renew_day  "
              "FROM apply  "
              "WHERE apply_type='续借';";
        dbTool->querySQL(Table::Apply,sql,ReviewType::Renew);
    }
    else if(ui->radioBtn_reviewLoss->isChecked()){
        sql = "SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,apply_type  "
              "FROM apply  "
              "WHERE apply_type='遗失';";
        dbTool->querySQL(Table::Apply,sql,ReviewType::Loss);
    }

    //显示审批借阅数量提示
    int num = dbTool->reviewQueryModel->rowCount();
    ui->lab_recordNumTip->setText(QString("%1条未审批").arg(QString::number(num)));
}

void Home::on_btn_reviewBookAgree_clicked()
{
    QString sql;
    QSqlQuery query;

    //1.从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->reviewQueryModel->record(dbTool->reviewSelectModel->currentIndex().row());

    //2.获取读者必要信息
    QString readerId = ui->lineEdit_reviewReaderId->text();
    QString readerName = ui->lineEdit_reviewReaderName->text();
    QString bookId = QString::number(currRecoed.value("book_id").toInt());
    QString bookName = currRecoed.value("book_name").toString();

    //3.不同操作类型执行不同操作
    QString type = ui->lineEdit_reviewApplyType->text();
    if("借书" == type){
        //查询该读者身份
        QString readerIdentify;
        sql = QString("SELECT usr_identify FROM users WHERE usr_id=%1;").arg(readerId);
        if(query.exec(sql) && query.next())
            readerIdentify = query.value("usr_identify").toString();
        sql = QString("INSERT INTO records(reader_id,reader_name,book_id,book_name,holdTime,remainder,book_status,reader_identify) "
                      "VALUES (%1,'%2',%3,'%4','%5','%6','%7','%8'); ").arg(
                      readerId,readerName,bookId ,bookName,
                      ui->lineEdit_reviewApplyDay->text(),
                      ui->lineEdit_reviewApplyDay->text(),
                      "借阅中",
                      readerIdentify);
        if(query.exec(sql))
            QMessageBox::information(this,"审批操作","已同意读者的借书申请！",QMessageBox::Ok);
        else
            qDebug()<<"借书操作执行失败！";
    }
    else if("还书" == type){
        //1.更新借阅表
        sql = QString("UPDATE records set is_return='是',return_date ='%1',book_status='已归还'  "
                      "WHERE reader_id=%2 AND book_id=%3;").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"),readerId,bookId);
        if(query.exec(sql))
            QMessageBox::information(this,"审批操作","已同意读者的还书申请！",QMessageBox::Ok);
        else
            qDebug()<<"还书操作执行失败！";

        //2.更新图书表，余量+1
        //2.1获取图书余量
        sql = QString("SELECT book_store FROM books WHERE book_id=%1;").arg(QString::number(currRecoed.value("book_id").toInt()));
        int store = 0;
        if(query.exec(sql) && query.next()){
            QString temp = query.value("book_store").toString();
            store = temp.leftRef(temp.length()-1).toInt() + 1 ;
        }
        //2.2更新图书余量
        sql = QString("UPDATE books SET book_store='%1本' WHERE book_id=%2;").arg(QString::number(store),bookId);
        if(!query.exec(sql))
            qDebug()<< "更新图书余量失败！";
    }
    else if("续借" == type){
       QString renewDay = currRecoed.value("renew_day").toString();
       int _renewDay = renewDay.leftRef(renewDay.length() - 1).toInt();

       sql = QString("SELECT holdTime,remainder  "
                     "FROM records "
                     "WHERE reader_id=%1 AND book_id=%2; ").arg(readerId,bookId);

       //计算新的持有天数和剩余天数
       query.exec(sql);
       if(query.exec(sql)){
            if(query.next()){
                QString holdTime,remainder;
                holdTime = query.value("holdTime").toString();
                remainder = query.value("remainder").toString();

                int _holdTime,_remainder;
                _holdTime = holdTime.leftRef(holdTime.length() - 1).toInt() + _renewDay; //去掉字符本，并增加新的申请天数
                _remainder = remainder.leftRef(remainder.length() - 1).toInt() + _renewDay;

                sql = QString("UPDATE records  "
                              "set holdTime='%1天'  AND remainder='%2天'  "
                              "WHERE reader_id=%3 AND book_id=%4;").arg(QString::number(_holdTime),
                               QString::number(_remainder),readerId,bookId);
                if(query.exec(sql))
                    QMessageBox::information(this,"审批操作","已同意读者的续借申请！",QMessageBox::Ok);
            }
       }else
            qDebug()<<"续借操作失败！";
    }
    else if("遗失" == type){
       sql = "UPDATE records  set book_status='已遗失';";
       if(query.exec(sql))
            QMessageBox::information(this,"审批操作","已同意读者的遗失申请！",QMessageBox::Ok);
       else
            qDebug()<<"遗失操作执行失败！";
    }

    //4.清除申请表中的申请，因为该数据已被添加到记录表中
    sql = QString("DELETE FROM apply WHERE reader_id=%1 AND book_id=%2;").arg(readerId,bookId);
    if(!query.exec(sql))
       qDebug()<<"申请表中的数据删除失败！";

    //5.刷新数据
    on_btn_bookReview_clicked();
}

void Home::on_btn_reviewBookRefuse_clicked()
{
    bool isOk = false;
    QString refuseText = QInputDialog::getText(this,"拒绝操作","拒绝原因(字数<250)", QLineEdit::Normal,"",&isOk);
    if(!isOk)
       return;

    if(refuseText.length() > 250)
       refuseText = refuseText.left(250);

    QString sql;
    QSqlQuery query;

    //从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->reviewQueryModel->record(dbTool->reviewSelectModel->currentIndex().row());
    QString readerId = ui->lineEdit_reviewReaderId->text();
    QString bookId = QString::number(currRecoed.value("book_id").toInt());

    //1.更新books表中的余量
    sql = QString("SELECT book_store FROM books WHERE book_id=%1;").arg(bookId);
    if(query.exec(sql)){
       if(query.next()){
            QString store = query.value("book_store").toString();
            int _store = store.leftRef(store.length() - 1).toInt() + 1;

            //更新新余量
            sql = QString("update books set book_store='%1本' where book_id=%2;").arg(QString::number(_store),bookId);
            if(query.exec(sql))
                QMessageBox::information(this,"审批操作","已拒绝读者的申请！",QMessageBox::Ok);
            else
                qDebug()<<"拒绝操作执行失败！";
        }
    }

    //2.更新appky表中的书籍状态，就是把管理员拒绝的理由更新上去
    sql = QString("update apply set apply_type='拒绝',book_status='%1' WHERE reader_id=%2 AND book_id=%3;").arg(refuseText,readerId,bookId);
    if(!query.exec(sql))
        qDebug()<<"申请表中的书籍状态更新失败！";

    //3.刷新数据
    on_btn_bookReview_clicked();
}

/************************************************************************************************************************
 * @brief: 部分5.借阅记录界面，读者在此查看记录、还书、遗失和续借操作
 * @note: 这里读者和管理员又分为两页，读者第一页，管理员第二页
 ************************************************************************************************************************/
void Home::on_btn_readerRecord_clicked()
{
    if(IdentifyType::Teacher == dbTool->userIdentify || IdentifyType::Student == dbTool->userIdentify){
        ui->stackedWidget_record->setCurrentIndex(0);
        clearBookRecord(); //清除上次遗留信息
        on_cBox_recordType_currentTextChanged("");//加载table view中的数据

        QString sql= QString("select count(apply_type)  "
                              "from apply  "
                              "WHERE reader_id=%1 AND apply_type='拒绝';").arg(QString::number(dbTool->userID));
        QSqlQuery query;
        if(query.exec(sql) && query.next())
            ui->lab_recordRefuseLookTip->setText( QString(" %1条历史被拒").arg( QString::number(query.value(0).toInt()) ));
    }else{
        ui->stackedWidget_record->setCurrentIndex(1);
        on_manageRecordQueryChanged();
    }
}

//管理员界面的查询记录改变
void Home::on_manageRecordQueryChanged()
{
    QString type = ui->cBox_recordType_manager->currentText();
    QString readerType = ui->cBox_recordReaderType_manager->currentText();

    QString sql = dbSql->readerRecordSQL(dbTool->userIdentify,type,readerType);//获取sql语句
    if("被拒借阅" == type){
        dbTool->querySQL(Table::Apply,sql,ReviewType::Manager);//被拒记录去申请表中查询
    }else{
        dbTool->querySQL(Table::Records,sql,RecordType::Manager);
    }

    //管理员界面显示借阅数量提示
    int rows = dbTool->recordQueryModel_manager->rowCount();
    ui->lab_recordNum_manager->setText(QString("%1条借阅记录").arg(QString::number(rows)));
}

void Home::on_cBox_recordType_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    QString type = ui->cBox_recordType->currentText();
    QString sql;

    RecordType recordType;//记录操作类型

     //不同操作对应不同sql语句
    if("当前借阅" == type){
        sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime,history_renew_day  "
                      "FROM records "
                      "WHERE reader_id=%1 AND (book_status='续借中' OR book_status='借阅中' OR book_status='超期中');").arg(dbTool->userID);
        recordType = RecordType::Current;
    }
    else if("超期借阅" == type) { //这里只列举出当前借阅中的超期记录
       sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime,is_return,return_date,history_renew_day  "
                      "FROM records WHERE reader_id=%1 AND book_status='超期中';").arg(dbTool->userID);
        recordType = RecordType::Overdue;
    }
    else if("遗失借阅" == type){
        sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime,history_renew_day  "
                     "FROM records WHERE reader_id=%1 AND book_status='已遗失';").arg(dbTool->userID);
        recordType = RecordType::Loss;

    }else if("历史借阅" == type){
       sql = QString("SELECT reader_id,reader_name,book_id,book_name,borrow_date,holdTime,  "
                      "is_return,return_date,book_status,history_renew_day,is_overdue  "
                      "FROM records WHERE reader_id=%1 AND (book_status='已遗失' OR book_status='已归还');").arg(dbTool->userID);
        recordType = RecordType::History;
    }
    else if("被拒借阅" == type){
       //去apply中获取数据显示在当前视图中，并且拿到记录条数去显示借阅记录数量
       sql = QString("SELECT reader_id,reader_name,book_id,book_name,apply_date,apply_day,book_status,renew_day "
                      "FROM apply WHERE reader_id=%1 AND apply_type='拒绝';").arg(dbTool->userID);
       dbTool->querySQL(Table::Apply,sql, ReviewType::Refuse);
       //被拒记录数量
       ui->lab_recordRefuseLookTip->setText(QString("%1条借阅记录").arg(QString::number( dbTool->recordQueryModel->rowCount() )));
       ui->widget_recordRight->setVisible(false); //拒绝的记录没有还书和续借以及遗失操作
    }

    //显示借阅记录数量
    if(dbTool->querySQL(Table::Records,sql,recordType))
       ui->lab_recordNum->setText( QString("%1条借阅记录").arg( QString::number( dbTool->recordQueryModel->rowCount() ) ));

    //显示和隐藏功能
    if("遗失借阅" == type || "历史借阅" == type || "被拒借阅" == type)
       ui->widget_recordRight->setVisible(false);
    else
       ui->widget_recordRight->setVisible(true);
}

void Home::on_recordUserOP(const QString &type)
{
    QString sql;
    QSqlQuery query;
    bool opState = false;

    //从数据模型中拿到当前行数据
    QSqlRecord currRecoed = dbTool->recordQueryModel->record(dbTool->recordSelectModel->currentIndex().row());

    QString bookId = ui->lineEdit_recordBookId->text();
    QString bookName = ui->lineEdit_recordBookName->text();
    QString readerId = QString::number(dbTool->userID);
    QString readerName = currRecoed.value("reader_name").toString();
    QString applyDay = ui->lineEdit_recordOldDay->text();
    QString applyDate = currRecoed.value("borrow_date").toString();

    QString applyType = type;
    QString bookStatus;

    if("还书" == type){
        QString returnDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
       //添加至apply表
        bookStatus = "还书处理中";
        query.prepare("INSERT INTO apply(reader_id,reader_name,book_id,book_name,"
                      "apply_date,apply_day,return_date,apply_type,book_status,reader_identify)   "
                      "VALUES (?,?,?,?,?,?,?,?,?,?);");
        query.bindValue(0,readerId.toInt());
        query.bindValue(1,readerName);
        query.bindValue(2,bookId.toInt());
        query.bindValue(3,bookName);
        query.bindValue(4,applyDate);
        query.bindValue(5,applyDay);
        query.bindValue(6,returnDate);
        query.bindValue(7,applyType);
        query.bindValue(8,bookStatus);
        query.bindValue(9,dbTool->userIdentify_str);
        if(query.exec()){
            opState = true;
            QMessageBox::information(this, type+"提示", QString("%1成功！\n图书状态：%2").arg(type,bookStatus),QMessageBox::Ok);

            //更新读者recods表
            sql = QString("UPDATE records SET is_return='是',return_date='%1',book_status='已归还'  "
                          "WHERE reader_id=%2 AND book_id=%3;").arg(returnDate,readerId,bookId);
            if(!query.exec(sql))
                qDebug()<<"还书操作：更新recods表失败！";
        }
    } else if("续借" == type){
        QString renewDay = ui->spinBox_recordAddNewDay->text().remove(QRegExp("\\s"));//去空格
        bookStatus = "续借审核中";
        query.prepare("INSERT INTO apply(reader_id,reader_name,book_id,"
                      "book_name,apply_date,apply_day,apply_type,book_status,renew_day,reader_identify)  "
                      "VALUES (?,?,?,?,?,?,?,?,?,?);");
        query.bindValue(0,readerId.toInt());
        query.bindValue(1,readerName);
        query.bindValue(2,bookId.toInt());
        query.bindValue(3,bookName);
        query.bindValue(4,applyDate);
        query.bindValue(5,applyDay);
        query.bindValue(6,applyType);
        query.bindValue(7,bookStatus);
        query.bindValue(8,renewDay);
        query.bindValue(9,dbTool->userIdentify_str);
        if(query.exec()){
            opState = true;
            QMessageBox::information(this, type+"提示", QString("%1成功！\n图书状态：%2").arg(type,bookStatus),QMessageBox::Ok);

        //更新读者recods表
        sql = QString("UPDATE records SET book_status='续借中' WHERE reader_id=%1 AND book_id=%2;").arg(readerId,bookId);
        if(!query.exec(sql))
             qDebug()<<"续借操作：更新recods表失败！";
        }
    } else if("遗失" == type){
        bookStatus = "遗失处理中";
        query.prepare("INSERT INTO apply(reader_id,reader_name,book_id,"
                      "book_name,apply_date,apply_day,apply_type,book_status,reader_identify) "
                      "VALUES (?,?,?,?,?,?,?,?,?);");
        query.bindValue(0,readerId.toInt());
        query.bindValue(1,readerName);
        query.bindValue(2,bookId.toInt());
        query.bindValue(3,bookName);
        query.bindValue(4,applyDate);
        query.bindValue(5,applyDay);
        query.bindValue(6,applyType);
        query.bindValue(7,bookStatus);
        query.bindValue(8,dbTool->userIdentify_str);
        if(query.exec()){
            opState = true;
            QMessageBox::information(this, type+"提示", QString("%1成功！\n图书状态：%2").arg(type,bookStatus),QMessageBox::Ok);

            //更新读者recods表
            sql = QString("UPDATE records SET book_status='已遗失' WHERE reader_id=%1 AND book_id=%2;").arg(readerId,bookId);
            if(!query.exec(sql))
                qDebug()<<"遗失操作：更新recods表失败！";
        }
    }

    if(opState){
        ui->btn_recordReturnBook->setEnabled(false);
        ui->spinBox_recordAddNewDay->setEnabled(false);
        ui->btn_recordBookApply->setEnabled(false);
        ui->btn_recordBookLoss->setEnabled(false);
    }else{
        qDebug()<<type<<"操作失败！";
        return;
    }

    //刷新图书信息
    on_cBox_recordType_currentTextChanged("");
}

void Home::on_btn_recordRefuseLook_clicked() { ui->cBox_recordType->setCurrentText("被拒借阅"); }

void Home::on_spinBox_recordAddNewDay_valueChanged(int arg1)
{
    if(arg1 > 0)
       ui->btn_recordBookApply->setEnabled(true);
    else
       ui->btn_recordBookApply->setEnabled(false);
}

void Home::clearBookRecord()
{
    ui->lab_recordBookStateTip->clear();
    ui->lineEdit_recordBookId->clear();
    ui->lineEdit_recordBookName->clear();
    ui->lab_recordIsOverdue->clear();
    ui->lineEdit_recordOldDay->clear();
    ui->lineEdit_recordStoreDay->clear();
    ui->spinBox_recordAddNewDay->setValue(0);
    ui->btn_recordReturnBook->setEnabled(false);
    ui->btn_recordBookApply->setEnabled(false);
    ui->spinBox_recordAddNewDay->setEnabled(false);
    ui->btn_recordBookLoss->setEnabled(false);
}

/************************************************************************************************************************
 * @brief: 部分 图书管理
 ************************************************************************************************************************/
void Home::on_manageBookQueryChanged()
{
    //拿到过滤条件
    QString input = ui->lineEdit_manageSearchBook->text();
    QString bookType = ui->cBox_manageBookType->currentText();

    QString sql = dbSql->manageBookSQL(input,bookType);
    dbTool->managerQuerySQL(Table::Books,sql);

    //显示图书数量提示
    int rows = dbTool->managerBookQueryModel->rowCount();
    ui->lab_manageBookNumTip->setText(QString("%1条图书记录").arg(QString::number(rows)));
}

//防止新增图书时，修改原有图书数据
void Home::on_lineEdit_bookId_textChanged(const QString &arg1)
{
    QString sql;
    QSqlQuery query;
    sql = QString("SELECT * FROM books WHERE book_id=%1;").arg( arg1 );
    if(query.exec(sql) && query.next()){
        ui->lineEdit_bookId->setText( QString::number(arg1.toInt()) );

        QString bookStore = query.value("book_store").toString();
        ui->spinBox_bookStore->setValue( bookStore.left(bookStore.length() - 1).toInt() );
        QString bookTotal = query.value("book_total").toString();
        ui->spinBox_bookTotal->setValue( bookTotal.left(bookTotal.length() - 1).toInt() );

        ui->lineEdit_bookName->setText( query.value("book_name").toString() );
        ui->lineEdit_bookPub->setText( query.value("book_pub").toString() );
        ui->lineEdit_bookPubDate->setText( query.value("book_pub_date").toString() );

        ui->lineEdit_bookAuthor->setText( query.value("book_author").toString() );
        ui->cBox_managerbookIsBorrow->setCurrentText( query.value("book_borrowable").toString() );
        ui->cBox_managerbookType->setCurrentText(query.value("book_type").toString() );

        QString bookPrice = query.value("book_price").toString();
        if("未知" == bookPrice)
            ui->doubleSpinBox_bookPrice->setValue(0);
        else{
            ui->doubleSpinBox_bookPrice->setValue(  bookPrice.left(bookPrice.length()-1).toDouble() );
        }
    }
//    clearManagerBookInfo(); //清除数据
}


void Home::on_btn_manageBook_clicked()
{
    ui->btn_manageSave->setEnabled(false);
    on_manageBookQueryChanged();
}

void Home::on_btn_manageAdd_clicked()
{
    ui->btn_manageSave->setEnabled(true);//允许保存
    clearManagerBookInfo();
    ui->lineEdit_bookId->setFocus(); //设置默认焦点
}

void Home::on_btn_manageDelete_clicked()
{
    //1.存在为空的情况
    QString bookId = ui->lineEdit_bookId->text();
    QString bookName = ui->lineEdit_bookName->text();
    if(bookId.isEmpty() || bookName.isEmpty()){
        QMessageBox::information(this,"图书删除","图书编号和名称为空！",QMessageBox::Ok);
        return;
    }

    QString sql;
    QSqlQuery query;
    QString tip;

    //2.查询删除图书是否在被借阅中！！！！
    sql = QString("SELECT count(book_id)  "
                  "FROM records  "
                  "WHERE book_id=%1 AND (book_status='借阅中' OR book_status='续借中' OR book_status='超期中');").arg(bookId);
    if(query.exec(sql) && query.next()){
       int count = query.value(0).toInt();
       //有借阅情况
       if(count > 0){
            tip = QString("无法删除，图书已被%1人借阅\n图书编号：%2\n图书名称：%3").arg(QString::number(count),bookId,bookName);
            QMessageBox::information(this,"图书删除",tip,QMessageBox::Ok);
            return;
       }
    }

    //3.查询是否存在为审批记录
    sql = QString("SELECT COUNT(reader_id)  "
                  "FROM apply WHERE book_id=%1  "
                  "AND (apply_type='借书' OR apply_type='还书' OR apply_type='续借' OR apply_type='遗失');").arg(bookId);
    if(query.exec(sql) && query.next()){
       int count = query.value(0).toInt();
       //有未审批情况
       if(count > 0){
            tip = QString("无法删除，图书存在%1条未审批记录\n图书编号：%2\n图书名称：%3").arg(QString::number(count),bookId,bookName);
            QMessageBox::information(this,"图书删除",tip,QMessageBox::Ok);
            return;
       }
    }

    //4.都没有就开始确认是否真正的删除数据
    tip = QString("请再次确认图书信息：\n图书编号：%1\n图书名称：%2").arg(bookId,bookName);
    QMessageBox::StandardButton res = QMessageBox::information(this,"图书删除",tip,QMessageBox::Ok,QMessageBox::Cancel);
    if(res != QMessageBox::Close && res != QMessageBox::Cancel){
        sql = QString("DELETE FROM books WHERE book_id=%1;").arg(bookId);
        if(query.exec(sql)){
           tip = QString("图书删除成功！\n图书编号：%1\n图书名称：%2").arg(bookId,bookName);
           QMessageBox::information(this,"图书删除",tip,QMessageBox::Ok);
        }else
           qDebug()<<"图书删除操作失败...";
    }
    ui->btn_manageDelete->setEnabled(false);
    ui->btn_manageSave->setEnabled(false);
    clearManagerBookInfo();//清除显示数据
    on_manageBookQueryChanged();//刷新数据
}

void Home::on_btn_manageSave_clicked()
{
    //1.存在为空的情况
    QString bookId = ui->lineEdit_bookId->text();
    QString bookName = ui->lineEdit_bookName->text();
    QString bookPub = ui->lineEdit_bookPub->text();
    QString bookPubDate = ui->lineEdit_bookPubDate->text(); //
    QString bookAuthor = ui->lineEdit_bookAuthor->text();
    if(bookId.isEmpty() || bookName.isEmpty() || bookPub.isEmpty() || bookPubDate.isEmpty() || bookAuthor.isEmpty()){
       QMessageBox::information(this,"图书信息","图书信息填写不全！",QMessageBox::Ok);
       return;
    }
    //2.书籍名称的《》判断与添加
    // 《书籍》   《书籍    书籍》   书籍
    QString leftFlag = "《", rightFlag="》";
    if(leftFlag != bookName.at(0))
        bookName.insert(0, "《");
    if(rightFlag != bookName.at(bookName.length()-1))
        bookName.append("》");

    //3.拿完剩余数据
    QString bookStore = ui->spinBox_bookStore->text();
    QString bookTotal =  ui->spinBox_bookTotal->text(); //
    QString bookIsBorrow = ui->cBox_managerbookIsBorrow->currentText();
    QString bookType = ui->cBox_managerbookType->currentText();
    QString bookPrice = ui->doubleSpinBox_bookPrice->text();
    if("0.00元" == bookPrice)
       bookPrice = "未知";
    QString bookIntro = ui->plainText_manageBookIntro->toPlainText(); //拿到图书简介
    // 获取书籍图片
    // 将QPixmap转换为二进制数据
    QPixmap pixmap = ui->lab_bookImage->pixmap(Qt::ReturnByValue);
    QByteArray imageArray; //图片的二进制数据
    QBuffer buffer(&imageArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    QString sql;
    QSqlQuery query;

    //4.判断是否已存在该书籍，如果存在就是更新图书数据，否则是新增图书操作
    sql = QString("SELECT book_id FROM books WHERE book_id=%1;").arg(bookId);
    //更新图书信息操作
    if(query.exec(sql) && query.next()){
       query.prepare("UPDATE books SET book_id=?,book_name=?,book_author=?,book_pub=?,"
                     "book_pub_date=?,book_store=?,book_total=?,book_borrowable=?,book_type=?,book_price=?,book_image=?,book_intro=?  "
                     "WHERE book_id=?;");
       query.bindValue(0,bookId.toInt());
       query.bindValue(1,bookName);
       query.bindValue(2,bookAuthor);
       query.bindValue(3,bookPub);
       query.bindValue(4,bookPubDate);
       query.bindValue(5,bookStore);
       query.bindValue(6,bookTotal);
       query.bindValue(7,bookIsBorrow);
       query.bindValue(8,bookType);
       query.bindValue(9,bookPrice);
       query.bindValue(10,imageArray);
       query.bindValue(11,bookIntro);
       query.bindValue(12,bookId.toInt());
       if(query.exec()){
           QString tip = QString("图书信息更改成功！\n图书编号：%1\n图书名称：%2").arg(bookId,bookName);
           QMessageBox::information(this,"图书更新",tip,QMessageBox::Ok);
        }else
           qDebug()<<"图书信息更新失败...";
    }
    else{ //新增图书操作
       query.prepare("INSERT INTO books(book_id,book_name,book_author,book_pub,"
                      "book_pub_date,book_store,book_total,book_borrowable,book_type,book_price,book_image,book_intro)  "
                      "VALUES(?,?,?,?,?,?,?,?,?,?,?,?);");
       query.bindValue(0,bookId.toInt());
       query.bindValue(1,bookName);
       query.bindValue(2,bookAuthor);
       query.bindValue(3,bookPub);
       query.bindValue(4,bookPubDate);
       query.bindValue(5,bookStore);
       query.bindValue(6,bookTotal);
       query.bindValue(7,bookIsBorrow);
       query.bindValue(8,bookType);
       query.bindValue(9,bookPrice);
       query.bindValue(10,imageArray);
       query.bindValue(11,bookIntro);
        if(query.exec()){
           QString tip = QString("图书新增成功！\n图书编号：%1\n图书名称：%2").arg(bookId,bookName);
           QMessageBox::information(this,"图书新增",tip,QMessageBox::Ok);
        }else
           qDebug()<<"新增图书失败...";
    }
    ui->btn_manageSave->setEnabled(false);//不可重复保存
    on_manageBookQueryChanged();//刷新数据
}

void Home::on_btn_manageSelectBookImage_clicked()
{
    //单个图片
    QString filePath = QFileDialog::getOpenFileName(this, "选择书籍图片", QDir::currentPath(),  "图片(*.jpg *.png)");
    if(filePath.isEmpty())
        return;

    // 获取图片并显示到label上
    QPixmap pixmap;
    pixmap.load(filePath);
    ui->lab_bookImage->setPixmap(pixmap);    //单个图片
}

void Home::clearManagerBookInfo()
{
    ui->lineEdit_bookId->clear();
    ui->spinBox_bookStore->setValue(0);
    ui->spinBox_bookTotal->setValue(0); //
    ui->lineEdit_bookName->clear();
    ui->lineEdit_bookPub->clear();
    ui->lineEdit_bookPubDate->clear(); //
    ui->lineEdit_bookAuthor->clear();
    ui->cBox_managerbookIsBorrow->setCurrentText("可借");
    ui->cBox_managerbookType->setCurrentText("其他");
    ui->doubleSpinBox_bookPrice->setValue(0);
    ui->plainText_manageBookIntro->clear();
    QPixmap pixmap;
    pixmap.load(":img/bookImage.png");
    ui->lab_bookImage->setPixmap(pixmap); //设置默认图片
}

/************************************************************************************************************************
 * @brief: 部分 管理读者
 ************************************************************************************************************************/
void Home::on_btn_manageReader_clicked()
{
    ui->widget_manageReader->setVisible(false);
    ui->btn_readerUpdateInfo->setText("修改信息");
    isEditingReader = false;//重置编辑状态
    on_manageReaderQueryChanged();
}

void Home::on_manageReaderQueryChanged()
{
    //拿到过滤条件
    QString input = ui->lineEdit_searchReader->text();
    QString identify = ui->cBox_readerIdentify->currentText();
    QString sex = ui->cBox_readerSex->currentText();

    QString sql = dbSql->manageReaderSQL(input,identify,sex);
    dbTool->managerQuerySQL(Table::Users,sql);

    //显示读者数量提示
    int rows = dbTool->managerReaderQueryModel->rowCount();
    ui->lab_manageReaderNumTip->setText(QString("%1条读者记录").arg(QString::number(rows)));
}

//防止新增读者时，修改原有读者数据
void Home::on_lineEdit_readerId_textChanged(const QString &arg1)
{
    QString sql;
    QSqlQuery query;
    sql = QString("SELECT *  FROM users WHERE usr_id=%1;").arg( arg1 );
    if(query.exec(sql) && query.next()){
        //    ui->lab_readerPicture->clear();
        ui->lineEdit_readerId->setText(QString::number(query.value("usr_id").toInt()) );
        ui->lineEdit_readerName->setText(query.value("usr_name").toString());
        ui->cBox_managerReaderIdentify->setCurrentText(query.value("usr_identify").toString());

        QString sex = query.value("usr_sex").toString();
        if("未知" == sex)
            ui->radioButton_null->setChecked(true);
        else if("女" == sex)
            ui->radioButton_woman->setChecked(true);
        else if("男" == sex)
            ui->radioButton_man->setChecked(true);

        QString lossNum = query.value("usr_loss_num").toString();
        ui->spinBox_readerLossNum->setValue( lossNum.left(lossNum.length()-1).toInt() );

        QString overdueNum = query.value("usr_overdue_num").toString();
        ui->spinBox_readerOverdueNum->setValue( overdueNum.left(overdueNum.length()-1).toInt() );

        ui->lineEdit_readerPhone->setText(query.value("usr_phone").toString());
        ui->lineEdit_readerEmail->setText(query.value("usr_email").toString());
        ui->lineEdit_readerCollege->setText(query.value("usr_college").toString());
        ui->lineEdit_readerMajor->setText(query.value("usr_major").toString());
        ui->lineEdit_readerClass->setText(query.value("usr_class").toString());
        ui->lineEdit_readerRegisterDate->setText(query.value("usr_register_date").toString());
    }
}

void Home::on_btn_readerUpdateInfo_clicked()
{
    isEditingReader = !isEditingReader;
    ui->widget_manageReader->setVisible(isEditingReader);
    if(isEditingReader){
        ui->btn_readerSave->setEnabled(false);
        ui->btn_readerUpdateInfo->setText("退出修改");
        clearManagerReaderInfo();
    }else
        ui->btn_readerUpdateInfo->setText("修改信息");
}

void Home::on_btn_readerAdd_clicked()
{
    ui->btn_readerSave->setEnabled(true);
    clearManagerReaderInfo();
    ui->lineEdit_readerId->setFocus(); //设置默认焦点
}

void Home::on_btn_readerDelete_clicked()
{
    //1.存在为空的情况
    QString readerId = ui->lineEdit_readerId->text();
    QString readerName = ui->lineEdit_readerName->text();
    if(readerId.isEmpty() || readerName.isEmpty()){
       QMessageBox::information(this,"读者删除","读者编号和姓名为空！",QMessageBox::Ok);
       return;
    }

    QString sql;
    QSqlQuery query;
    QString tip;

    //2.查询被删除的读者是否在有正在借阅中的记录
    sql = QString("SELECT count(reader_id)  "
                  "FROM records  "
                  "WHERE reader_id=%1 AND (book_status='借阅中' OR book_status='续借中' OR book_status='超期中');").arg(readerId);
    if(query.exec(sql) && query.next()){
       int count = query.value(0).toInt();
       //读者有借阅情况
       if(count > 0){
            tip = QString("无法删除，读者有%1条正在借阅记录\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
            QMessageBox::information(this,"读者删除",tip,QMessageBox::Ok);
            return;
       }
    }

    //3.查询读者是否存在图书申请
    sql = QString("SELECT COUNT(reader_id)  "
                  "FROM apply WHERE reader_id=%1  "
                  "AND (apply_type='借书' OR apply_type='还书' OR apply_type='续借' OR apply_type='遗失');").arg(readerId);
    if(query.exec(sql) && query.next()){
       int count = query.value(0).toInt();
       //读者有图书申请，即存在未审批情况
       if(count > 0){
            tip = QString("无法删除，读者存在%1条未审批记录\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
            QMessageBox::information(this,"读者删除",tip,QMessageBox::Ok);
            return;
       }
    }

    //4.都没有就开始确认是否真正的删除数据
    tip = QString("请再次确认读者信息：\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
    QMessageBox::StandardButton res =  QMessageBox::information(this,"读者删除",tip,QMessageBox::Ok,QMessageBox::Cancel);
    if(res != QMessageBox::Close && res != QMessageBox::Cancel){
       sql = QString("DELETE FROM users WHERE usr_id=%1;").arg(readerId);
       if(query.exec(sql)){
           tip = QString("读者删除成功！\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
           QMessageBox::information(this,"读者删除",tip,QMessageBox::Ok);
       }else
           qDebug()<<"读者删除操作失败...";
    }
    ui->btn_readerDelete->setEnabled(false);//不可重复删除
    ui->btn_readerSave->setEnabled(false);//不可重复保存
    on_manageReaderQueryChanged(); //刷新数据
}

void Home::on_btn_readerSave_clicked()
{
    //1.存在为空的情况
    QString readerId = ui->lineEdit_readerId->text();
    QString readerName = ui->lineEdit_readerName->text();
    QString readerCollege = ui->lineEdit_readerCollege->text();
    if(readerId.isEmpty() || readerName.isEmpty() || readerCollege.isEmpty()){
        QMessageBox::information(this,"读者信息","读者信息填写不全！",QMessageBox::Ok);
        return;
    }

    //2.拿完剩余数据
    QString readerSex;
    if(ui->radioButton_man->isChecked())
        readerSex = "男";
    else if(ui->radioButton_woman->isChecked())
        readerSex = "女";
    else
        readerSex = "未知";
    QString readerIdentify = ui->cBox_managerReaderIdentify->currentText();
    QString readerLoss = ui->spinBox_readerLossNum->text();
    QString readerOverdue = ui->spinBox_readerOverdueNum->text();
    QString readerEmial = ui->lineEdit_readerEmail->text();
    QString readerPhone = ui->lineEdit_readerPhone->text();
    QString readerMajor = ui->lineEdit_readerMajor->text();
    QString readerClass = ui->lineEdit_readerClass->text();
    // 获取读者头像
    // 将QPixmap转换为二进制数据
    QPixmap pixmap = ui->lab_readerPicture->pixmap(Qt::ReturnByValue);
    QByteArray imageArray; //图片的二进制数据
    QBuffer buffer(&imageArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    QString sql;
    QSqlQuery query;

    //3.判断是否已存在该读者，如果存在就是更新操作，否则就是新增操作
    sql = QString("SELECT usr_id FROM users WHERE usr_id=%1;").arg(readerId);
    //更新读者息操作
    if(query.exec(sql) && query.next()){
        query.prepare("UPDATE users SET usr_id=?,usr_name=?,usr_sex=?,usr_identify=?,usr_overdue_num=?,"
                      "usr_loss_num=?,usr_phone=?,usr_email=?,usr_college=?,usr_major=?,usr_class=?,usr_image=?  "
                      "WHERE usr_id=?;");
        query.bindValue(0,readerId.toInt());
        query.bindValue(1,readerName);
        query.bindValue(2,readerSex);
        query.bindValue(3,readerIdentify);
        query.bindValue(4,readerOverdue);
        query.bindValue(5,readerLoss);
        query.bindValue(6,readerPhone);
        query.bindValue(7,readerEmial);
        query.bindValue(8,readerCollege);
        query.bindValue(9,readerMajor);
        query.bindValue(10,readerClass);
        query.bindValue(11,imageArray); //读者图片
        query.bindValue(12,readerId.toInt());
       if(query.exec()){
           QString tip = QString("读者信息更改成功！\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
           QMessageBox::information(this,"读者更新",tip,QMessageBox::Ok);
        }else
           qDebug()<<"读者信息更新失败...";
    }
    else{ //新增读者操作
        //判断是否在id_type表中，即判断是不是本校教师或者学生
        sql = QString("SELECT usr_id FROM id_type WHERE usr_id=%1 AND usr_state='否';").arg(readerId);
        //是本校教师或学生，并且未完善信息
        if(query.exec(sql) && query.next()){
            //默认密码为读者编号，这里为拿到读者编号加密之后的密码
            QString md5 = dbTool->encryptData(readerId);
            query.prepare("INSERT INTO users(usr_id,usr_password,usr_name,usr_sex,usr_identify,"
                          "usr_overdue_num,usr_loss_num,usr_phone,usr_email,usr_college,usr_major,usr_class,usr_image)  "
                          "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?);");
            query.bindValue(0,readerId.toInt());
            query.bindValue(1,md5);
            query.bindValue(2,readerName);
            query.bindValue(3,readerSex);
            query.bindValue(4,readerIdentify);
            query.bindValue(5,readerOverdue);
            query.bindValue(6,readerLoss);
            query.bindValue(7,readerPhone);
            query.bindValue(8,readerEmial);
            query.bindValue(9,readerCollege);
            query.bindValue(10,readerMajor);
            query.bindValue(11,readerClass);
            query.bindValue(12,imageArray);  //读者图片
            if(query.exec()){
               QString tip = QString("读者新增成功！\n读者编号：%1\n读者姓名：%2").arg(readerId,readerName);
               QMessageBox::information(this,"读者新增",tip,QMessageBox::Ok);
            }else
               qDebug()<<"新增读者失败...";

            //更新id表中的用户状态
            sql = QString("UPDATE id_type SET usr_state='是' WHERE usr_id=%1;").arg(readerId);
            if(!query.exec(sql))
               qDebug()<<"id表中的数据更新失败...";
        }else{
            QString tip = QString("读者新增失败！\n原因：[%1]不是本校师生编号").arg(readerId);
            QMessageBox::information(this,"读者新增",tip,QMessageBox::Ok);
        }
    }
    ui->btn_readerSave->setEnabled(false);//不可重复保存
    on_manageReaderQueryChanged(); //刷新数据
}

//更新读者图片
void Home::on_btn_manageSelectReaderImage_clicked()
{
    //单个图片
    QString filePath = QFileDialog::getOpenFileName(this, "选择读者图片", QDir::currentPath(),  "图片(*.jpg *.png)");
    if(filePath.isEmpty())
        return;

    // 获取图片并显示到label上
    QPixmap pixmap;
    pixmap.load(filePath);
    ui->lab_readerPicture->setPixmap(pixmap);
}

void Home::clearManagerReaderInfo()
{
    ui->lab_readerPicture->clear();
    ui->lineEdit_readerId->clear();
    ui->lineEdit_readerName->clear();
    ui->cBox_managerReaderIdentify->setCurrentText("学生");
    if(ui->radioButton_man->isChecked())
        ui->radioButton_man->setChecked(false);
    if(ui->radioButton_woman->isChecked())
        ui->radioButton_woman->setChecked(false);
    ui->radioButton_null->setChecked(true);//默认未知
    ui->spinBox_readerLossNum->setValue(0);
    ui->spinBox_readerOverdueNum->setValue(0);
    ui->lineEdit_readerPhone->clear();
    ui->lineEdit_readerEmail->clear();
    ui->lineEdit_readerCollege->clear();
    ui->lineEdit_readerMajor->clear();
    ui->lineEdit_readerClass->clear();
    ui->lineEdit_readerRegisterDate->clear();
    QPixmap pixmap;
    pixmap.load(":img/userImage.png");
    ui->lab_readerPicture->setPixmap(pixmap); //设置默认图片
}

/************************************************************************************************************************
 * @brief: 部分  员工管理
 ************************************************************************************************************************/
void Home::on_btn_manageStaff_clicked()
{
    on_lineEdit_searchStaff_textChanged( ui->lineEdit_searchStaff->text() );
    ui->btn_staffSave->setEnabled(false);
}

//实时搜索显示员工数据
void Home::on_lineEdit_searchStaff_textChanged(const QString &arg1)
{
    QString sql = dbSql->manageStaffSQL(arg1);
    dbTool->managerQuerySQL(Table::Managers,sql);

    //显示员工数量提示
    int rows = dbTool->managerStaffQueryModel->rowCount();
    ui->lab_manageStaffnumTip->setText(QString("%1条员工记录").arg(QString::number(rows)));
}

//防止新增员工时，修改原有员工数据
void Home::on_lineEdit_staffId_textChanged(const QString &arg1)
{
    QString sql;
    QSqlQuery query;
    sql = QString("SELECT * FROM managers WHERE man_id=%1 AND man_identify='图书管理员';").arg( arg1 );
    if(query.exec(sql) && query.next()){
        ui->lineEdit_staffId->setText( QString::number(query.value("man_id").toInt()) );
        ui->lineEdit_staffName->setText( query.value("man_name").toString() );

        QString staffSex = query.value("man_sex").toString();
        if("男" == staffSex)
            ui->cBox_staffSex->setCurrentText("男");
        else if("女" == staffSex)
            ui->cBox_staffSex->setCurrentText("女");
        else
            ui->cBox_staffSex->setCurrentText("未知");
        ui->doubleSpinBox_salary->setValue( query.value("man_salary").toDouble() );
        ui->lineEdit_staffPhone->setText( query.value("man_phone").toString() );
        ui->dateEdit_staffEntryDate->setDateTime( query.value("man_entry_date").toDateTime() );
    }
}

void Home::on_btn_staffAdd_clicked()
{
    ui->btn_staffSave->setEnabled(true);
    clearManagerStaffInfo();
    ui->lineEdit_staffId->setFocus(); //设置默认焦点
}

void Home::on_btn_staffDelete_clicked()
{
    //1.存在为空的情况
    QString staffId = ui->lineEdit_staffId->text();
    QString staffName =  ui->lineEdit_staffName->text();
    if(staffId.isEmpty() || staffName.isEmpty()){
        QMessageBox::information(this,"员工信息","员工信息填写不全！",QMessageBox::Ok);
        return;
    }

    //2.确认是否真正的删除数据
    QString tip = QString("请再次确认员工信息：\n员工编号：%1\n员工姓名：%2").arg(staffId,staffName);
    QMessageBox::StandardButton res =  QMessageBox::information(this,"员工删除",tip,QMessageBox::Ok,QMessageBox::Cancel);
    if(res == QMessageBox::Close || res == QMessageBox::Cancel)
        return;

    //3.开始删除数据
    QString sql = QString("DELETE FROM managers WHERE man_id=%1 AND man_identify='图书管理员';").arg(staffId);
    QSqlQuery query;
    if(query.exec(sql)){
        QString tip = QString("员工删除成功！\n员工编号：%1\n员工姓名：%2").arg(staffId,staffName);
        QMessageBox::information(this,"员工删除",tip,QMessageBox::Ok);
    }else
        qDebug()<<"员工删除失败...";

    ui->btn_staffDelete->setEnabled(false);//不可重复删除
    ui->btn_staffSave->setEnabled(false);//不可重复保存
    on_lineEdit_searchStaff_textChanged( ui->lineEdit_searchStaff->text() ); //刷新数据
}


void Home::on_btn_staffSave_clicked()
{
    //1.存在为空的情况
    QString staffId = ui->lineEdit_staffId->text();
    QString staffName =  ui->lineEdit_staffName->text();
    if(staffId.isEmpty() || staffName.isEmpty()){
        QMessageBox::information(this,"员工信息","员工信息填写不全！",QMessageBox::Ok);
        return;
    }

    //2.拿到剩余数据
    QString staffSex;
    if("男" == ui->cBox_staffSex->currentText())
        staffSex = "男";
    else if("女" == ui->cBox_staffSex->currentText())
        staffSex = "女";
    else
        staffSex = "未知";
    double staffSalary = ui->doubleSpinBox_salary->value();
    QString staffPhone = ui->lineEdit_staffPhone->text();
    QString staffEntry = ui->dateEdit_staffEntryDate->dateTime().toString("yyyy-MM-dd");
    // 获取员工图片
    // 将QPixmap转换为二进制数据
    QPixmap pixmap = ui->lab_staffPicture->pixmap(Qt::ReturnByValue);
    QByteArray imageArray; //图片的二进制数据
    QBuffer buffer(&imageArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    QString sql;
    QSqlQuery query;

    //3.判断是否已存在该员工，如果存在就是更新操作，否则就是新增操作
    sql = QString("SELECT man_id FROM managers WHERE man_id=%1;").arg(staffId);
    //更新读者息操作
    if(query.exec(sql) && query.next()){
        query.prepare("UPDATE managers SET man_id=?,man_name=?,man_sex=?,man_salary=?,man_phone=?,man_entry_date=?,man_image=?  "
                      "WHERE man_id=? AND man_identify='图书管理员';");
        query.bindValue(0,staffId.toInt());
        query.bindValue(1,staffName);
        query.bindValue(2,staffSex);
        query.bindValue(3,staffSalary);
        query.bindValue(4,staffPhone);
        query.bindValue(5,staffEntry);
        query.bindValue(6,imageArray);//员工图片
        query.bindValue(7,staffId.toInt());
        if(query.exec()){
           QString tip = QString("员工信息更改成功！\n员工编号：%1\n员工姓名：%2").arg(staffId,staffName);
           QMessageBox::information(this,"员工更新",tip,QMessageBox::Ok);
        }else
           qDebug()<<"员工信息更新失败...";
    }else{
       //新增员工操作
        //默认密码为读者编号，这里为拿到读者编号加密之后的密码
        QString md5 = dbTool->encryptData(staffId);
        query.prepare("INSERT INTO  "
                      "managers(man_id,man_password,man_name,man_sex,man_identify,man_salary,man_phone,man_entry_date,man_image)  "
                      "VALUES(?,?,?,?,?,?,?,?,?);");
        query.bindValue(0,staffId.toInt());
        query.bindValue(1,md5);
        query.bindValue(2,staffName);
        query.bindValue(3,staffSex);
        query.bindValue(4,"图书管理员");
        query.bindValue(5,staffSalary);
        query.bindValue(6,staffPhone);
        query.bindValue(7,staffEntry);
        query.bindValue(8,imageArray);//员工图片
        if(query.exec()){
           QString tip = QString("员工新增成功！\n员工编号：%1\n员工姓名：%2").arg(staffId,staffName);
           QMessageBox::information(this,"员工新增",tip,QMessageBox::Ok);
        }else
           qDebug()<<"员工新增失败...";
    }

    on_lineEdit_searchStaff_textChanged( ui->lineEdit_searchStaff->text() );
    ui->btn_staffSave->setEnabled(false);//不可重复保存
}

void Home::on_btn_manageSelectStaffImage_clicked()
{
    //单个图片
    QString filePath = QFileDialog::getOpenFileName(this, "选择员工图片", QDir::currentPath(),  "图片(*.jpg *.png)");
    if(filePath.isEmpty())
        return;

    // 获取图片并显示到label上
    QPixmap pixmap;
    pixmap.load(filePath);
    ui->lab_staffPicture->setPixmap(pixmap);
}

void Home::clearManagerStaffInfo()
{
    ui->lineEdit_staffId->clear();
    ui->lineEdit_staffName->clear();
    ui->cBox_staffSex->setCurrentText("未知");
    ui->doubleSpinBox_salary->setValue(0);
    ui->lineEdit_staffPhone->clear();
    ui->dateEdit_staffEntryDate->setDateTime( QDateTime::currentDateTime() );
    QPixmap pixmap;
    pixmap.load(":img/userImage.png");
    ui->lab_staffPicture->setPixmap(pixmap); //设置默认图片
}

/************************************************************************************************************************
 * @brief: 部分9 个人信息
 ************************************************************************************************************************/
void Home::on_btn_userInfo_clicked()
{
    clearSelfInfos(3); //清除个人信息

    QString sql;
    QSqlQuery query;

    switch (dbTool->userIdentify) {
    case IdentifyType::Teacher:
    case IdentifyType::Student:{
       sql = QString("SELECT * FROM users WHERE usr_id=%1;").arg(QString::number(dbTool->userID));
       if(query.exec(sql) && query.next()){
            ui->lineEdit_name->setText(query.value("usr_name").toString());
            QString sex = query.value("usr_sex").toString();
            if(sex.isEmpty())
                ui->rBtn_sexNull->setChecked(true);
            else if("男" == sex)
                ui->rBtn_man->setChecked(true);
            else if("女" == sex)
                ui->rBtn_woman->setChecked(true);
            ui->lab_idTitle->setText("账 户");
            ui->lineEdit_id->setText( QString::number(query.value("usr_id").toInt()) );
            ui->lineEdit_college->setText(query.value("usr_college").toString());
            ui->lineEdit_major->setText(query.value("usr_major").toString());
            ui->lineEdit_class->setText(query.value("usr_class").toString());
            ui->lineEdit_email->setText(query.value("usr_email").toString());
            ui->lineEdit_phone->setText(query.value("usr_phone").toString());

            ui->lineEdit_overdue->setText("总计" + query.value("usr_overdue_num").toString());
            ui->lineEdit_loss->setText("总计" + query.value("usr_loss_num").toString());

            // 加载图片，如果没有则加载默认图片
            QByteArray imageArray = query.value("usr_image").toByteArray();
            if(imageArray.isEmpty()){
                ui->lab_selfPicture->setPixmap(QPixmap(":img/userImage.png"));
            }else{
                QPixmap pixmap;
                pixmap.loadFromData(imageArray); //加载二进制数据
                ui->lab_selfPicture->setPixmap(pixmap);
            }
       }
       break;
    }
    case IdentifyType::Bookmanager:{
       sql = QString("SELECT * FROM managers WHERE man_identify='图书管理员' AND man_id=%1;").arg(QString::number(dbTool->userID));
       if(query.exec(sql) && query.next()){
            ui->lineEdit_name->setText(query.value("man_name").toString());
            QString sex = query.value("man_sex").toString();
            if(sex.isEmpty())
                ui->rBtn_sexNull->setChecked(true);
            else if("男" == sex)
                ui->rBtn_man->setChecked(true);
            else if("女" == sex)
                ui->rBtn_woman->setChecked(true);
            ui->lab_idTitle->setText("工 号");
            ui->lineEdit_id->setText( QString::number(query.value("man_id").toInt()) );
            ui->lineEdit_salary->setText( QString::number(query.value("man_salary").toDouble()) );
            ui->lineEdit_phone->setText(query.value("man_phone").toString());
            QString dateTip = QString("于%1入职").arg(query.value("man_entry_date").toDateTime().toString("yyyy年MM月dd日"));
            ui->lineEdit_entryDate->setText(dateTip);

            // 加载图片，如果没有则加载默认图片
            QByteArray imageArray = query.value("man_image").toByteArray();
            if(imageArray.isEmpty()){
                ui->lab_selfPicture->setPixmap(QPixmap(":img/userImage.png"));
            }else{
                QPixmap pixmap;
                pixmap.loadFromData(imageArray); //加载二进制数据
                ui->lab_selfPicture->setPixmap(pixmap);
            }
       }
       break;
    }
    case IdentifyType::Manager:{
       sql = QString("SELECT *  FROM managers WHERE man_identify='管理员' AND man_id=%1;").arg(QString::number(dbTool->userID));
       if(query.exec(sql) && query.next()){
            ui->lineEdit_name->setText(query.value("man_name").toString());
            QString sex = query.value("man_sex").toString();
            if(sex.isEmpty())
                ui->rBtn_sexNull->setChecked(true);
            else if("男" == sex)
                ui->rBtn_man->setChecked(true);
            else if("女" == sex)
                ui->rBtn_woman->setChecked(true);
            ui->lineEdit_id->setText( QString::number(query.value("man_id").toInt()) );
            ui->lineEdit_phone->setText(query.value("man_phone").toString());
//            QString dateTip = "注册时间：" + query.value("man_entry_date").toDateTime().toString("yyyy年MM月dd日");
//            ui->lineEdit_entryDate->setText(dateTip);

            // 加载图片，如果没有则加载默认图片
            QByteArray imageArray = query.value("man_image").toByteArray();
            if(imageArray.isEmpty()){
                ui->lab_selfPicture->setPixmap(QPixmap(":img/userImage.png"));
            }else{
                QPixmap pixmap;
                pixmap.loadFromData(imageArray); //加载二进制数据
                ui->lab_selfPicture->setPixmap(pixmap);
            }
       }
       break;
    }
    default:
       break;
    }
}

void Home::on_btn_editSelfInfo_clicked()
{
    if(!isEditingInfo){
        isEditingInfo = true;
        ui->btn_editSelfInfo->setText("确定修改");
        // 启用
        ui->rBtn_man->setEnabled(isEditingInfo);
        ui->rBtn_woman->setEnabled(isEditingInfo);
        ui->rBtn_sexNull->setEnabled(isEditingInfo);
        ui->lineEdit_name->setReadOnly(false);
        ui->lineEdit_college->setReadOnly(false);
        ui->lineEdit_major->setReadOnly(false);
        ui->lineEdit_class->setReadOnly(false);
        ui->lineEdit_email->setReadOnly(false);
        ui->lineEdit_phone->setReadOnly(false);

        //样式
        ui->lineEdit_name->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_major->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_college->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_class->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_email->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_phone->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_name->setFocus(); //设置默认焦点
        return;
    }

    //保存用户修改的信息
    QString sql;
    QSqlQuery query;

    //公共数据
    QString userId = QString::number(dbTool->userID);
    QString name = ui->lineEdit_name->text();
    QString sex;
    if(ui->rBtn_man->isChecked())
       sex = "男";
    else if(ui->rBtn_woman->isChecked())
       sex = "女";
    else if(ui->rBtn_sexNull->isChecked())
       sex = "未知";
    QString phone = ui->lineEdit_phone->text();

    // 根据不同用户类型准备sql语句
    if(IdentifyType::Teacher == dbTool->userIdentify || IdentifyType::Student == dbTool->userIdentify){
       QString college = ui->lineEdit_college->text();
       QString major = ui->lineEdit_major->text();
       QString _class = ui->lineEdit_class->text();
       QString email = ui->lineEdit_email->text();

       if(name.isEmpty() || college.isEmpty()){
            ui->lab_editSelfInfoTip->setText("姓名或学院不能为空，请补充填写！");
            return;
       }
       query.prepare("UPDATE users "
                     "SET usr_name=?,usr_sex=?,usr_college=?,usr_major=?,usr_class=?,usr_email=?,usr_phone=? "
                     "WHERE usr_id=?;");
       query.bindValue(0,name);
       query.bindValue(1,sex);
       query.bindValue(2,college);
       query.bindValue(3,major);
       query.bindValue(4,_class);
       query.bindValue(5,email);
       query.bindValue(6,phone);
       query.bindValue(7,userId.toInt());
    }else {
       if(name.isEmpty()){
            ui->lab_editSelfInfoTip->setText("姓名不能为空，请补充填写！");
            return;
       }
       query.prepare("UPDATE managers SET man_name=?,man_sex=?,man_phone=?  WHERE man_id=?;");
       query.bindValue(0,name);
       query.bindValue(1,sex);
       query.bindValue(2,phone);
       query.bindValue(3,userId.toInt());
    }
    if(query.exec()){
       QString currDateTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss");
       ui->lab_editSelfInfoTip->setText(QString("最近修改时间：%1").arg(currDateTime));
    }
    //成功修改个人信息，清空数据
    clearSelfInfos(1);
}

void Home::on_btn_editSelfInfoImage_clicked()
{
    // 1.选择单个图片
    QString filePath = QFileDialog::getOpenFileName(this, "选择图片", QDir::currentPath(),  "图片(*.jpg *.png)");
    if(filePath.isEmpty())
        return;

    // 2.获取图片并显示到label上
    QPixmap pixmap;
    pixmap.load(filePath);
    ui->lab_selfPicture->setPixmap(pixmap);

    // 3.把照片存储值数据库中
    // 将QPixmap转换为二进制数据
    QByteArray imageArray; //图片的二进制数据
    QBuffer buffer(&imageArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    // 根据不同用户类型准备sql语句
    int userId = ui->lineEdit_id->text().toInt();
    QSqlQuery query;
    if(IdentifyType::Teacher == dbTool->userIdentify || IdentifyType::Student == dbTool->userIdentify){
        query.prepare("UPDATE users SET usr_image=? WHERE usr_id=?;");
        query.bindValue(0,imageArray);
        query.bindValue(1,userId);
    }else{
        query.prepare("UPDATE managers SET man_image=?  WHERE man_id=?;");
        query.bindValue(0,imageArray);
        query.bindValue(1,userId);
    }
    if(query.exec()){
        QString currDateTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss");
        ui->lab_editSelfInfoTip->setText(QString("最近修改时间：%1").arg(currDateTime));
     }
}

void Home::on_btn_editNewPwd_clicked()
{
    if(!isEditingPwd){
        isEditingPwd = true; //编辑中...
        ui->btn_editNewPwd->setText("确定修改");
        ui->btn_newPwdVisible->setVisible(isEditingPwd);
        ui->btn_suerNewPwdVisible->setVisible(isEditingPwd);
        ui->lineEdit_newPwd->setReadOnly(false);
        ui->lineEdit_sureNewPwd->setReadOnly(false);
        ui->lineEdit_newPwd->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_sureNewPwd->setStyleSheet(LINE_EDIT_START);
        ui->lineEdit_newPwd->setFocus(); //设置默认焦点
        return;
    }

    //--保存密码时，需要充足的判断
    QString newPwd = ui->lineEdit_newPwd->text();
    QString surePwd = ui->lineEdit_sureNewPwd->text();

    //1.判断是否为空
    if(newPwd.isEmpty() || surePwd.isEmpty()){
       ui->lab_updatePwdTip->setText("密码不能为空！");
       return;
    }

    //2.判断用户新密码和确认是否相等
    if(newPwd != surePwd){
       ui->lab_updatePwdTip->setText("新密码不一致！");
       return;
    }

    QString sql;
    QSqlQuery query;
    QString userId = ui->lineEdit_id->text();

    //3.判断用户新密码和数据库中的原密码是否相同
    QString md5 = dbTool->encryptData(newPwd);//新密码MD5加密之后的!!!
    if(IdentifyType::Teacher == dbTool->userIdentify || IdentifyType::Student == dbTool->userIdentify){
        sql = QString("SELECT usr_password FROM users WHERE usr_id=%1;").arg(userId);
       if(query.exec(sql) && query.next()){
            if(query.value("usr_password").toString() == md5){
                ui->lab_updatePwdTip->setText("新旧密码相同！");
                return;
            }
       }
    }else{
       sql = QString("SELECT man_password FROM managers WHERE man_id=%1;").arg(userId);
       if(query.exec(sql) && query.next()){
            if(query.value("man_password").toString() == md5){
                ui->lab_updatePwdTip->setText("新旧密码相同！");
                return;
            }
       }
    }

    //4.真正的开始修改数据库中的用户密码
    if(IdentifyType::Teacher == dbTool->userIdentify || IdentifyType::Student == dbTool->userIdentify)
       sql =  QString("UPDATE users SET usr_password='%1' WHERE usr_id=%2;").arg(md5,userId);
    else
       sql =  QString("UPDATE managers SET man_password='%1' WHERE man_id=%2;").arg(md5,userId);
    if(query.exec(sql)){
       QString currDateTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh时mm分ss秒");
       ui->lab_editSelfInfoTip->setText(QString("最近修改时间：%1").arg(currDateTime));
//       QMessageBox::information(this,"修改信息","密码修改成功！",QMessageBox::Ok);
    }

    //成功修改密码，清空数据
    clearSelfInfos(2);
}

void Home::clearSelfInfos(const int& flag)
{
    //1-清空个人信息 2-清除密码部分 3-全部清除
    if(1 == flag || 3 == flag){
        isEditingInfo = false;
        ui->btn_editSelfInfo->setText("修改信息");
        // 启用
        ui->rBtn_man->setEnabled(isEditingInfo);
        ui->rBtn_woman->setEnabled(isEditingInfo);
        ui->rBtn_sexNull->setEnabled(isEditingInfo);
        ui->lineEdit_name->setReadOnly(true);
        ui->lineEdit_college->setReadOnly(true);
        ui->lineEdit_major->setReadOnly(true);
        ui->lineEdit_class->setReadOnly(true);
        ui->lineEdit_email->setReadOnly(true);
        ui->lineEdit_phone->setReadOnly(true);

        //样式
        ui->lineEdit_name->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_major->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_college->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_class->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_email->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_phone->setStyleSheet(LINE_EDIT_FINISH);

        ui->btn_editSelfInfo->setText("修改信息");
    }

    //密码这边的
    if(2 == flag || 3 == flag){
        isEditingPwd = false;
        ui->lineEdit_newPwd->clear();
        ui->lineEdit_sureNewPwd->clear();
        ui->lab_updatePwdTip->clear();
        ui->btn_editNewPwd->setText("修改密码");
        ui->lineEdit_newPwd->setStyleSheet(LINE_EDIT_FINISH);
        ui->lineEdit_sureNewPwd->setStyleSheet(LINE_EDIT_FINISH);

        ui->btn_newPwdVisible->setVisible(false);
        ui->btn_suerNewPwdVisible->setVisible(false);
        ui->lineEdit_newPwd->setEchoMode(QLineEdit::Password);
        ui->lineEdit_sureNewPwd->setEchoMode(QLineEdit::Password);
        ui->lineEdit_newPwd->setReadOnly(true);
        ui->lineEdit_sureNewPwd->setReadOnly(true);
        ui->btn_newPwdVisible->setIcon(QIcon(":img/pwdClose.png"));
        ui->btn_suerNewPwdVisible->setIcon(QIcon(":img/pwdClose.png"));
    }
}

/************************************************************************************************************************
 * @brief: 设置输入框的正则表达式
 ************************************************************************************************************************/
void Home::setInputRegExp()
{
    //编号/手机号码：纯数字
    QRegExp numReg(NUM_REG);
    ui->lineEdit_bookId->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_readerId->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_staffId->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_readerPhone->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_staffPhone->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_phone->setValidator(new QRegExpValidator(numReg,this));

    //邮箱
    QRegExp emailReg(EMAIL_REG);
    ui->lineEdit_readerEmail->setValidator(new QRegExpValidator(emailReg,this));
    ui->lineEdit_email->setValidator(new QRegExpValidator(emailReg,this));

    //学院/姓名/专业：汉字
    QRegExp zhReg(ZH_REG);
    ui->lineEdit_readerCollege->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_readerMajor->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_readerName->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_staffName->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_name->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_college->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_major->setValidator(new QRegExpValidator(zhReg,this));

    //班级：英文+汉字+数字
    QRegExp classReg(ZH_EN_NUM_REG);
    ui->lineEdit_readerClass->setValidator(new QRegExpValidator(classReg,this));
    ui->lineEdit_class->setValidator(new QRegExpValidator(classReg,this));

    //密码：英文大小写+数字
    QRegExp pwdReg(PWD_REG);
    ui->lineEdit_newPwd->setValidator(new QRegExpValidator(pwdReg,this));
    ui->lineEdit_sureNewPwd->setValidator(new QRegExpValidator(pwdReg,this));
}


/************************************************************************************************************************
 * @brief: 重置所有显示操作，即恢复默认登陆状态
 ************************************************************************************************************************/
void Home::restoreUI()
{
    //图书浏览-公共部分
    ui->lineEdit_searchBook->clear();
    ui->cBox_bookType->setCurrentText("所有");
    ui->cBox_bookIsBorrowable->setCurrentText("可借");

    //个人信息更新时间记录
    ui->lab_editSelfInfoTip->clear();

    if(IdentifyType::Manager == dbTool->userIdentify || IdentifyType::Bookmanager == dbTool->userIdentify){
        //图书分析
        ui->lineEdit_analysisSearchBook->clear();
        ui->lab_analysisBookAuthor->clear();
        ui->lab_analysisBookEntryDate->clear();
        ui->lab_analysisBookId->clear();
        ui->lab_analysisBookIsBorrowble->clear();
        ui->lab_analysisBookName->clear();
        ui->lab_analysisBookPrice->clear();
        ui->lab_analysisBookType->clear();

        singleChart->removeAllSeries();//删除所有序列
        singleChart->removeAxis(singleChart->axisX());//删除x轴
        singleChart->removeAxis(singleChart->axisY());//删除y轴

        typeChart->removeAllSeries();

        //借阅记录
        ui->radioBtn_reviewAll->setChecked(true);
        ui->cBox_recordType_manager->setCurrentText("所有借阅");
        ui->cBox_recordReaderType_manager->setCurrentText("全部");

        //图书管理
        clearManagerBookInfo();
        ui->lineEdit_manageSearchBook->clear();
        ui->cBox_manageBookType->setCurrentText("所有");

        //读者管理
        ui->lineEdit_searchReader->clear();
        ui->cBox_readerIdentify->setCurrentText("所有");
        ui->cBox_readerSex->setCurrentText("所有");

        //员工管理
        if(IdentifyType::Manager == dbTool->userIdentify) {
            clearManagerStaffInfo();
            ui->lineEdit_searchStaff->clear();
        }
    }
    else{ //学生或教师
        ui->cBox_recordType->setCurrentText("当前借阅");
        ui->lab_recordBookStateTip->clear();
        ui->lab_recordRefuseLookTip->clear();
    }
}

