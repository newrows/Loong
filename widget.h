#ifndef WIDGET_H
#define WIDGET_H

#define Page_Main       0
#define Page_Water      1
#define Page_Soil       2
#define Page_Air        3
#define Page_Database   4
#define Page_Logs       5
#define Page_Us         6
#define Page_Aqi        7
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

#include <QtCharts>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QDateTimeAxis>

using namespace QT_CHARTS_NAMESPACE;
QT_CHARTS_USE_NAMESPACE

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

    QString Json_String(QJsonObject object, QString keyword, QString m_str);
    int Json_Int(QJsonObject object, QString keyword, int m_info);
    float Json_Float(QJsonObject object, QString keyword, float m_info);
    bool Json_Bool(QJsonObject object, QString keyword, bool m_info);

    void Database_Init();
    void M_Index_Set(int index);
    void Database_Update();
    void Database_Out(const QString &tableName,const QString &csvFileName);

    void Time_Init();
    void Timer_Init();

    void Charts_Config(int num, QValueAxis *(&m_axisX), QValueAxis *(&m_axisY), QLineSeries *(&m_lineSeries), QChart *(&m_chart), QtCharts::QChartView *(&m_ui));
    void Charts_Init();
    void Charts_Show_Config(int num, QChart *(&m_chart), QLineSeries *(&m_lineSeries), bool state, int &count);
    void Charts_Show(int num, bool state);
    void Charts_Update();

    void Vedio_Starting();
    void Vedio_Stopping();
    void Vedio_Receive();
    void Vedio_Process();

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

    void on_charts_pre_clicked();

    void on_chart_next_clicked();

    void on_air_next_clicked();

    void on_chart_water_pre_clicked();

    void on_chart_water_next_clicked();

    void on_chart_soil_pre_clicked();

    void on_chart_soil_next_clicked();

    void on_chart_air_pre_clicked();

    void on_chart_air_next_clicked();

    void on_btn_vedio_start_clicked();

    void on_btn_vedio_stop_clicked();

    void on_btn_out_clicked();

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

    QString aqoi;
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

    //charts
    int count1 = 0, count2 = 0, count3 = 0, count4 = 0, count5 = 0, count6 = 0, count7 = 0, count8 = 0, count9 = 0;
    QChart *m_chart1, *m_chart2, *m_chart3, *m_chart4;  //water
    QChart *m_chart5, *m_chart6;    //soil
    QChart *m_chart7, *m_chart8, *m_chart9; //air

    bool state1=ENABLE, state2=DISABLE, state3=DISABLE, state4=DISABLE, state5=DISABLE, state6=DISABLE, state7=DISABLE, state8=DISABLE, state9=DISABLE;

    QValueAxis *m_axisX1, *m_axisY1, *m_axisX2, *m_axisY2, *m_axisX3, *m_axisY3, *m_axisX4, *m_axisY4;
    QValueAxis *m_axisX5, *m_axisY5, *m_axisX6, *m_axisY6;
    QValueAxis *m_axisX7, *m_axisY7, *m_axisX8, *m_axisY8, *m_axisX9, *m_axisY9;

    QLineSeries *m_lineSeries1, *m_lineSeries2, *m_lineSeries3, *m_lineSeries4;
    QLineSeries *m_lineSeries5, *m_lineSeries6;
    QLineSeries *m_lineSeries7, *m_lineSeries8, *m_lineSeries9;

    const int AXIS_MAX_X1 = 10, AXIS_MAX_Y1 = 20;
    const int AXIS_MAX_X2 = 10, AXIS_MAX_Y2 = 100;
    const int AXIS_MAX_X3 = 10, AXIS_MAX_Y3 = 10;
    const int AXIS_MAX_X4 = 10, AXIS_MAX_Y4 = 100;
    const int AXIS_MAX_X5 = 10, AXIS_MAX_Y5 = 100;
    const int AXIS_MAX_X6 = 10, AXIS_MAX_Y6 = 50;
    const int AXIS_MAX_X7 = 10, AXIS_MAX_Y7 = 100;
    const int AXIS_MAX_X8 = 10, AXIS_MAX_Y8 = 20;
    const int AXIS_MAX_X9 = 10, AXIS_MAX_Y9 = 1200;

    QDateTime m_time;

    //视频传输
    bool Vedio_Start = DISABLE;
    bool Vedio_Stop  = ENABLE;

    QQueue<QByteArray> dataQueue;           //接收数据流
    QByteArray frameData;
    QByteArray frameBuffer;
    QNetworkReply *Client = nullptr;
    QNetworkAccessManager *Manager;
    bool startflag = false;

};
#endif // WIDGET_H
