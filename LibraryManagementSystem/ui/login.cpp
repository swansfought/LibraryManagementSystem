#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    home(nullptr),
    startTip(false),
    allowDrag(false),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    //设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint );//| Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("LibrarySystem");

    //设置窗口阴影
    QGraphicsDropShadowEffect *shadow =  new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);               // 设置阴影距离
    shadow->setColor(QColor(55, 55, 55));  // 设置阴影颜色
    shadow->setBlurRadius(7);              // 设置阴影圆角
    ui->widget_background->setGraphicsEffect(shadow);

    //加载样式表
    QFile file(":/qss/login.qss");
    if(file.open(QFile::ReadOnly)){
        QTextStream stream(&file);
        QString styleSheet = file.readAll();
        this->setStyleSheet(styleSheet);
    }

    ui->stackedWidget->setCurrentIndex(0);//设置默认为登录页

     //事件过滤器
    ui->widget_drag->installEventFilter(this);
    ui->btn_pwdVisible->installEventFilter(this);
    ui->btn_registerPwdVisible->installEventFilter(this);
    ui->btn_suerPwdVisible->installEventFilter(this);

    //home界面
    home = new Home;
    connect(home,&Home::userExit,this,[=]{
        this->show();
        home->hide();
        timer->start();//开启轮播图
    });
    connect(this,&Login::login,home,&Home::on_functionLimit);

    //回车确认
    connect(ui->lineEdit_loginPwd,&QLineEdit::returnPressed,this,[=]{on_btn_login_clicked();});

    //轮播图，并且在里面设置显示时间
    setCarouselImage();
    timer = new QTimer(this);
    timer->setInterval(100); //100 ms
    connect(timer,&QTimer::timeout,this,&Login::on_startCarousel);//定时器时间到，就移动图片
    connect(timer,&QTimer::timeout,this,&Login::on_suerInfoTip);//确认信息提示
    timer->start();

    setInputRegExp(); //设置正则表达式

    ui->lineEdit_loginId->setFocus();//设置默认焦点
}

Login::~Login()
{
    if(nullptr != home)
        delete home;
    delete ui;
}

// @brief: 圆角实现
void Login::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//抗锯齿
    painter.setBrush(QColor(57, 61, 73));
    painter.setPen(Qt::transparent);//设置透明画笔
    painter.drawRoundedRect(this->rect(), 5, 5);//绘制圆角矩形
    QWidget::paintEvent(event);
}

bool Login::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Enter){
        if(dynamic_cast<QWidget*>(watched) == ui->widget_drag){
            allowDrag = true;
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_pwdVisible){
            ui->btn_pwdVisible->setIcon(QIcon(":/img/pwdShow.png"));
            ui->lineEdit_loginPwd->setEchoMode(QLineEdit::Normal);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_registerPwdVisible){
            ui->btn_registerPwdVisible->setIcon(QIcon(":/img/pwdShow.png"));
            ui->lineEdit_pwd->setEchoMode(QLineEdit::Normal);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_suerPwdVisible){
            ui->btn_suerPwdVisible->setIcon(QIcon(":/img/pwdShow.png"));
            ui->lineEdit_surePwd->setEchoMode(QLineEdit::Normal);
        }
    }

    if(event->type() == QEvent::Leave){
        if(dynamic_cast<QWidget*>(watched) == ui->widget_drag){
            allowDrag = false;
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_pwdVisible){
            ui->btn_pwdVisible->setIcon(QIcon(":/img/pwdClose.png"));
            ui->lineEdit_loginPwd->setEchoMode(QLineEdit::Password);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_registerPwdVisible){
            ui->btn_registerPwdVisible->setIcon(QIcon(":/img/pwdClose.png"));
            ui->lineEdit_pwd->setEchoMode(QLineEdit::Password);
        }
        else if(dynamic_cast<QPushButton*>(watched) == ui->btn_suerPwdVisible){
            ui->btn_suerPwdVisible->setIcon(QIcon(":/img/pwdClose.png"));
            ui->lineEdit_surePwd->setEchoMode(QLineEdit::Password);
        }
    }
    return QWidget::eventFilter(watched,event);
}

//鼠标按压事件
void Login::mousePressEvent(QMouseEvent *event)
{
    winOffset = event->globalPos() - this->pos();
    QWidget::mouseMoveEvent(event);
}

//鼠标按键释放事件
void Login::mouseReleaseEvent(QMouseEvent *event)
{
    winOffset = QPoint();
    QWidget::mouseMoveEvent(event);
}

//鼠标移动事件
void Login::mouseMoveEvent(QMouseEvent *event)
{
    //防止按钮拖动导致的奇怪问题
    if(QPoint() == winOffset || !allowDrag)
        return;
    move(event->globalPos() - winOffset);
    QWidget::mouseMoveEvent(event);
}

//清除信息
void Login::on_btn_clearInfo_clicked()
{
    ui->lineEdit_id->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_college->clear();
    ui->lineEdit_identify->clear();
    ui->lineEdit_pwd->clear();
    ui->lineEdit_surePwd->clear();
    ui->lab_prefectInfoTip->setText("带*为必填项，请认真填写个人信息！");
}

void Login::on_btn_sureInfo_clicked()
{
    if("√" != ui->lab_idTip->text()||"√" != ui->lab_nameTip->text()||
       "√" != ui->lab_collegeTip->text()||"√" != ui->lab_pwdTip->text() ||"√" != ui->lab_surePwdTip->text()) {
        startTip = true;
        return;
    }
    QString sql;
    QSqlQuery query;

    QString id = ui->lineEdit_id->text();
    QString name = ui->lineEdit_name->text();
    QString college = ui->lineEdit_college->text();
    QString identify = ui->lineEdit_identify->text();
    QString surePWd = ui->lineEdit_surePwd->text();

    //加密用户的密码
    auto dbTool = DBTool::getInstance();
    QString md5 = dbTool->encryptData(surePWd);

    sql = QString("insert into users(usr_id,usr_password,usr_name,usr_identify,usr_college) values"
                  "(%1,'%2','%3','%4','%5');").arg(id,md5,name,identify,college);

    if(!query.exec(sql))
        qDebug()<<"完善信息失败...";
    else{
        QMessageBox::information(this,"完善信息", name+identify+",信息完善成功！",QMessageBox::Ok);

        //更新id表中的用户状态
        sql = QString("UPDATE id_type SET usr_state='是' WHERE usr_id=%1;").arg(id);
        if(!query.exec(sql))
            qDebug()<<"id表中的数据更新失败...";
    }

    ui->lineEdit_pwd->clear();
    ui->lineEdit_surePwd->clear();
    on_lineEdit_id_textChanged(id);
}

//返回登录界面
void Login::on_btn_returnLogin_clicked() {
    ui->lineEdit_loginId->setFocus();//设置默认焦点
    ui->stackedWidget->setCurrentIndex(0);

    //把完善信息界面的ID显示到登录界面中
    QString prefectId = ui->lineEdit_id->text();
    if(!prefectId.isEmpty() && ui->lineEdit_loginId->text() != prefectId)
        ui->lineEdit_loginId->setText(prefectId);

    on_btn_clearInfo_clicked();//清除完善信息

    //默认不显示密码
    ui->lineEdit_surePwd->setEchoMode(QLineEdit::Password);
    ui->btn_suerPwdVisible->setIcon(QIcon(":img/pwdClose.png"));
}

//前往完善信息页面
void Login::on_btn_perfectInfo_clicked() {
    ui->lineEdit_id->setFocus();//设置默认焦点
    ui->stackedWidget->setCurrentIndex(1);

    ui->lineEdit_id->setText(ui->lineEdit_loginId->text());//把登录输入的id拿过来

    ui->lineEdit_loginPwd->clear();
    ui->lab_loginTip->clear();
}

//完善信息-账户输入
void Login::on_lineEdit_id_textChanged(const QString &arg1)
{
    //清除搜索实时搜索出来的数据，具体添加哪些数据下面的判断会添加新数据
    ui->lineEdit_identify->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_college->clear();
    ui->btn_sureInfo->setEnabled(true);

    if(arg1.isEmpty())
        ui->lab_idTip->setText("*");
    else if(arg1.length() != ID_LENGTH)
        ui->lab_idTip->setText("!");
    else{
        //去拿到该用户类型
        QSqlQuery query;
        QString sql = QString("SELECT usr_type,usr_state FROM id_type WHERE usr_id=%1;").arg(arg1);
        if(query.exec(sql) && query.next()){
            QString identify = query.value("usr_type").toString();
            QString state = query.value("usr_state").toString();
            //已经完善过信息
            if("是" == state){
                //拿到该用户姓名和所在学院
                sql = QString("SELECT usr_name,usr_college FROM users WHERE usr_id=%1;").arg(arg1);
                if(query.exec(sql) && query.next()){
                    ui->lineEdit_name->setText( query.value("usr_name").toString());
                    ui->lineEdit_college->setText( query.value("usr_college").toString());
                }
                ui->lineEdit_identify->setText(identify+"[已完善]");
                ui->btn_sureInfo->setEnabled(false);
            }else{
                ui->lineEdit_identify->setText(identify);
            }
            ui->lab_idTip->setText("√");
        }else {
            ui->lineEdit_identify->setText("不存在");
            ui->lab_idTip->setText("×");
        }
    }
}

//完善信息-姓名输入
void Login::on_lineEdit_name_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        ui->lab_nameTip->setText("*");
    else
        ui->lab_nameTip->setText("√");
}

void Login::on_lineEdit_college_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        ui->lab_collegeTip->setText("*");
    else
        ui->lab_collegeTip->setText("√");
}

//完善信息-密码输入
void Login::on_lineEdit_pwd_textChanged(const QString &arg1)
{
    //只要原始密码被修改，确认密码都应该清除
    ui->lineEdit_surePwd->clear();

    if(arg1.isEmpty())
        ui->lab_pwdTip->setText("*");
    else if(arg1.length() < PWD_LENGTH)
        ui->lab_pwdTip->setText("!");
    else
        ui->lab_pwdTip->setText("√");
}

//完善信息-确认密码
void Login::on_lineEdit_surePwd_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        ui->lab_surePwdTip->setText("*");
    else if(arg1.length() < PWD_LENGTH)
        ui->lab_surePwdTip->setText("!");
    else if(ui->lineEdit_pwd->text() != arg1)
        ui->lab_surePwdTip->setText("×");
    else
        ui->lab_surePwdTip->setText("√");
}


//登录
void Login::on_btn_login_clicked()
{
    if(ui->lineEdit_loginId->text().isEmpty() || ui->lineEdit_loginPwd->text().isEmpty()){
        ui->lab_loginTip->setText("账号或密码为空！");
        return;
    }
    int id = ui->lineEdit_loginId->text().toInt();
    QString pwd = ui->lineEdit_loginPwd->text();

    auto dbTool =  DBTool::getInstance();//获取数据库工具
    if(dbTool->loginVertify(id,pwd)){
        emit login();  //提示主界面登录成功，便于功能限制
        home->show();
        this->hide();
        clearLoginInfo(); //清除登录信息
    }else{
        //去用户类型表中查询该用户是否没有完善信息，因为完善信息的用户是会被存放至用户表中的
        QSqlQuery query;
        QString sql = QString("SELECT usr_type,usr_state FROM id_type WHERE usr_id=%1;").arg(QString::number(id));
        if(query.exec(sql) && query.next()){
            if(query.value("usr_state").toString() == "否")
                ui->lab_loginTip->setText("请先完善信息！");
            else
                ui->lab_loginTip->setText("账户或密码输入错误！");
        } else
            ui->lab_loginTip->setText("账户或密码输入错误！");
    }
}

void Login::on_btn_visitor_clicked()
{
    auto db = DBTool::getInstance();
    db->userIdentify = IdentifyType::Visitor;
    db->userIdentify_str = "游客";
    emit login();  //提示主界面登录成功，便于功能限制
    home->show();
    this->hide();
    clearLoginInfo(); //清除登录信息
}

//只要账号被修改，密码都应该清除
void Login::on_lineEdit_loginId_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->lineEdit_loginPwd->clear();
}

void Login::on_btn_close_clicked()
{
    QMessageBox::StandardButton res;
    res = QMessageBox::question(this,"确认窗口","请确认是否退出图书系统",QMessageBox::Yes|QMessageBox::No,QMessageBox::NoButton);
    if(QMessageBox::Yes == res)
        this->close();
}

void Login::on_btn_min_clicked() { this->showMinimized(); }

//设置轮播图图片
void Login::setCarouselImage()
{
    currImageIndex = 0; //默认指向第一张
    waitTime = 0; //已等待时间

    //设置图片的x坐标
    ax_1 = IMAGE_1_START_AX;
    ax_2 = IMAGE_2_START_AX;
    ax_3 = IMAGE_3_START_AX;
    ax_4 = IMAGE_4_START_AX;

    //设置图片起始位置
    ui->labImage_1->move(IMAGE_1_START_AX,0);
    ui->labImage_2->move(IMAGE_2_START_AX,0);
    ui->labImage_3->move(IMAGE_3_START_AX,0);
    ui->labImage_4->move(IMAGE_4_START_AX,0);

    //设置图片
    ui->labImage_1->setPixmap(QPixmap(":/img/fight.png"));
    ui->labImage_2->setPixmap(QPixmap(":/img/run.png"));
    ui->labImage_3->setPixmap(QPixmap(":/img/thinking.png"));
    ui->labImage_4->setPixmap(QPixmap(":/img/puzzle.png"));
}

//设置正则表达式
void Login::setInputRegExp()
{
    //编号/手机号码：纯数字
    QRegExp numReg(NUM_REG);
    ui->lineEdit_loginId->setValidator(new QRegExpValidator(numReg,this));
    ui->lineEdit_id->setValidator(new QRegExpValidator(numReg,this));

    //密码：英文大小写+数字
    QRegExp pwdReg(PWD_REG);
    ui->lineEdit_loginPwd->setValidator(new QRegExpValidator(pwdReg,this));
    ui->lineEdit_pwd->setValidator(new QRegExpValidator(pwdReg,this));
    ui->lineEdit_surePwd->setValidator(new QRegExpValidator(pwdReg,this));

   //学院/姓名：汉字
    QRegExp zhReg(ZH_REG);
    ui->lineEdit_name->setValidator(new QRegExpValidator(zhReg,this));
    ui->lineEdit_college->setValidator(new QRegExpValidator(zhReg,this));
}

void Login::clearLoginInfo()
{
    timer->stop(); //关闭轮播图

    //清除登录信息，但是保留用户ID
    ui->lineEdit_loginPwd->clear();
    ui->lab_loginTip->clear();
    ui->lineEdit_loginPwd->setEchoMode(QLineEdit::Password);
    ui->btn_pwdVisible->setIcon(QIcon(":img/pwdClose.png"));
}

// @brief: 轮播图  1 2 3 4, 1 2 3 4, 1 2 3 4
void Login::on_startCarousel()
{
    //设置显示时间
    QString currDateTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日  dddd  hh:mm:ss");
//    ui->lab_currDateTime->setText("时间管理：" + currDateTime);

    //已等待时间
    if(waitTime != IMAGE_WAIT_TIME){
        ++waitTime;
        return;
    }

    ax_1 -= 15;
    ax_2 -= 15;
    ax_3 -= 15;
    ax_4 -= 15;

    //设置图片新的位置
    ui->labImage_1->move(ax_1,0);
    ui->labImage_2->move(ax_2,0);
    ui->labImage_3->move(ax_3,0);
    ui->labImage_4->move(ax_4,0);

    if(0 == ax_1){
        currImageIndex = 0;
        waitTime = 0;

        //将第4张图片接至尾部
        ui->labImage_4->move(IMAGE_4_START_AX,0);

        //重置图片x轴坐标位置
        ax_1 = IMAGE_1_START_AX;
        ax_2 = IMAGE_2_START_AX;
        ax_3 = IMAGE_3_START_AX;
        ax_4 = IMAGE_4_START_AX;
    }
    else if(0 == ax_2){
        currImageIndex = 1;
        waitTime = 0;
    }
    else if(0 == ax_3){
        currImageIndex = 2;
        waitTime = 0;
    }
    else if(0 == ax_4){
        currImageIndex = 3;
        waitTime = 0;

        //将第1,2,3张图片按顺序接至尾部
        ui->labImage_1->move(IMAGE_2_START_AX,0);
        ui->labImage_2->move(IMAGE_3_START_AX,0);
        ui->labImage_3->move(IMAGE_4_START_AX,0);

        //重置图片x轴坐标位置
        ax_1 = IMAGE_2_START_AX;
        ax_2 = IMAGE_3_START_AX;
        ax_3 = IMAGE_4_START_AX;
        ax_4 = IMAGE_1_START_AX;
    }
}

void Login::on_suerInfoTip()
{
    //1.没有开启提示
    if(!startTip)
        return;

    //2.初始化提示信息，只会被初始化一次
    static int tipCount = SURE_TIP_COUNT; //提示次数
    static bool changeTip = false;//提示状态，true为红色，false为黑色

    //3.提示状态改变
    changeTip = !changeTip;
    tipCount = tipCount - 1;

    //4.结束提示判断，以次数0为借书标志
    if(tipCount < 0){
        //恢复初始值
        tipCount = SURE_TIP_COUNT;
        changeTip = false;

         // 结束提示
        startTip = false;
    }

    //5.开始闪烁提示
    if(changeTip){
        if("√" != ui->lab_idTip->text())
           ui->lineEdit_id->setStyleSheet(QString("QLineEdit{border: 1px solid red;}"));

        if("√" != ui->lab_nameTip->text())
           ui->lineEdit_name->setStyleSheet(QString("QLineEdit{border: 1px solid red;}"));

        if("√" != ui->lab_collegeTip->text())
           ui->lineEdit_college->setStyleSheet(QString("QLineEdit{border: 1px solid red;}"));

        if("√" != ui->lab_pwdTip->text())
           ui->lineEdit_pwd->setStyleSheet(QString("QLineEdit{border: 1px solid red;}"));

        if("√" != ui->lab_surePwdTip->text())
           ui->lineEdit_surePwd->setStyleSheet(QString("QLineEdit{border: 1px solid red;}"));
    }else{
        if("√" != ui->lab_idTip->text())
           ui->lineEdit_id->setStyleSheet(QString("QLineEdit{border: 1px solid black;}"));

        if("√" != ui->lab_nameTip->text())
           ui->lineEdit_name->setStyleSheet(QString("QLineEdit{border: 1px solid black;}"));

        if("√" != ui->lab_collegeTip->text())
           ui->lineEdit_college->setStyleSheet(QString("QLineEdit{border: 1px solid black;}"));

        if("√" != ui->lab_pwdTip->text())
           ui->lineEdit_pwd->setStyleSheet(QString("QLineEdit{border: 1px solid black;}"));

        if("√" != ui->lab_surePwdTip->text())
           ui->lineEdit_surePwd->setStyleSheet(QString("QLineEdit{border: 1px solid black;}"));
    }
}


