#ifndef WIDGET_H
#define WIDGET_H

#define Page_Main       0
#define Page_Water      1
#define Page_Soil       2
#define Page_Air        3
#define Page_Database   4
#define Page_Logs       5
#define Page_Us         6
#define ENABLE          true
#define DISABLE         false


#include <QWidget>
#include "login.h"
#include "ui_widget.h"

#include <qmqttclient.h>
#include "mqtt/qmqtt.h"
#include <QtNetwork>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include "rewritestyleditemdelegate.h"

#include <QDateTime>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void All_Init();
    void Mqtt_Init();
    void Mqtt_Publish();
    void Mqtt_Subscribe(QMQTT::Message message);
    void Infomation_Send();
    void Infomation_Tran();
    QString State_Config(bool state);
    void Info_Handling();
    float Sqr_Pow(int date);
    void Infomation_Show();
    void Button_Set(QPushButton *button, bool state);
    void Button_Clk(QPushButton *button, bool state);

    QString Json_String(QJsonObject object, QString keyword);
    int Json_Int(QJsonObject object, QString keyword, int m_info);
    bool Json_Bool(QJsonObject object, QString keyword, bool m_info);

    void Database_Init();
    void M_Index_Set(int index);
    void Database_Update();

    void Time_Init();
    void Timer_Init();
public: //SLOTS


private slots:
    void on_btn_mainpage_clicked();

    void on_btn_timedata_clicked();

    void on_btn_database_clicked();

    void on_btn_logs_clicked();

    void on_btn_us_clicked();

    void on_water_next_clicked();

    void on_soil_pre_clicked();

    void on_soil_next_clicked();

    void on_air_pre_clicked();

    void on_btn_config_clicked();

    void on_btn_filter_clicked();

    void on_btn_water1_clicked();

    void on_btn_water2_clicked();

    void on_btn_oxyen_clicked();

    void on_btn_water_on_clicked();

    void on_btn_blow_clicked();

    void on_light_on_clicked();

    void on_btn_update_clicked();

    void on_btn_clear_clicked();

    void on_btn_find_clicked();

private:
    Ui::Widget *ui;
    Login m_login;

/*------------------这个部分是定义的变量-----------------------------*/
    //水
    int water_level        = 0;
    int water_turbidity    = 0;    //浊度
    int water_tempreture   = 0;
    int water_ph           = 0;
    bool water_1           = DISABLE;
    bool water_2           = DISABLE;
    bool water_filter      = DISABLE;
    bool oxyen_on          = DISABLE;
    //陆
    int soil_temperature   = 100;
    int soil_moisture      = 0;    //湿度
    bool water_on          = DISABLE;
    //空
    int CO2                = 0;
    int smoke              = 0;
    int light_intensity    = 0; //光强
    int lighting           = 0; //pwm调光
    bool light_on          = DISABLE;
    bool fan_on            = DISABLE;
    //初始化标志位
    int flag = 0;
    int key1 = 0;
/*------------------这个部分是定义的变量-----------------------------*/

/*------------------这个部分是定义的变量-----------------------------*/
    float levl = 0;
    float turb = 0;
    float temp = 0;
    float ph   = 0;
    float stmp = 0;
    float smos = 0;
    float CO  = 0;
    float smok = 0;
    float lith = 0;
/*------------------这个部分是定义的变量-----------------------------*/


    QMQTT::Client *m_mqtt_client;

    //数据库
    QSqlDatabase m_database;
    QSqlTableModel *m_table;
    QSqlQuery *m_query;
    int m_index = 0;

    QTimer *m_timer;
    QTimer *m_timer1;
    QTimer *m_timer2;

};
#endif // WIDGET_H
