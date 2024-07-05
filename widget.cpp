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
    Charts_Init();
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
        aqoi                = Json_String(object, "aqi", aqoi);
    }

    Infomation_Tran();
    qDebug()<<aqoi;
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
    QString str;

    QString wat1 = State_Config(water_1);
    QString wat2 = State_Config(water_2);
    QString watfil = State_Config(water_filter);
    QString oxy = State_Config(oxyen_on);
    QString waton = State_Config(water_on);
    QString ligon = State_Config(light_on);
    QString fanon = State_Config(fan_on);
    QString liging = QString("%1").arg(lighting);

    if(liging.length() == 1)
        str = wat1 + wat2 + watfil + oxy + waton + ligon + fanon + "0" + liging;
    else
        str = wat1 + wat2 + watfil + oxy + waton + ligon + fanon + liging;
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
    lith = float(light_intensity);

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

QString Widget::Json_String(QJsonObject object, QString keyword, QString m_str)
{
    QString m_case = m_str;
    if(object.contains(keyword))
    {
        QJsonValue m_key = object.value(keyword);
        QString str = m_key.toString();
        return str;
    }
    return m_case;
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

float Widget::Json_Float(QJsonObject object, QString keyword, float m_info)
{
    float m_case = m_info;
    if(object.contains(keyword))
    {
        QJsonValue m_key = object.value(keyword);
        float data = m_key.toDouble();
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
                          "time VARCHAR(20),"
                          "water_level VARCHAR(20),"
                          "water_turbidity VARCHAR(20),"
                          "water_tempreture VARCHAR(20),"
                          "water_ph VARCHAR(20),"
                          "soil_temperature VARCHAR(20),"
                          "soil_moisture VARCHAR(20),"
                          "CO2 VARCHAR(20),"
                          "light_intensity VARCHAR(20),"
                          "smoke VARCHAR(20),"
                          "aqi VARCHAR(20)"
                          ")";
    if(!m_query->exec(str_command))
    {
        qDebug() << "SQL Error: " << m_query->lastError().text();
        qDebug()<<"Database Create Failed!";
    }

    m_table = new QSqlTableModel(this);
    m_table->setTable("Newrows");
    m_table->select();
    m_table->setHeaderData(0, Qt::Horizontal, "时间");
    m_table->setHeaderData(1, Qt::Horizontal, "水位");
    m_table->setHeaderData(2, Qt::Horizontal, "浊度");
    m_table->setHeaderData(3, Qt::Horizontal, "水温");
    m_table->setHeaderData(4, Qt::Horizontal, "PH");
    m_table->setHeaderData(5, Qt::Horizontal, "温度");
    m_table->setHeaderData(6, Qt::Horizontal, "土壤湿度");
    m_table->setHeaderData(7, Qt::Horizontal, "CO2浓度");
    m_table->setHeaderData(8, Qt::Horizontal, "光照强度");
    m_table->setHeaderData(9, Qt::Horizontal, "烟雾浓度");
    m_table->setHeaderData(10, Qt::Horizontal, "aqi指数");

    m_table->setEditStrategy(QSqlTableModel::OnManualSubmit);   //设置为提交更新数据

    ui->tabv_db->setModel(m_table);
    ui->tabv_db->setColumnWidth(0, 100);
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
    QString sql =QString("insert into Newrows values('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg(m_time.toString("M月dd日hh:mm"))
            .arg(ui->le_water_level->text())
            .arg(ui->le_water_turb->text())
            .arg(ui->le_water_temp->text())
            .arg(ui->le_water_ph->text())
            .arg(ui->le_soil_temp->text())
            .arg(ui->le_soil_mois->text())
            .arg(ui->le_air_co2->text())
            .arg(ui->le_air_lighting->text())
            .arg(ui->le_air_smoke->text())
            .arg(aqoi);

    if(!m_query->exec(sql))
    {
        qDebug()<<"无法执行sql语句";
    }

    m_table->database().transaction();
}

void Widget::Database_Out(const QString &tableName,const QString &csvFileName)
{
    QSqlTableModel *db = new QSqlTableModel();
    db->setTable(tableName);
    db->select();

    QStringList strList;//记录数据库中的一行报警数据
    QString strString;
    const QString FILE_PATH(csvFileName);

    QFile csvFile(FILE_PATH);
    if (csvFile.open(QIODevice::ReadWrite))
    {
        for (int i=0;i<db->rowCount();i++)
        {
            for(int j=0;j<db->columnCount();j++)
            {
                strList.insert(j,db->data(db->index(i,j)).toString());//把每一行的每一列数据读取到strList中
            }

    strString = strList.join(", ")+"\n";//给两个列数据之前加“,”号，一行数据末尾加回车
    strList.clear();//记录一行数据后清空，再记下一行数据
    csvFile.write(strString.toUtf8());//使用方法：转换为Utf8格式后在windows下的excel打开是乱码,可先用notepad++打开并转码为unicode，再次用excel打开即可。
    }
    csvFile.close();
    }
}

void Widget::Time_Init()
{
    m_time = QDateTime::currentDateTime();
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

    m_timer ->setInterval(1200);
    m_timer1->setInterval(1500);
//    m_timer2->setInterval(1000);

//    connect(m_timer,  &QTimer::timeout, this, &Widget::Infomation_Tran);
    connect(m_timer,  &QTimer::timeout, this, &Widget::Infomation_Send);
//    connect(m_timer,  &QTimer::timeout, this, &Widget::Charts_Show);
    connect(m_timer1, &QTimer::timeout, this, &Widget::Charts_Update);
    m_timer ->start();
    m_timer1->start();
    //    m_timer2->start();
}

void Widget::Charts_Config(int num, QValueAxis *(&m_axisX), QValueAxis *(&m_axisY), QLineSeries *(&m_lineSeries), QChart *(&m_chart), QtCharts::QChartView *(&m_ui))
{
    QString str_y;
    QString str_title;
    int MAX_X;
    int MAX_Y;

    switch (num) {
    case 1:
        str_y = "水位"; str_title = "水位"; MAX_X = AXIS_MAX_X1; MAX_Y = AXIS_MAX_Y1; break;
    case 2:
        str_y = "浊度"; str_title = "浊度"; MAX_X = AXIS_MAX_X2; MAX_Y = AXIS_MAX_Y2; break;
    case 3:
        str_y = "PH值"; str_title = "PH值"; MAX_X = AXIS_MAX_X3; MAX_Y = AXIS_MAX_Y3; break;
    case 4:
        str_y = "水温"; str_title = "水温"; MAX_X = AXIS_MAX_X4; MAX_Y = AXIS_MAX_Y4; break;
    case 5:
        str_y = "温度"; str_title = "温度"; MAX_X = AXIS_MAX_X5; MAX_Y = AXIS_MAX_Y5; break;
    case 6:
        str_y = "土壤湿度"; str_title = "土壤湿度"; MAX_X = AXIS_MAX_X6; MAX_Y = AXIS_MAX_Y6; break;
    case 7:
        str_y = "CO2浓度"; str_title = "CO2浓度"; MAX_X = AXIS_MAX_X7; MAX_Y = AXIS_MAX_Y7; break;
    case 8:
        str_y = "烟雾浓度"; str_title = "烟雾浓度"; MAX_X = AXIS_MAX_X8; MAX_Y = AXIS_MAX_Y8; break;
    case 9:
        str_y = "光照强度"; str_title = "光照强度"; MAX_X = AXIS_MAX_X9; MAX_Y = AXIS_MAX_Y9; break;
    }

    m_axisX = new QValueAxis;
    m_axisY = new QValueAxis;
    m_axisX->setTitleText("count");
    m_axisY->setTitleText(str_y);
    m_axisX->setMin(0);
    m_axisY->setMin(0);
    m_axisX->setMax(MAX_X);
    m_axisY->setMax(MAX_Y);

    m_lineSeries = new QLineSeries;
    m_lineSeries->setPointsVisible(true);
    m_lineSeries->setName(str_title);

    m_chart = new QChart;
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_chart->addSeries(m_lineSeries);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    m_lineSeries->attachAxis(m_axisX); //曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    m_lineSeries->attachAxis(m_axisY); //曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

    m_ui->setChart(m_chart);
    m_ui->setRenderHint(QPainter::Antialiasing);

}

void Widget::Charts_Init()
{
    Charts_Config(1, m_axisX1, m_axisY1, m_lineSeries1, m_chart1, ui->chart_level);
    Charts_Config(2, m_axisX2, m_axisY2, m_lineSeries2, m_chart2, ui->chart_turb);
    Charts_Config(3, m_axisX3, m_axisY3, m_lineSeries3, m_chart3, ui->chart_ph);
    Charts_Config(4, m_axisX4, m_axisY4, m_lineSeries4, m_chart4, ui->chart_water_temp);
    Charts_Config(5, m_axisX5, m_axisY5, m_lineSeries5, m_chart5, ui->chart_temp);
    Charts_Config(6, m_axisX6, m_axisY6, m_lineSeries6, m_chart6, ui->chart_mois);
    Charts_Config(7, m_axisX7, m_axisY7, m_lineSeries7, m_chart7, ui->chart_co2);
    Charts_Config(8, m_axisX8, m_axisY8, m_lineSeries8, m_chart8, ui->chart_smoke);
    Charts_Config(9, m_axisX9, m_axisY9, m_lineSeries9, m_chart9, ui->chart_light);


/*
    m_axisX1 = new QValueAxis;
    m_axisY1 = new QValueAxis;
    m_axisX1->setTitleText("x-label");
    m_axisY1->setTitleText("水位");
    m_axisX1->setMin(0);
    m_axisY1->setMin(0);
    m_axisX1->setMax(AXIS_MAX_X1);
    m_axisY1->setMax(AXIS_MAX_Y1);

    m_lineSeries1 = new QLineSeries;
    m_lineSeries1->setPointsVisible(true);
    m_lineSeries1->setName("Demo");

    m_chart1 = new QChart;
    m_chart1->addAxis(m_axisX1, Qt::AlignBottom);
    m_chart1->addAxis(m_axisY1, Qt::AlignLeft);
    m_chart1->addSeries(m_lineSeries1);
    m_chart1->setAnimationOptions(QChart::SeriesAnimations);

    m_lineSeries1->attachAxis(m_axisX1); //曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    m_lineSeries1->attachAxis(m_axisY1); //曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后

    ui->chart_level->setChart(m_chart1);
    ui->chart_level->setRenderHint(QPainter::Antialiasing);
*/
}

void Widget::Charts_Show_Config(int num, QChart *(&m_chart), QLineSeries *(&m_lineSeries), bool state, int &count)
{
    if(ENABLE == state)
    {
        float my_date;
        int MAX_X;
        switch (num) {
        case 1:
            my_date = levl; MAX_X = AXIS_MAX_X1; count = count1; break;
        case 2:
            my_date = turb; MAX_X = AXIS_MAX_X2; count = count2; break;
        case 3:
            my_date = ph  ; MAX_X = AXIS_MAX_X3; count = count3; break;
        case 4:
            my_date = temp; MAX_X = AXIS_MAX_X4; count = count4; break;
        case 5:
            my_date = stmp; MAX_X = AXIS_MAX_X5; count = count5; break;
        case 6:
            my_date = smos; MAX_X = AXIS_MAX_X6; count = count6; break;
        case 7:
            my_date = CO  ; MAX_X = AXIS_MAX_X7; count = count7; break;
        case 8:
            my_date = smok; MAX_X = AXIS_MAX_X8; count = count8; break;
        case 9:
            my_date = lith; MAX_X = AXIS_MAX_X9; count = count9; break;
        }

        if(count > MAX_X)
        {
            m_lineSeries->remove(0);
            m_chart->axes(Qt::Horizontal).back()->setMin(count - MAX_X);
            m_chart->axes(Qt::Horizontal).back()->setMax(count);
        }
            m_lineSeries->append(QPointF(count, my_date));
            count++;
    }
    else
        return;
}

void Widget::Charts_Show(int num, bool state)
{
    switch (num) {
    case 1:
            Charts_Show_Config(1, m_chart1, m_lineSeries1, state, count1); break;
    case 2:
            Charts_Show_Config(2, m_chart2, m_lineSeries2, state, count2); break;
    case 3:
            Charts_Show_Config(3, m_chart3, m_lineSeries3, state, count3); break;
    case 4:
            Charts_Show_Config(4, m_chart4, m_lineSeries4, state, count4); break;
    case 5:
            Charts_Show_Config(5, m_chart5, m_lineSeries5, state, count5); break;
    case 6:
            Charts_Show_Config(6, m_chart6, m_lineSeries6, state, count6); break;
    case 7:
            Charts_Show_Config(7, m_chart7, m_lineSeries7, state, count7); break;
    case 8:
            Charts_Show_Config(8, m_chart8, m_lineSeries8, state, count8); break;
    case 9:
            Charts_Show_Config(9, m_chart9, m_lineSeries9, state, count9); break;
    }

}

void Widget::Charts_Update()
{
    Charts_Show(1, state1);
    Charts_Show(2, state2);
    Charts_Show(3, state3);
    Charts_Show(4, state4);
    Charts_Show(5, state5);
    Charts_Show(6, state6);
    Charts_Show(7, state7);
    Charts_Show(8, state8);
    Charts_Show(9, state9);
}

void Widget::Vedio_Starting()
{
    if(Vedio_Start == DISABLE)
        return;

    QString ipAddress = "192.168.0.104";
    //视频流的http请求命令格式，例如："http://192.168.1.8:81/stream"
    QNetworkRequest request;
    QString url="http://" + ipAddress + ":80//mjpeg/1";
    request.setUrl(QUrl(url));
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("User-Agent", "1601");

    Manager = new QNetworkAccessManager();
    Client = Manager->get(request);

    connect(Client, &QNetworkReply::readyRead, this, &Widget::Vedio_Receive);

    qDebug()<<url;
}

void Widget::Vedio_Stopping()
{
    if(Vedio_Stop == DISABLE)
        return;

    Client->close();
    Client->deleteLater();
}

void Widget::Vedio_Receive()
{
    QByteArray buffer = Client->readAll();
    QString data(buffer);

    //对接收的到的数据放到队列中
    int pos = data.indexOf("Content-Type");
    if(pos != -1)
    {
        frameData.append(buffer.left(pos));
        dataQueue.enqueue(frameData);
        frameData.clear();
        frameData.append(buffer.mid(pos));

    } else {
        frameData.append(buffer);
    }

    //触发void dataProcess()函数，合成图片和显示
    if (!dataQueue.isEmpty()) {
        frameBuffer = dataQueue.dequeue();
        Vedio_Process();
    }
}

void Widget::Vedio_Process()
{
    QString data = QString::fromUtf8(frameBuffer.data(), 50); //截取前面50个字符

    const QString lengthKeyword = "Content-Length: ";

    int lengthIndex = data.indexOf(lengthKeyword);
    if (lengthIndex >= 0){
        int endIndex = data.indexOf("\r\n", lengthIndex);
        int length = data.midRef(lengthIndex + 16, endIndex - (lengthIndex + 16 - 1)).toInt(); //取出Content-Length后的数字
        QPixmap pixmap;
        auto loadStatus = pixmap.loadFromData(frameBuffer.mid(endIndex + 4, length));

        //合成失败
        if (!loadStatus) {
            qDebug() << "Video load failed";
            frameBuffer.clear();
            return;
        }
        frameBuffer.clear();
        QPixmap pps = pixmap.scaled(ui->lb_vedio->width(), ui->lb_vedio->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->lb_vedio->setPixmap(pps);
     }
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
    state5 = ENABLE;
}

void Widget::on_soil_pre_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Water);
}

void Widget::on_soil_next_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Air);
    state7 = ENABLE;
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
    Mqtt_Publish();
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
    Mqtt_Publish();
    //    Infomation_Send();
}

void Widget::on_light_on_clicked()
{
    light_on = ENABLE;
    lighting = ui->sld_lighting->value();
    Mqtt_Publish();
    //    Infomation_Send();
}

void Widget::on_btn_update_clicked()
{
    m_table->setFilter("'' = ''");
    m_table->select();
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
    case 9:
        item_name = "time"; break;
    }
    m_table->setFilter(QString("%1 = '%2'").arg(item_name).arg(ui->le_find->text()));
    m_table->select();
}


void Widget::on_charts_pre_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Air);
}

void Widget::on_chart_next_clicked()
{
}

void Widget::on_air_next_clicked()
{
    ui->stackedWidget->setCurrentIndex(Page_Aqi);
}

void Widget::on_chart_water_pre_clicked()
{
    int page = ui->sw_charts->currentIndex();
    switch (page) {
    case 1:        state2 = DISABLE; state1 = ENABLE; break;
    case 2:        state3 = DISABLE; state2 = ENABLE; break;
    case 3:        state4 = DISABLE; state3 = ENABLE; break;
    }

    if(page != 0)
        ui->sw_charts->setCurrentIndex(page - 1);
}

void Widget::on_chart_water_next_clicked()
{
    int page = ui->sw_charts->currentIndex();
    switch (page) {
    case 0:        state2 = ENABLE; state1 = DISABLE; break;
    case 1:        state3 = ENABLE; state2 = DISABLE; break;
    case 2:        state4 = ENABLE; state3 = DISABLE; break;
    }

    if(page != 3)
        ui->sw_charts->setCurrentIndex(page + 1);
}

void Widget::on_chart_soil_pre_clicked()
{
    int page = ui->sw_charts_soil->currentIndex();
    state6 = DISABLE; state5 = ENABLE;

    if(page != 0)
        ui->sw_charts_soil->setCurrentIndex(page - 1);
}

void Widget::on_chart_soil_next_clicked()
{
    int page = ui->sw_charts_soil->currentIndex();
    state6 = ENABLE; state5 = DISABLE;

    if(page != 1)
        ui->sw_charts_soil->setCurrentIndex(page + 1);
}

void Widget::on_chart_air_pre_clicked()
{
    int page = ui->sw_charts_air->currentIndex();
    switch (page) {
    case 1:        state8 = DISABLE; state7 = ENABLE; break;
    case 2:        state9 = DISABLE; state8 = ENABLE; break;
    }

    if(page != 0)
        ui->sw_charts_air->setCurrentIndex(page - 1);
}

void Widget::on_chart_air_next_clicked()
{
    int page = ui->sw_charts_air->currentIndex();
    switch (page) {
    case 0:        state8 = ENABLE; state7 = DISABLE; break;
    case 1:        state9 = ENABLE; state8 = DISABLE; break;
    }

    if(page != 2)
        ui->sw_charts_air->setCurrentIndex(page + 1);
}

void Widget::on_btn_vedio_start_clicked()
{
    qDebug()<<"starting!!!!!!";
    ui->btn_vedio_start->setEnabled(DISABLE);
    ui->btn_vedio_stop->setEnabled(ENABLE);
    Vedio_Start = ENABLE;
    Vedio_Stop  = DISABLE;
    Vedio_Starting();

}

void Widget::on_btn_vedio_stop_clicked()
{
    qDebug()<<"stopping!!!!!!";
    ui->btn_vedio_start->setEnabled(ENABLE);
    ui->btn_vedio_stop->setEnabled(DISABLE);
    Vedio_Stop  = ENABLE;
    Vedio_Start = DISABLE;
    Vedio_Stopping();

}

void Widget::on_btn_out_clicked()
{
    Database_Out("Newrows", "Newrows.csv");
}
