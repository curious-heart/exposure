#ifndef LOWBATTERYWARNING_H
#define LOWBATTERYWARNING_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class LowBatteryWarning;
}

class LowBatteryWarning : public QDialog
{
    Q_OBJECT

public:
    explicit LowBatteryWarning(QWidget *parent = nullptr);
    ~LowBatteryWarning();
    void startShutdownTimer();                          //启动定时器

private slots:
    void on_pushButton_nowShutDown_clicked();           //立即关机
    void on_pushButton_tenMinutesLater_clicked();       //十分钟后提示
    void on_pushButton_notPrompt_clicked();             //不再提示
    void onShutdownTimerOutTime();                      //一秒更新一次倒计时数据

signals:
    void maskWidgetClosed();        //关闭maskWidget层
    void tenMinutesTimerStarted();  //启动十分钟倒计时定时器

private:
    Ui::LowBatteryWarning *ui;
    QWidget *p;
    QTimer * shutdownTimer;     //定时器 1秒执行一次
    int time;                   //倒计时秒数
};

#endif // LOWBATTERYWARNING_H
