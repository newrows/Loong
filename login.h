#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

signals:
    void login_success();

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_btn_login_clicked();

    void on_btn_quit_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
