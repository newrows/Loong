#include "login.h"
#include "ui_login.h"
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_login_clicked()
{
//    if(nullptr == ui->le_username->text() || nullptr == ui->le_password->text())
//    {
//        QMessageBox::warning(nullptr, "登录失败！", "输入框不能为空！");
//        return;
//    }

//    if(ui->le_username->text() == "admin")
//    {
//        if("admin" != ui->le_password->text())
//        {
//            QMessageBox::warning(nullptr, "登录失败！", "请输入正确密码！");
//            return;
//        }
//        QMessageBox::warning(nullptr, "登录成功！", "登录成功！");
//        emit login_success();
//    }
    emit login_success();
}

void Login::on_btn_quit_clicked()
{
    exit(0);
}
