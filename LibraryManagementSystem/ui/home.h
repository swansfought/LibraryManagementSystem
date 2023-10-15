
#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QMovie>
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QInputDialog>
#include <QtCharts>
#include <QWebEngineView>

#include "db/dbtool.h"
#include "db/dbsql.h"
#include "global.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Home; }
QT_END_NAMESPACE

class Home : public QWidget

{
    Q_OBJECT

public:
    Home(QWidget *parent = nullptr);
    ~Home();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void userExit();

public slots:
    void on_functionLimit();//不同用户登录限制一些功能

private slots:
    //各界面table view的选择模型
    void on_bookSelectModel(const QModelIndex &current, const QModelIndex &previous);//图书浏览
    void on_reviewSelectModel(const QModelIndex &current, const QModelIndex &previous);//图书审批
    void on_recordSelectModel(const QModelIndex &current, const QModelIndex &previous);//借阅记录
    void on_managerBookSelectModel(const QModelIndex &current, const QModelIndex &previous);//管理图书
    void on_managerReaderSelectModel(const QModelIndex &current, const QModelIndex &previous);//管理读者
    void on_managerStaffSelectModel(const QModelIndex &current, const QModelIndex &previous);//管理员工

    //拖动栏三个按钮
    void on_btn_close_clicked();
    void on_btn_max_clicked();
    void on_btn_min_clicked();
    void on_btn_help_clicked();
    void on_btn_returnLogin_clicked();

    //左边dock栏
    void on_btn_bookBrowse_clicked();//图书浏览
    void on_btn_bookReview_clicked();//审批借阅
    void on_btn_readerRecord_clicked();//借阅记录
    void on_btn_manageBook_clicked();//管理图书
    void on_btn_manageReader_clicked();//管理读者
    void on_btn_manageStaff_clicked();//管理员工
    void on_btn_userInfo_clicked();//个人中心

    //2 .图书浏览界面
    void on_bookBrowseQueryChanged();
    void on_spinBox_borrowDay_valueChanged(int arg1);
    void on_btn_borrowApply_clicked(); // 申请借书

    //3.图书分析
    void on_btn_bookAnalysis_clicked();
    void on_btn_goAnalysisBookType_clicked();
    void on_btn_exitAnalysisBookType_clicked();
    void on_lineEdit_analysisSearchBook_returnPressed();//搜索单本书，显示图书数据和柱状图

    //4.审批借阅界面
    void on_bookReviewQueryChanged(); //选择性审批
    void on_btn_reviewBookAgree_clicked();
    void on_btn_reviewBookRefuse_clicked();

    //5.借阅记录界面
    void on_manageRecordQueryChanged(); //管理员根据字段去查询读者借阅记录
    void on_cBox_recordType_currentTextChanged(const QString &arg1);
    void on_spinBox_recordAddNewDay_valueChanged(int arg1);
    void on_recordUserOP(const QString &type);//用户还书、续借、遗失
    void on_btn_recordRefuseLook_clicked();//查看被拒记录

    //图书管理
    void on_manageBookQueryChanged();
    void on_lineEdit_bookId_textChanged(const QString &arg1);//防止新增图书时，修改原有图书数据
    void on_btn_manageAdd_clicked();//增,清除填写的信息
    void on_btn_manageDelete_clicked();//删
    void on_btn_manageSave_clicked();//保存
    void on_btn_manageSelectBookImage_clicked();

    //管理读者
    void on_manageReaderQueryChanged();
    void on_lineEdit_readerId_textChanged(const QString &arg1);//防止新增读者时，修改原有读者数据
    void on_btn_readerUpdateInfo_clicked();
    void on_btn_readerAdd_clicked();
    void on_btn_readerDelete_clicked();
    void on_btn_readerSave_clicked();
    void on_btn_manageSelectReaderImage_clicked();

    //管理员工
    void on_lineEdit_searchStaff_textChanged(const QString &arg1);
    void on_lineEdit_staffId_textChanged(const QString &arg1); //防止新增员工时，修改原有员工数据
    void on_btn_staffAdd_clicked();
    void on_btn_staffDelete_clicked();
    void on_btn_staffSave_clicked();
    void on_btn_manageSelectStaffImage_clicked();

    //9.个人中心
    void on_btn_editSelfInfo_clicked();
    void on_btn_editSelfInfoImage_clicked(); //修改个人照片
    void on_btn_editNewPwd_clicked();

private:
    void clearBookBrowse();//2 .图书浏览界面
    void clearBookReview();//4.审批借阅
    void clearBookRecord();//5.借阅记录
    void clearSelfInfos(const int& flag); //清除个人中心信息，1-清空个人信息 2-清除密码部分 3-全部清除
    void clearSelfInfoPwd();//清除个人中心的密码信息
    void clearManagerBookInfo(); //清除管理图书信息
    void clearManagerReaderInfo(); //清除管理读者信息
    void clearManagerStaffInfo(); //清除管理员工信息
    void restoreUI(); //重置所有显示操作，即恢复默认登陆状态
    void setInputRegExp(); //设置正则表达式

    //图书分析
    void totalBookShow();
    void signleBookShow(const QString &input);
    void bookNumShow();
    void bookTypeShow();
    void flushTotalBookShow();

    //数据库
    DBTool *dbTool;
    DbSQL *dbSql;
    QStringList typeList;

    //个人信息-编辑信息操作
    bool isEditingInfo;//编辑信息状态，默认false
    bool isEditingPwd;//编辑密码，默认false

    bool isEditingReader; //管理读者，是修改信息和返回的单按钮双状态操作的判断标志

    //图书分析
    QChart *singleChart; //单本书籍显示图表，柱状图
    QChart *totalChart; //全书籍显示图表，柱状图
    QChart *typeChart; //书籍类型显示图表，饼状图

    //窗口拖动
    QPoint winOffset;
    bool allowDrag;

    QMovie *movie; //用于播放主页左上角gif图片

private:
    Ui::Home *ui;
};

#endif // HOME_H
