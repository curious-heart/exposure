#include "fpdsetting.h"
#include "ui_fpdsetting.h"
#include "settingcfg.h"


FpdSetting::FpdSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FpdSetting)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(810, 480);
    ui->ok->setText(NULL);
    ui->cancel->setText(NULL);
    updateTriggerMode();
    FpdSettingCfg fpdSettingCfg=SettingCfg::getInstance().getFpdSettingCfg();
    //    QVariant v(0);
    //    ui->trigger->addItem("Outer",Enm_TriggerMode::Enm_TriggerMode_Outer);
    //    ui->trigger->addItem("Inner",Enm_TriggerMode::Enm_TriggerMode_Inner);
    //    ui->trigger->addItem("Software",Enm_TriggerMode::Enm_TriggerMode_Soft);
    //    ui->trigger->addItem("PREP",Enm_TriggerMode::Enm_TriggerMode_Prep);
    //    QVariant OuterV=ui->trigger->itemData(0,Qt::UserRole - 1);
    //    QVariant InnerV=ui->trigger->itemData(1,Qt::UserRole - 1);
    //    QVariant SoftwareV=ui->trigger->itemData(2,Qt::UserRole - 1);
    //    QVariant PREPV=ui->trigger->itemData(3,Qt::UserRole - 1);
    //    ui->trigger->setItemData(0,v, Qt::UserRole - 1);
    //    ui->trigger->setItemData(1,v, Qt::UserRole - 1);
    //    ui->trigger->setItemData(2,v, Qt::UserRole - 1);
    //    ui->trigger->setItemData(3,v, Qt::UserRole - 1);
    //    QVariant able(1);
    //    QMap<QString, QStringList> fpdTriggerModeMap=SettingCfg::getInstance().getFpdBaseCfg().fpdTriggerModeMap;
    //    QString fpdName=SettingCfg::getInstance().getSystemSettingCfg().fpdName;
    //    foreach(const QString &str,fpdTriggerModeMap.keys()){
    //        if(fpdName==str){
    //            QStringList list=fpdTriggerModeMap.value(str);
    //            ui->trigger->addItems(list);
    //            foreach(const QString triggerMode,list){
    //                if(triggerMode=="Outer"){
    //                    ui->trigger->setItemData(0,OuterV, Qt::UserRole - 1);
    //                }else if(triggerMode=="Outer"){
    //                    ui->trigger->setItemData(1,InnerV, Qt::UserRole - 1);
    //                }else if(triggerMode=="Software"){
    //                    ui->trigger->setItemData(2,SoftwareV, Qt::UserRole - 1);
    //                }else if(triggerMode=="PREP"){
    //                    ui->trigger->setItemData(3,PREPV, Qt::UserRole - 1);
    //                }
    //            }
    //        }
    //    }
    //    ui->trigger->setCurrentIndex(fpdSettingCfg.trigger);
    ui->PREPgroupBox->setEnabled(fpdSettingCfg.trigger==Enm_TriggerMode::Enm_TriggerMode_Prep?true:false);
    QIntValidator *intValidator = new QIntValidator;
    intValidator->setRange(0, 10000);
    ui->clearAcqParamDelayTime->setValidator(intValidator);
    ui->clearAcqParamDelayTime->setText(QString("%1").arg(fpdSettingCfg.PREP_clearAcqParamDelayTime));

    //隐藏不需要的控件
    ui->fpdLabel_4->hide();
    ui->fpdLabel_5->hide();
    ui->fpdLabel_6->hide();
    ui->groupBox_2->hide();
    ui->groupBox_3->hide();
    ui->groupBox_4->hide();


    if(fpdSettingCfg.offsetCorrectOption==Enm_CorrectOption::Enm_CorrectOp_SW_PreOffset){
        ui->SWPreOffset->setChecked(true);
    }else if(fpdSettingCfg.offsetCorrectOption==Enm_CorrectOption::Enm_CorrectOp_HW_PreOffset){
        ui->HWPreOffset->setChecked(true);
    }else if(fpdSettingCfg.offsetCorrectOption==Enm_CorrectOption::Enm_CorrectOp_SW_PostOffset){
        ui->SWPostOffset->setChecked(true);
    }else if(fpdSettingCfg.offsetCorrectOption==Enm_CorrectOp_HW_PostOffset){
        ui->HWPostOffset->setChecked(true);
    }
    if(fpdSettingCfg.gainCorrectOption==Enm_CorrectOption::Enm_CorrectOp_SW_Gain){
        ui->SWGain->setChecked(true);
    }else if(fpdSettingCfg.gainCorrectOption==Enm_CorrectOption::Enm_CorrectOp_HW_Gain){
        ui->HWGain->setChecked(true);
    }
    if(fpdSettingCfg.defectCorrectOption==Enm_CorrectOption::Enm_CorrectOp_SW_Defect){
        ui->SWDefect->setChecked(true);
    }else if(fpdSettingCfg.defectCorrectOption==Enm_CorrectOption::Enm_CorrectOp_HW_Defect){
        ui->HWDefect->setChecked(true);
    }
}

FpdSetting::~FpdSetting()
{
    delete ui;
}


/**
 * @brief FpdSetting::on_trigger_currentIndexChanged 改变触犯方式时
 * @param index
 */
void FpdSetting::on_trigger_currentIndexChanged(int index){
    if(index==Enm_TriggerMode::Enm_TriggerMode_Prep){
        ui->PREPgroupBox->setEnabled(true);
    }else{
        ui->PREPgroupBox->setEnabled(false);
    }

}


/**
 * @brief FpdSetting::on_ok_clicked 点击确定按钮时
 */
void FpdSetting::on_ok_clicked(){
    SystemSettingCfg ssc=SettingCfg::getInstance().getSystemSettingCfg();
    FpdSettingCfg fsc=SettingCfg::getInstance().getFpdSettingCfg();
    fsc.trigger=(ui->trigger->currentIndex());
    fsc.PREP_clearAcqParamDelayTime=(ui->clearAcqParamDelayTime->text().toInt());
    if(ui->SWPreOffset->isChecked()){
        fsc.offsetCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_PreOffset;
    }else if(ui->HWPreOffset->isChecked()){
        fsc.offsetCorrectOption=Enm_CorrectOption::Enm_CorrectOp_HW_PreOffset;
    }else if(ui->SWPostOffset->isChecked()){
        fsc.offsetCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_PostOffset;
    }else if(ui->HWPostOffset->isChecked()){
        fsc.offsetCorrectOption=Enm_CorrectOption::Enm_CorrectOp_HW_PostOffset;
    }
    if(ui->SWGain->isChecked()){
        fsc.gainCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_Gain;
    }else if(ui->HWGain->isChecked()){
        fsc.gainCorrectOption=Enm_CorrectOption::Enm_CorrectOp_HW_Gain;
    }
    if(ui->SWDefect->isChecked()){
        fsc.defectCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_Defect;
    }else if(ui->HWDefect->isChecked()){
        fsc.defectCorrectOption=Enm_CorrectOption::Enm_CorrectOp_HW_Defect;
    }
    SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
    this->close();
    emit maskWidgetClosed();
}


/**
 * @brief FpdSetting::on_cancel_clicked 点击取消按钮时
 */
void FpdSetting::on_cancel_clicked(){
    this->close();
    emit maskWidgetClosed();
}

/**
 * @brief FpdSetting::updateTriggerMode 更新触发方式下拉框内容
 */
void FpdSetting::updateTriggerMode()
{
    ui->trigger->clear();
    QString o="Outer",i="Inner",s="Software",p="PREP",f="FreeSync";
    ui->trigger->addItem(o,Enm_TriggerMode::Enm_TriggerMode_Outer);
    ui->trigger->addItem(i,Enm_TriggerMode::Enm_TriggerMode_Inner);
    ui->trigger->addItem(s,Enm_TriggerMode::Enm_TriggerMode_Soft);
    ui->trigger->addItem(p,Enm_TriggerMode::Enm_TriggerMode_Prep);
    ui->trigger->addItem(f,Enm_TriggerMode::Enm_TriggerMode_FreeSync);
    FpdSettingCfg fpdSettingCfg=SettingCfg::getInstance().getFpdSettingCfg();
    QMap<QString, QStringList> fpdTriggerModeMap=SettingCfg::getInstance().getFpdBaseCfg().fpdTriggerModeMap;
    QString fpdName=SettingCfg::getInstance().getSystemSettingCfg().fpdName;
    bool oflag=false,iflag=false,sflag=false,pflag=false,fflag=false;
    foreach(const QString &str,fpdTriggerModeMap.keys()){
        if(fpdName==str){
            QStringList list=fpdTriggerModeMap.value(str);
            //ui->trigger->addItems(list);
            foreach(const QString triggerMode,list){
                if(triggerMode==o){
                    oflag=true;
                }else if(triggerMode==i){
                    iflag=true;
                }else if(triggerMode==s){
                    sflag=true;
                }else if(triggerMode==p){
                    pflag=true;
                }else if(triggerMode==f){
                    fflag=true;
                }
            }
        }
    }
    QVariant v(0);
    if(!oflag){
        ui->trigger->setItemData(ui->trigger->findText(o),v, Qt::UserRole - 1);
    }
    if(!iflag){
        ui->trigger->setItemData(ui->trigger->findText(i),v, Qt::UserRole - 1);
    }
    if(!sflag){
        ui->trigger->setItemData(ui->trigger->findText(s),v, Qt::UserRole - 1);
    }
    if(!pflag){
        ui->trigger->setItemData(ui->trigger->findText(p),v, Qt::UserRole - 1);
    }
    if(!fflag){
        ui->trigger->setItemData(ui->trigger->findText(f),v, Qt::UserRole - 1);
    }
    ui->trigger->setCurrentIndex(fpdSettingCfg.trigger);
}
