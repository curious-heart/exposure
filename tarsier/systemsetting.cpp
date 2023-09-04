#include "systemsetting.h"
#include "ui_systemsetting.h"
#include "settingcfg.h"
#include "version_def.h"
#include "fpdmodels.h"
#include "logger.h"

#include <QtSerialPort/QSerialPortInfo>

SystemSetting::SystemSetting(QWidget *parent, CFpdModels * fpd_models) :
    QDialog(parent),
    ui(new Ui::SystemSetting), m_fpd_models(fpd_models)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(860, 370);
    ui->sysOk->setText(NULL);
    ui->sysCancel->setText(NULL);
    QString s_port_in_cfg_file = SettingCfg::getInstance().getSystemSettingCfg().hvCtrlIntfName;
    bool s_port_match = false;
    //添加串口
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        ui->hvCtrlIntf->addItem(info.portName());
        if(info.portName() == s_port_in_cfg_file)
        {
            s_port_match = true;
        }
    }
    //add network interface card for modbus tcp
    ui->hvCtrlIntf->addItem(gs_hvCtrlIntfName_NIC);
    if(gs_hvCtrlIntfName_NIC == s_port_in_cfg_file)
    {
        s_port_match = true;
    }
    if(!s_port_match)
    {
        DIY_LOG(LOG_WARN, QString("HV controller interface in cfg file is %1,"
                                  " but there is no such interface on current machine."
                                  "So we update the cfg file to the 1st interface on machine: %2.")
                            .arg(s_port_in_cfg_file, ui->hvCtrlIntf->itemText(0)));

        SystemSettingCfg &ssc=SettingCfg::getInstance().getSystemSettingCfg();
        ssc.hvCtrlIntfName = ui->hvCtrlIntf->itemText(0);
        SettingCfg::getInstance().writeSettingConfig(&ssc, nullptr);
    }
    //添加探测器列表
    ui->fpdName->addItem(FPD_NAME_NONE_UI_STR, "");
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
    ui->hvCtrlIntf->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().hvCtrlIntfName);
    QString ui_s = SettingCfg::getInstance().fpd_name_internal_to_ui(SettingCfg::getInstance().getSystemSettingCfg().fpdName);
    ui->fpdName->setCurrentText(ui_s);
    ui->sleepTime->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().sleepTime);
    ui->shutdownTime->setCurrentText(SettingCfg::getInstance().getSystemSettingCfg().shutdownTime);

    ui->versionLabel->setText(APP_VER_STR);
}

SystemSetting::~SystemSetting()
{
    delete ui;
}


void SystemSetting::on_sysOk_clicked(){
    SystemSettingCfg &ssc=SettingCfg::getInstance().getSystemSettingCfg();
    FpdSettingCfg &fsc=SettingCfg::getInstance().getFpdSettingCfg();
    ssc.hvCtrlIntfName=(ui->hvCtrlIntf->currentText());
    QString in_s = SettingCfg::getInstance().fpd_name_ui_to_internal(ui->fpdName->currentText());
    if(ssc.fpdName != in_s)
    {
        ssc.fpdName = in_s;
        struct FpdSettingCfg* his = SettingCfg::getInstance().get_fpd_his(in_s);
        if(his)
        {
            fsc.trigger = his->trigger;
        }
        else
        {
            fpd_model_info_t* inf;
            inf = m_fpd_models->get_fpd_minfo_from_name(in_s);
            if(inf)
            {
                fsc.trigger = inf->def_trigger_mdde;
            }
            else
            {
                fsc.trigger = INVALID_TRIGGER_MODE;
            }
            SettingCfg::getInstance().update_fpd_setting_his();
        }
    }
    ssc.sleepTime=(ui->sleepTime->currentText());
    ssc.shutdownTime=(ui->shutdownTime->currentText());
    ssc.fpdWorkDir=(SettingCfg::getInstance().getSystemBaseCfg().fpdworkdirMap.value(in_s));
    SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
    this->close();
    emit fpdAndControllerConnect();
    emit maskWidgetClosed();
    emit systemSettingAccepted();
}

void SystemSetting::on_sysCancel_clicked()
{
    this->close();
    emit maskWidgetClosed();
}
