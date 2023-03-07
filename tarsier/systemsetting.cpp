#include "systemsetting.h"
#include "ui_systemsetting.h"
#include "settingcfg.h"

#include <QtSerialPort/QSerialPortInfo>

SystemSetting::SystemSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemSetting)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(860, 370);
    ui->sysOk->setText(NULL);
    ui->sysCancel->setText(NULL);
    //添加串口
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        ui->serialPort->addItem(info.portName());
    }
    //添加探测器列表
    ui->fpdName->addItem("无","");
    QMap<QString, QString> fMap= SettingCfg::getInstance().getSystemBaseCfg().fpdworkdirMap;
    foreach(const QString &str,fMap.keys()){
        ui->fpdName->addItem(str,fMap.value(str));
    }
    //添加休眠保护时间列表
    ui->sleepTime->addItem("off",-1);
    ui->sleepTime->addItem("1",1);
    ui->sleepTime->addItem("3",3);
    ui->sleepTime->addItem("5",5);
    ui->sleepTime->addItem("10",10);
    ui->sleepTime->addItem("15",15);
    ui->sleepTime->addItem("20",20);
    //添加自动关机时间列表
    ui->shutdownTime->addItem("off",-1);
    ui->shutdownTime->addItem("10",10);
    ui->shutdownTime->addItem("15",15);
    ui->shutdownTime->addItem("20",20);
    ui->shutdownTime->addItem("25",25);
    ui->shutdownTime->addItem("30",30);
    //数据回填
    ui->serialPort->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().serialPortName);
    ui->fpdName->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().fpdName);
    ui->sleepTime->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().sleepTime);
    ui->shutdownTime->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().shutdownTime);
}

SystemSetting::~SystemSetting()
{
    delete ui;
}


void SystemSetting::on_sysOk_clicked(){
    SystemSettingCfg &ssc=SettingCfg::getInstance().getSystemSettingCfg();
    FpdSettingCfg &fsc=SettingCfg::getInstance().getFpdSettingCfg();
    ssc.serialPortName=(ui->serialPort->currentText());
    if(ssc.fpdName!=ui->fpdName->currentText()){
        ssc.fpdName=(ui->fpdName->currentText());
        fsc.trigger=0;
    }
    ssc.sleepTime=(ui->sleepTime->currentText());
    ssc.shutdownTime=(ui->shutdownTime->currentText());
    ssc.fpdWorkDir=(SettingCfg::getInstance().getSystemBaseCfg().fpdworkdirMap.value(ui->fpdName->currentText()));
    SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
    this->close();
    emit fpdAndControllerConnect();
    emit maskWidgetClosed();
}

void SystemSetting::on_sysCancel_clicked()
{
    this->close();
    emit maskWidgetClosed();
}
