#include "lowbatterywarning.h"
#include "ui_lowbatterywarning.h"
#include <QDebug>

LowBatteryWarning::LowBatteryWarning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LowBatteryWarning)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(590, 302);
    p=parent;
    ui->pushButton_notPrompt->setText(NULL);
    ui->pushButton_nowShutDown->setText(NULL);
    ui->pushButton_tenMinutesLater->setText(NULL);
    time=10;
    ui->lineEdit_shutDownTime->setText(QString("%1").arg(time));
    shutdownTimer=new QTimer(this);
    connect(shutdownTimer, &QTimer::timeout, this, &LowBatteryWarning::onShutdownTimerOutTime);
}

LowBatteryWarning::~LowBatteryWarning()
{
    if(shutdownTimer->isActive()){
        shutdownTimer->stop();
    }
    delete shutdownTimer;
    delete ui;
}

/**
 * @brief LowBatteryWarning::startShutdownTimer 启动定时器
 */
void LowBatteryWarning::startShutdownTimer()
{
    time=10;
    ui->lineEdit_shutDownTime->setText(QString("%1").arg(time));
    if(!shutdownTimer->isActive()){
        shutdownTimer->setInterval(1000);
        shutdownTimer->start();
    }
}

/**
 * @brief LowBatteryWarning::on_pushButton_nowShutDown_clicked 立即关机
 */
void LowBatteryWarning::on_pushButton_nowShutDown_clicked()
{
    //TODO
    //qDebug()<<"已经关机";
    system("shutdown -s -t 00");
    this->close();
    p->close();
}

/**
 * @brief LowBatteryWarning::on_pushButton_tenMinutesLater_clicked  十分钟后提示
 */
void LowBatteryWarning::on_pushButton_tenMinutesLater_clicked()
{
    if(shutdownTimer->isActive()){
        shutdownTimer->stop();
    }
    this->close();
    emit maskWidgetClosed();
    emit tenMinutesTimerStarted();
}

/**
 * @brief LowBatteryWarning::on_pushButton_notPrompt_clicked 不再提示
 */
void LowBatteryWarning::on_pushButton_notPrompt_clicked()
{
    if(shutdownTimer->isActive()){
        shutdownTimer->stop();
    }
    this->close();
    emit maskWidgetClosed();
}

/**
 * @brief LowBatteryWarning::onShutdownTimerOutTime 一秒更新一次倒计时数据
 */
void LowBatteryWarning::onShutdownTimerOutTime()
{
    time--;
    if(time>0){
        ui->lineEdit_shutDownTime->setText(QString("%1").arg(time));
    }else{
        on_pushButton_nowShutDown_clicked();
    }
}
