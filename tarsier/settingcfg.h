#ifndef SETTINGCFG_H
#define SETTINGCFG_H

#include <QObject>
#include <QMap>
#include <QSerialPort>
#include <include/IRayEnumDef.h>

#define DEFAULT_TUBE_VOL 70 //kv
#define DEFAULT_TUBE_AMT 3000 //uA
#define DEFAULT_EXPOSURE_TIME_IDX 10

struct SystemSettingCfg{
    QString serialPortName = "COM1";  //串口名称
    int serialParity=QSerialPort::NoParity;
    int serialBaudRate=QSerialPort::Baud9600;
    int serialDataBits=QSerialPort::Data8;
    int serialStopBits=QSerialPort::OneStop;
    int timeout=1000;
    int numberOfRetries=3;
    int serverAddress=1;//1-255
    int exposureTimeIndex=DEFAULT_EXPOSURE_TIME_IDX /*25*/;
    int tubeVol = DEFAULT_TUBE_VOL;
    int tubeAmt = DEFAULT_TUBE_AMT;
    int isAutoOff=0;
    QString fpdName="";
    QString fpdWorkDir="";
    QString sleepTime="off";
    QString shutdownTime="off";
    QString imageDir="";
};
struct SystemBaseCfg{
    QStringList fpdNameList;//探测器列表
    QMap<QString, QString> fpdworkdirMap;//探测器与工作目录的对应关系
};
struct FpdSettingCfg{
    int trigger=0;//默认Software触发模式
    int PREP_clearAcqParamDelayTime=3000;//清空和采图之间的时间间隔 默认3000ms
    int offsetCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_PreOffset;
    int gainCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_Gain;
    int defectCorrectOption=Enm_CorrectOption::Enm_CorrectOp_SW_Defect;

    int softwareTriggerWaitTimeBeforeAcqImg = 8950; //ms. software触发模式下，曝光之后、数据采集之前需要等待的时间。
    int innerTriggerWaitTimeBeforeAcqImg = 7650; //ms. inner触发模式下，曝光之后、数据采集之前需要等待的时间。
};
struct FpdBaseCfg{
    QMap<QString, QStringList> fpdTriggerModeMap;//探测器与触发模式列表的对应关系
};

class SettingCfg : public QObject
{
    Q_OBJECT
public:
    static SettingCfg & getInstance();
    explicit SettingCfg(QObject *parent = nullptr);
    void readSettingConfig();
    void writeSettingConfig(SystemSettingCfg * ssc,FpdSettingCfg * fsc);
    SystemSettingCfg &getSystemSettingCfg();
    FpdSettingCfg &getFpdSettingCfg();
    void readBaseConfig();
    SystemBaseCfg getSystemBaseCfg();
    FpdBaseCfg getFpdBaseCfg();
private:
    SystemSettingCfg systemSettingCfg;
    FpdSettingCfg fpdSettingCfg;
    SystemBaseCfg systemBaseCfg;
    FpdBaseCfg fpdBaseCfg;
};

#endif // SETTINGCFG_H
