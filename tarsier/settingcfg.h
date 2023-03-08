#ifndef SETTINGCFG_H
#define SETTINGCFG_H

#include <QObject>
#include <QMap>
#include <QSerialPort>
#include <QtXml/QDomDocument>
#include <include/IRayEnumDef.h>

extern const char* INI_KEY_RTUSERIAL_TUBE_VOL;
extern const char* INI_KEY_RTUSERIAL_TUBE_AMT;
#define MIN_TUBE_VOL 55 //kv
#define MAX_TUBE_VOL 80
#define DEF_TUBE_VOL 70
#define MIN_TUBE_AMT 500 //mA
#define MAX_TUBE_AMT 6600
#define DEF_TUBE_AMT 3000

#define MAX_EXPOSURE_DURA_STEP 17 //idx. refer to exposureTimeList in MainWindow.cpp.
#define DEF_EXPOSURE_DURA_IDX 3
#define DEF_CURR_EXPOSURE_OPT 1

typedef enum
{
    exposure_opt_type_auto = 0,
    exposure_opt_type_manual = 1,
}exposure_opt_type_t;
typedef struct
{
    exposure_opt_type_t type;
    QString title;
    int idx;
    int vol, amt, dura;
}exposure_opt_item_t;
typedef QMap<int, exposure_opt_item_t*> exposure_opts_t;

struct SystemSettingCfg{
    QString serialPortName = "COM1";  //串口名称
    int serialParity=QSerialPort::NoParity;
    int serialBaudRate=QSerialPort::Baud9600;
    int serialDataBits=QSerialPort::Data8;
    int serialStopBits=QSerialPort::OneStop;
    int timeout=1000;
    int numberOfRetries=3;
    int serverAddress=1;//1-255
    int exposureTimeIndex=DEF_EXPOSURE_DURA_IDX /*25*/;
    int tubeVol = DEF_TUBE_VOL;
    int tubeAmt = DEF_TUBE_AMT;
    int isAutoOff=0;
    QString fpdName="";
    QString fpdWorkDir="";
    QString sleepTime="off";
    QString shutdownTime="off";
    QString imageDir="";
    int currExposureOpt = DEF_CURR_EXPOSURE_OPT;
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
    ~SettingCfg();
    void readSettingConfig();
    void writeSettingConfig(SystemSettingCfg * ssc,FpdSettingCfg * fsc);
    SystemSettingCfg &getSystemSettingCfg();
    FpdSettingCfg &getFpdSettingCfg();
    void readBaseConfig();
    SystemBaseCfg& getSystemBaseCfg();
    FpdBaseCfg &getFpdBaseCfg();
    exposure_opts_t& getExposureOptsCfg();
private:
    SystemSettingCfg systemSettingCfg;
    FpdSettingCfg fpdSettingCfg;
    SystemBaseCfg systemBaseCfg;
    FpdBaseCfg fpdBaseCfg;

    exposure_opts_t exposureOptsCfg;
    void read_exposure_opts_cfg(QDomDocument &doc);
    void construct_default_exposure_opts();
    void init_exposure_opt_item(exposure_opt_item_t* opt_item);
    void clear_exposure_opts_cfg();
    bool check_exposure_opt_value(exposure_opt_item_t* opt_item);
};

#endif // SETTINGCFG_H
