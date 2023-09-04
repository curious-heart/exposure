#ifndef SETTINGCFG_H
#define SETTINGCFG_H

#include "common_tool_func.h"

#include <QObject>
#include <QMap>
#include <QSerialPort>
#include <QtXml/QDomDocument>
#include <include/IRayEnumDef.h>

#define MIN_TUBE_VOL 55 //kv
#define MAX_TUBE_VOL 90
#define DEF_TUBE_VOL 70
#define MIN_TUBE_AMT 500 //mA
#define MAX_TUBE_AMT 8000
#define DEF_TUBE_AMT 3000

#define MAX_EXPOSURE_DURA_STEP 17 //idx. refer to exposureTimeList in MainWindow.cpp.
#define DEF_EXPOSURE_DURA_IDX 3
#define DEF_CURR_EXPOSURE_OPT 1

extern const QString gs_hvCtrlIntfName_NIC;
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
    QString hvCtrlIntfName = "COM1";  //串口名称
    int serialParity=QSerialPort::NoParity;
    int serialBaudRate=QSerialPort::Baud9600;
    int serialDataBits=QSerialPort::Data8;
    int serialStopBits=QSerialPort::OneStop;
    QString mb_tcp_ip_addr = "10.10.18.1";
    int mb_tcp_port = 502;

    int timeout=1000;
    int numberOfRetries=3;
    int serverAddress=1;//1-255
    int exposureTimeIndex=DEF_EXPOSURE_DURA_IDX /*25*/;
    int tubeVol = DEF_TUBE_VOL;
    int tubeAmt = DEF_TUBE_AMT;
    int isAutoOff=0;
    QString fpdName=""; //This var contains FPD_NAME_NONE_INTERNAL_STR but not FPD_NAME_NONE_ZH_STR.
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

    ip_intf_type_t fpd_conn_media = IP_INTF_WIFI;
};
struct FpdBaseCfg{
    QMap<QString, QStringList> fpdTriggerModeMap;//探测器与触发模式列表的对应关系
    QMap<QString, QString> fpdHostIp;
};

/* Just use the existed FpdSettingCfg,although for some fpd, not all info are valid.
 * currently, we care "trigger" only.
 *
 * key: INI_GRP_FPD_HIS + "/" + fpdname
 * FPD_NAME_NONE_ZH_STR is not used here. we use FPD_NAME_NONE_INTERNAL_STR.
*/
typedef QMap<QString, struct FpdSettingCfg*> fpd_settings_his_t;

class SettingCfg : public QObject
{
    Q_OBJECT
public:
    static SettingCfg & getInstance();
    explicit SettingCfg(QObject *parent = nullptr);
    ~SettingCfg();
    void readSettingConfig();
    void writeSettingConfig(SystemSettingCfg * ssc,FpdSettingCfg * fsc);
    void update_fpd_setting_his();
    struct FpdSettingCfg* get_fpd_his(QString & name);
    QString fpd_name_internal_to_ui(const QString &name);
    QString fpd_name_ui_to_internal(const QString &name);
    /*
     * 20230307：
     * 注意：这几个函数的返回类型改为了reference，这样可以避免多次拷贝；但在使用时就必须注意，
     * 如果对返回的结构体中某项进行了赋值，一定要writeSettingConfig，以避免本地数据与配置文件数据
     * 不一致。
    */
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
    fpd_settings_his_t m_fpd_settings_his;

    exposure_opts_t exposureOptsCfg;
    void read_exposure_opts_cfg(QDomDocument &doc);
    void construct_default_exposure_opts();
    void init_exposure_opt_item(exposure_opt_item_t* opt_item);
    void clear_exposure_opts_cfg();
    bool check_exposure_opt_value(exposure_opt_item_t* opt_item);

    void clear_fpd_settings_his();
};

#endif // SETTINGCFG_H
