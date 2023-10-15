#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTimer>

#include "db/dbtool.h"
#include "ui/home.h"
#include "global.h"

#define IMAGE_1_START_AX 0
#define IMAGE_2_START_AX 360
#define IMAGE_3_START_AX 720
#define IMAGE_4_START_AX 1080
#define IMAGE_WAIT_TIME 30  //图片等待时间，10为1s

#define ID_LENGTH 7
#define PWD_LENGTH 7
#define SURE_TIP_COUNT 8

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void login();//用户成功登录

private slots:
    // 轮播图、时间显示以及完善信息确认提示
    void on_startCarousel();
    void on_suerInfoTip();

    //登陆界面
    void on_btn_login_clicked();
    void on_btn_visitor_clicked(); //游客模式登录
    void on_lineEdit_loginId_textChanged(const QString &arg1);

    //完善信息界面
    void on_btn_perfectInfo_clicked();
    void on_lineEdit_id_textChanged(const QString &arg1);
    void on_lineEdit_name_textChanged(const QString &arg1);
    void on_lineEdit_college_textChanged(const QString &arg1);
    void on_lineEdit_pwd_textChanged(const QString &arg1);
    void on_lineEdit_surePwd_textChanged(const QString &arg1);

    void on_btn_clearInfo_clicked();
    void on_btn_sureInfo_clicked();
    void on_btn_returnLogin_clicked();

    void on_btn_close_clicked();
    void on_btn_min_clicked();


private:
    void setCarouselImage();//设置轮播图
    void setInputRegExp(); //设置正则表达式
    void clearLoginInfo();

    Home *home; //系统主界面

    QTimer *timer;

    bool startTip;//确认信息提示，默认关闭

    //轮播图
    int currImageIndex;
    int waitTime;
    int ax_1, ax_2, ax_3, ax_4; //四张图片x坐标

    //窗口操作
    QPoint winOffset;
    bool allowDrag;

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
