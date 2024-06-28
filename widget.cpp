#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageAuthenticationCode>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_login.show();

    All_Init();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::All_Init()
{
    connect(&m_login, &Login::login_success, [=](){
        m_login.close();
        this->show();
    });

    ui->stackedWidget->setCurrentIndex(0);

    Mqtt_Init();
    Database_Init();
    Time_Init();
}

void Widget::Mqtt_Init()
{
//    QString ProductKey      = "k1dvqmKfYdi";
//    QString DeviceName      = "2k1000";
//    QString DeviceSecret    = "e90140ff0ce5790bc6e0d831f1b65622";

    QString ProductKey      = "k1dl3eJ2RBB";
    QString DeviceName      = "ESPtest";
    QString DeviceSecret    = "39021df5d0307c8e31a7d9efd83c9e9a";

    QString clientid        = "2k1000";
    QString RegionId        = "cn-shanghai";
    QString signmethod      = "hmacsha1";

    QString HostName = ProductKey + ".iot-as-mqtt." + RegionId + ".aliyuncs.com";
    QString UserName = DeviceName + "&" + ProductKey;
    QString Password = "clientId"+clientid + "deviceName"+DeviceName + "productKey"+ProductKey;
    QString ClientId = clientid + "|securemode=3,signmethod=" + signmethod + "|";

    m_mqtt_client = new QMQTT::Client(QHostAddress::LocalHost, 1883);
    m_mqtt_client->setHostName(HostName);
    m_mqtt_client->setClientId(ClientId);
    m_mqtt_client->setPort(1883);
    m_mqtt_client->setUsername(UserName);
    m_mqtt_client->setPassword(QMessageAuthenticationCode::hash(Password.toLocal8Bit(), DeviceSecret.toLocal8Bit(),
                                                                QCryptographicHash::Sha1).toHex());
    m_mqtt_client->setKeepAlive(120);   //在 1.5*Keep Alive 的时间间隔内，没有消息则断开；

    m_mqtt_client->connectToHost();

}

//发送给微信的控制位
void Widget::Mqtt_Publish()
{
    QString topic = "/k1dl3eJ2RBB/ESPtest/user/esptest";

    QJsonObject m_object;
    m_object.insert("water_1",      water_1);
    m_object.insert("water_2",      water_2);
    m_object.insert("water_filter", water_filter);
    m_object.insert("oxyen_on",     oxyen_on);
    m_object.insert("water_on",     water_on);
    m_object.insert("lighting",     lighting);
    m_object.insert("light_on",     light_on);
    m_object.insert("fan_on",       fan_on);

    QJsonDocument m_doc;
    m_doc.setObject(m_object);
    QByteArray m_data = m_doc.toJson();
    QMQTT::Message message(1, topic, m_data);
    m_mqtt_client->publish(message);
}

void Widget::Mqtt_Subscribe(QMQTT::Message message)
{
    QByteArray pay_load = message.payload();
    qDebug()<<pay_load;
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(pay_load, &jsonError);

    //上述为将收到的消息包转化json包格式
    if (doc.isObject())
    {
        QJsonObject object = doc.object();  // 转化为对象

        water_level         = Json_Int (object, "water_level", water_level);
        water_turbidity     = Json_Int (object, "water_turbidity", water_turbidity);
        water_tempreture    = Json_Int (object, "water_tempreture", water_tempreture);
        water_ph            = Json_Int (object, "water_ph", water_ph);
        water_1             = Json_Bool(object, "water_1", water_1);
        water_2             = Json_Bool(object, "water_2", water_2);
        water_filter        = Json_Bool(object, "water_filter", water_filter);
        oxyen_on            = Json_Bool(object, "oxyen_on", oxyen_on);
        soil_temperature    = Json_Int (object, "soil_temperature", soil_temperature);
        soil_moisture       = Json_Int (object, "soil_moisture", soil_moisture);
        water_on            = Json_Bool(object, "water_on", water_on);
        CO2                 = Json_Int (object, "CO2", CO2);
        smoke               = Json_Int (object, "smoke", smoke);
        light_intensity     = Json_Int (object, "light_intensity", light_intensity);
//        lighting            = Json_Int (object, "lighting", lighting);
        light_on            = Json_Bool(object, "light_on", light_on);
        fan_on              = Json_Bool(object, "fan_on", fan_on);
        key1                = Json_Int (object, "key1", key1);
    }

    Infomation_Tran();
    qDebug()<<"Successful Trans!";
    Infomation_Show();
//    if(key1 == 1)
//    {
//        Infomation_Send();
//        key1 = 0;
//    }
    Database_Update();
//    Infomation_Send();
}

//发送给下位机的控制位
void Widget::Infomation_Send()
{
    QString topic = "/k1dl3eJ2RBB/ESPtest/user/update";

    QString wat1 = State_Config(water_1);
    QString wat2 = State_Config(water_2);
    QString watfil = State_Config(water_filter);
    QString oxy = State_Config(oxyen_on);
    QString waton = State_Config(water_on);
    QString ligon = State_Config(light_on);
    QString fanon = State_Config(fan_on);
    QString liging = QString("%1").arg(lighting);

    QString str = wat1+wat2+watfil+oxy+waton+ligon+fanon+liging;

    QByteArray m_data = str.toLatin1();
    qDebug()<<m_data;

    QMQTT::Message message(1, topic, m_data);
    m_mqtt_client->publish(message);

}

//发送给微信的处理好的数据
void Widget::Infomation_Tran()
{
    QString topic = "/k1dl3eJ2RBB/ESPtest/user/mqtt1";

    QJsonObject m_object;
    m_object.insert("water_level", levl);
    m_object.insert("water_turbidity", turb);
    m_object.insert("water_tempreture", temp);
    m_object.insert("water_ph", ph);
    m_object.insert("soil_temperature", stmp);
    m_object.insert("soil_moisture", smos);
    m_object.insert("CO2", CO);
    m_object.insert("smoke", smok);
    m_object.insert("light_intensity", lith);

    QJsonDocument m_doc;
    m_doc.setObject(m_object);
    QByteArray m_data = m_doc.toJson();
    qDebug()<<m_data;
    QMQTT::Message message(1, topic, m_data);
    m_mqtt_client->publish(message);

}

QString Widget::State_Config(bool state)
{
    QString str;
    if(ENABLE == state)
        str = "1";
    else
        str = "0";
    return str;
}

void Widget::Info_Handling()
{
    levl = Sqr_Pow(water_level);
    temp = Sqr_Pow(water_tempreture);
    stmp = Sqr_Pow(soil_temperature);
    smos = Sqr_Pow(soil_moisture);
    CO   = Sqr_Pow(CO2);
    smok = Sqr_Pow(smoke);
    lith = Sqr_Pow(light_intensity);

    turb = water_turbidity * 3.3 /4096;
    turb = turb *100 /3.3;
    if(turb > 100)  turb = 100;

    ph = water_ph * 3.3 /4096;
    ph = -5.7541*ph + 16.654;
}

float Widget::Sqr_Pow(int date)
{
    float m_date = sqrt(date);
    QString str = QString::number(m_date, 'f', 2);
    float num = str.toFloat();
    return num;
}

void Widget::Infomation_Show()
{
    Info_Handling();
    ui->le_water_level  ->setText(QString::number(levl));
    ui->le_water_turb   ->setText(QString::number(turb));
    ui->le_water_temp   ->setText(QString::number(temp));
    ui->le_water_ph     ->setText(QString::number(ph));
    ui->le_soil_temp    ->setText(QString::number(stmp));
    ui->le_soil_mois    ->setText(QString::number(smos));
    ui->le_air_co2      ->setText(QString::number(CO));
    ui->le_air_smoke    ->setText(QString::number(smok));
    ui->le_air_lighting ->setText(QString::number(lith));
//    ui->sld_lighting    ->setValue(lighting);

    Button_Set(ui->btn_water1,  water_1);
    Button_Set(ui->btn_water2,  water_2);
    Button_Set(ui->btn_filter,  water_filter);
    Button_Set(ui->btn_oxyen,  oxyen_on);
    Button_Set(ui->btn_water_on,  water_on);
    Button_Set(ui->btn_blow,  fan_on);
}

void Widget::Button_Set(QPushButton *button, bool state)
{
    QPushButton *m_button = button;
    if(ENABLE == state)
        m_button->setText("ON");
    else
        m_button->setText("OFF");
    m_button->setChecked(state);
}

void Widget::Button_Clk(QPushButton *button, bool state)
{
    QPushButton *m_button = button;
    if(ENABLE == state)
    {
        m_button->setText("OFF");
        state = DISABLE;
    }
    else
    {
        m_button->setText("ON");
        state = ENABLE;
    }
}

QString Widget::Json_String(QJsonObject object, QString keyword)
{
    QJsonValue m_key = object.value(keyword);
    QString str = m_key.toString();
    return str;
}

int Widget::Json_Int(QJsonObject object, QString keyword, int m_info)
{
    int m_case = m_info;
    if(object.contains(keyword))
    {
        QJsonValue m_key = object.value(keyword);
        int data = m_key.toInt();
        return data;
    }
    return m_case;
}

bool Widget::Json_Bool(QJsonObject object, QString keyword, bool m_info)
{
    bool m_case = m_info;
    if(object.contains(keyword))
    {
        QJsonValue m_key = object.value(keyword);
        bool state = m_key.toBool();
        return state;
    }
    return m_case;
}

void Widget::Database_Init()
{
    qDebug() << "Available SQL Drivers: " << QSqlDatabase::drivers();

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("newrows.db");
    if(!m_database.open())
        qDebug()<<"Database Open Failed!";
    else
        qDebug()<<"Database Open Succeed!";
    m_query = new QSqlQuery;
    QString str_command = "CREATE TABLE Newrows("
                          "water_level VARCHAR(20),"
                          "water_turbidity VARCHAR(20),"
                          "water_tempreture VARCHAR(20),"
                          "water_ph VARCHAR(20),"
                          "soil_temperature VARCHAR(20),"
                          "soil_moisture VARCHAR(20),"
                          "CO2 VARCHAR(20),"
                          "light_intensity VARCHAR(20)"
//                          "smoke VARCHAR(20)"
                          ")";
    if(!m_query->exec(str_command))
    {
        qDebug() << "SQL Error: " << m_query->lastError().text();
        qDebug()<<"Database Create Failed!";
    }

    m_table = new QSqlTableModel(this);
    m_table->setTable("Newrows");
    m_table->select();
    m_table->setHeaderData(0, Qt::Horizontal, "水位");
    m_table->setHeaderData(1, Qt::Horizontal, "浊度");
    m_table->setHeaderData(2, Qt::Horizontal, "水温");
    m_table->setHeaderData(3, Qt::Horizontal, "PH");
    m_table->setHeaderData(4, Qt::Horizontal, "温度");
    m_table->setHeaderData(5, Qt::Horizontal, "土壤湿度");
    m_table->setHeaderData(6, Qt::Horizontal, "CO2浓度");
    m_table->setHeaderData(7, Qt::Horizontal, "光照强度");
//    m_table->setHeaderData(8, Qt::Horizontal, "烟雾浓度");

    m_table->setEditStrategy(QSqlTableModel::OnManualSubmit);   //设置为提交更新数据

    ui->tabv_db->setModel(m_table);
    ui->tabv_db->setColumnWidth(0, 0);
    ui->tabv_db->verticalHeader()->hide();
    ui->tabv_db->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //设置拉伸
    RewriteStyledItemDelegate *m_delegate = new RewriteStyledItemDelegate;
    ui->tabv_db->setItemDelegate(m_delegate);
    ui->cob_info->setItemDelegate(m_delegate);
    connect(ui->cob_info, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Widget::M_Index_Set);

}

void Widget::M_Index_Set(int index)
{
    m_index = index;
    qDebug()<<QString::number(m_index);
}

void Widget::Database_Update()
{
    QString sql =QString("insert into Newrows values('%1','%2','%3','%4','%5','%6','%7','%8')")
            .arg(ui->le_water_level->text())
            .arg(ui->le_water_turb->text())
            .arg(ui->le_water_temp->text())
            .arg(ui->le_water_ph->text())
            .arg(ui->le_soil_temp->text())
            .arg(ui->le_soil_mois->text())
            .arg(ui->le_air_co2->text())
            .arg(ui->le_air_lighting->text());

    if(!m_query->exec(sql))
    {
        qDebug()<<"无法执行sql语句1";
    }

    m_table->database().transaction();

//    if (m_table->submitAll())
//    {
//        m_table->database().commit();
//    } else
//    {
//        m_table->database().rollback();
//    }
}

void Widget::Time_Init()
{
    QDateTime m_time = QDateTime::currentDateTime();
    ui->lb_hour->setText(m_time.toString("hh"));
    ui->lb_min->setText(m_time.toString("mm"));
    ui->lb_date->setText(m_time.toString("M月dd日"));
    ui->lb_week->setText(m_time.toString("dddd"));
}

void Widget::Timer_Init()
{
    m_timer = new QTimer;
    m_timer1= new QTimer;
    m_timer2= new QTimer;

    m_timer ->setInterval(1000);
//    m_timer1->setInterval(800);
//    m_timer2->setInterval(1000);

//    connect(m_timer,  &QTimer::timeout, this, &Widget::Infomation_Tran);
    connect(m_timer,  &QTimer::timeout, this, &Widget::Infomation_Send);
//    connect(m_timer2, &QTimer::timeout, this, &Widget::Infomation_Tran);
    m_timer ->start();
//    m_timer2->start();
//    m_timer2->start();
}

void Widget::on_btn_mainpage_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Main);
}

void Widget::on_btn_timedata_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Water);
}

void Widget::on_btn_database_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Database);
}

void Widget::on_btn_logs_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Logs);
}

void Widget::on_btn_us_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Us);
}

void Widget::on_water_next_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Soil);
}

void Widget::on_soil_pre_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Water);
}

void Widget::on_soil_next_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Air);
}

void Widget::on_air_pre_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Soil);
}

void Widget::on_btn_config_clicked()
{
    if(flag == 0)
    {
        flag = 1;
        QString subscription="/k1dl3eJ2RBB/ESPtest/user/get";
        m_mqtt_client->subscribe(subscription);
        qDebug()<<"ok";
        connect(m_mqtt_client, &QMQTT::Client::received, this, &Widget::Mqtt_Subscribe);
    }
    Timer_Init();
}

void Widget::on_btn_filter_clicked()
{
    if(water_filter == ENABLE)
    {
        water_filter = DISABLE;
        ui->btn_filter->setText("OFF");
    }
    else
    {
        water_filter = ENABLE;
        ui->btn_filter->setText("ON");
    }
    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_water1_clicked()
{
    if(water_1 == ENABLE)
    {
        water_1 = DISABLE;
        ui->btn_water1->setText("OFF");
    }
    else
    {
        water_1 = ENABLE;
        ui->btn_water1->setText("ON");
    }
    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_water2_clicked()
{
    if(water_2 == ENABLE)
    {
        water_2 = DISABLE;
        ui->btn_water2->setText("OFF");
    }
    else
    {
        water_2 = ENABLE;
        ui->btn_water2->setText("ON");
    }
    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_oxyen_clicked()
{
    if(oxyen_on == ENABLE)
    {
        oxyen_on = DISABLE;
        ui->btn_oxyen->setText("OFF");
    }
    else
    {
        oxyen_on = ENABLE;
        ui->btn_oxyen->setText("ON");
    }
    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_water_on_clicked()
{
    if(water_on == ENABLE)
    {
        water_on = DISABLE;
        ui->btn_water_on->setText("OFF");
    }
    else
    {
        water_on = ENABLE;
        ui->btn_water_on->setText("ON");
    }
//    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_blow_clicked()
{
    if(fan_on == ENABLE)
    {
        fan_on = DISABLE;
        ui->btn_blow->setText("OFF");
    }
    else
    {
        fan_on = ENABLE;
        ui->btn_blow->setText("ON");
    }
//    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_light_on_clicked()
{
    light_on = ENABLE;
    lighting = ui->sld_lighting->value();
//    Mqtt_Publish();
//    Infomation_Send();
}

void Widget::on_btn_update_clicked()
{
    m_table->database().transaction();
    if (m_table->submitAll())
    {
        if(m_table->database().commit()) // 提交
            QMessageBox::information(this, "数据更新提示", "数据更新提示成功！");
    } else
    {
        m_table->database().rollback(); // 回滚
        QMessageBox::warning(this, "数据更新提示", QString("数据库出错！：%1").arg(m_table->lastError().text()), QMessageBox::Ok);
    }
}

void Widget::on_btn_clear_clicked()
{
    int ok = QMessageBox::warning(this, "清除全表!", "你确定清除全表吗？", QMessageBox::Yes, QMessageBox::No);
    if(ok == QMessageBox::No)
    { // 如果不删除，则撤销
        m_table->revertAll();
    } else
    { // 否则提交，删除整个数据库
        m_query->exec("delete from Newrows");
        m_table->select();
    }
}

void Widget::on_btn_find_clicked()
{
    QString item_name;
    switch (m_index) {
    case 0:
        item_name = "water_level"; break;
    case 1:
        item_name = "water_turbidity"; break;
    case 2:
        item_name = "water_tempreture"; break;
    case 3:
        item_name = "water_ph"; break;
    case 4:
        item_name = "soil_temperature"; break;
    case 5:
        item_name = "soil_moisture"; break;
    case 6:
        item_name = "CO2"; break;
    case 7:
        item_name = "smoke"; break;
    case 8:
        item_name = "light_intensity"; break;
    }
    m_table->setFilter(QString("%1 = '%2'").arg(item_name).arg(ui->le_find->text()));
    m_table->select();
}


