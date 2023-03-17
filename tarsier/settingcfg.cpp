#include "settingcfg.h"
#include "logger.h"
#include <QSettings>
#include <QFile>
#include <iostream>

const char* INI_GRP_USER_SETTINGS = "UserSettings";
const char* INI_KEY_CURR_EXPOSURE_OPT = "currExposureOpt";
const char* INI_KEY_RTUSERIAL_TUBE_VOL = "tubeVol";
const char* INI_KEY_RTUSERIAL_TUBE_AMT = "tubeAmt";

const char* TAG_STR_EXPOSURE_OPTS = "exposure_opts";
const char* TAG_STR_OPT = "opt";
const char* TAG_STR_TYPE = "type";
const char* TAG_STR_TITLE = "title";
const char* TAG_STR_TUBE_VOL = INI_KEY_RTUSERIAL_TUBE_VOL;
const char* TAG_STR_TUBE_AMT = INI_KEY_RTUSERIAL_TUBE_AMT;
const char* TAG_STR_DURATION = "dura";
const char* TAG_STR_IDX = "idx";

typedef struct
{
    QString key_or_tag;
    void* ptr;
}read_cfg_helper_t;

void SettingCfg::clear_exposure_opts_cfg()
{
    exposure_opts_t::iterator it = exposureOptsCfg.begin();
    while(it != exposureOptsCfg.end())
    {
        delete it.value();
        ++it;
    }
    exposureOptsCfg.clear();
}
bool SettingCfg::check_exposure_opt_value(exposure_opt_item_t* opt_item)
{
    if(!opt_item)
    {
        return false;
    }

    if(opt_item->idx < 0)
    {
       DIY_LOG(LOG_ERROR, "Invalid exposure_opts %s: %d. It should be int and >= 0.",
               TAG_STR_IDX, opt_item->idx);
       return false;
    }
    if(opt_item->type == exposure_opt_type_manual)
    {
        return true;
    }

    if((opt_item->type != exposure_opt_type_auto)
            && (opt_item->type != exposure_opt_type_manual))
    {
        DIY_LOG(LOG_ERROR, "Invalid exposure_opts %s: %d. It should be int of %d or %d.",
                TAG_STR_TYPE, opt_item->type, exposure_opt_type_auto, exposure_opt_type_manual);
        return false;
    }
    if((opt_item->vol > MAX_TUBE_VOL) || (opt_item->vol < MIN_TUBE_VOL))
    {
       DIY_LOG(LOG_ERROR, "Invalid exposure_opts %s: %d. It should be int between [%d, %d].",
               TAG_STR_TUBE_VOL, opt_item->vol, MIN_TUBE_VOL, MAX_TUBE_VOL);
       return false;
    }
    if((opt_item->amt > MAX_TUBE_AMT) || (opt_item->amt < MIN_TUBE_AMT))
    {
       DIY_LOG(LOG_ERROR, "Invalid exposure_opts %s: %d. It should be int between [%d, %d].",
               TAG_STR_TUBE_AMT, opt_item->amt, MIN_TUBE_AMT, MAX_TUBE_AMT);
       return false;
    }
    if((opt_item->dura >= MAX_EXPOSURE_DURA_STEP) || (opt_item->dura < 0))
    {
       DIY_LOG(LOG_ERROR, "Invalid exposure_opts %s: %d. It should be int between [%d, %d].",
               TAG_STR_DURATION, opt_item->dura, 0, MAX_EXPOSURE_DURA_STEP-1);
       return false;
    }

    return true;
}
void SettingCfg::init_exposure_opt_item(exposure_opt_item_t* opt_item)
{
    if(opt_item)
    {
        opt_item->idx = -1;
        opt_item->title = "";
        opt_item->type = (exposure_opt_type_t)-1;
        opt_item->vol = MIN_TUBE_VOL - 1;
        opt_item->amt = MIN_TUBE_AMT - 1;
        opt_item->dura = -1;
    }
}

/**
 * @brief SettingCfg::getInstance 获取单例的SettingCfg
 * @return 单例的SettingCfg实例
 */
SettingCfg &SettingCfg::getInstance(){
    static SettingCfg instance;
    return instance;
}


/**
 * @brief SettingCfg::SettingCfg 构造函数
 * @param parent
 */
SettingCfg::SettingCfg(QObject *parent): QObject(parent){
    readSettingConfig();
    readBaseConfig();
}

SettingCfg::~SettingCfg()
{
    clear_exposure_opts_cfg();
}

/**
 * @brief SettingCfg::readSettingConfig 读取data/settingCfg.ini文件中的数据
 */
void SettingCfg::readSettingConfig(){
    QSettings settings("data/settingCfg.ini", QSettings::IniFormat);
    settings.beginGroup("System");
    systemSettingCfg.sleepTime = settings.value("sleepTime").isNull()?systemSettingCfg.sleepTime:settings.value("sleepTime").toString();
    systemSettingCfg.shutdownTime = settings.value("shutdownTime").isNull()?systemSettingCfg.shutdownTime:settings.value("shutdownTime").toString();
    systemSettingCfg.imageDir = settings.value("imageDir").isNull()?systemSettingCfg.imageDir:settings.value("imageDir").toString();
    settings.endGroup();
    settings.beginGroup("RtuSerial");
    systemSettingCfg.serialPortName = settings.value("serialPortName").isNull()?systemSettingCfg.serialPortName:settings.value("serialPortName").toString();
    systemSettingCfg.serialParity = settings.value("serialParity").isNull()?systemSettingCfg.serialParity:settings.value("serialParity").toInt();
    systemSettingCfg.serialBaudRate = settings.value("serialBaudRate").isNull()?systemSettingCfg.serialBaudRate:settings.value("serialBaudRate").toInt();
    systemSettingCfg.serialDataBits = settings.value("serialDataBits").isNull()?systemSettingCfg.serialDataBits:settings.value("serialDataBits").toInt();
    systemSettingCfg.serialStopBits = settings.value("serialStopBits").isNull()?systemSettingCfg.serialStopBits:settings.value("serialStopBits").toInt();
    systemSettingCfg.timeout = settings.value("timeout").isNull()?systemSettingCfg.timeout:settings.value("timeout").toInt();
    systemSettingCfg.numberOfRetries = settings.value("numberOfRetries").isNull()?systemSettingCfg.numberOfRetries:settings.value("numberOfRetries").toInt();
    systemSettingCfg.serverAddress = settings.value("serverAddress").isNull()?systemSettingCfg.serverAddress:settings.value("serverAddress").toInt();
    systemSettingCfg.exposureTimeIndex= settings.value("exposureTimeIndex").isNull()?systemSettingCfg.exposureTimeIndex:settings.value("exposureTimeIndex").toInt();
    systemSettingCfg.tubeVol = settings.value(INI_KEY_RTUSERIAL_TUBE_VOL, DEF_TUBE_VOL).toInt();
    systemSettingCfg.tubeAmt = settings.value(INI_KEY_RTUSERIAL_TUBE_AMT , DEF_TUBE_AMT).toInt();
    systemSettingCfg.isAutoOff = settings.value("isAutoOff").isNull()?systemSettingCfg.isAutoOff:settings.value("isAutoOff").toInt();
    settings.endGroup();

    settings.beginGroup("Fpd");
    systemSettingCfg.fpdName=settings.value("fpdName").isNull()?systemSettingCfg.fpdName:settings.value("fpdName").toString();
    systemSettingCfg.fpdWorkDir=settings.value("fpdWorkDir").isNull()?systemSettingCfg.fpdWorkDir:settings.value("fpdWorkDir").toString();
    fpdSettingCfg.trigger=settings.value("trigger").isNull()?fpdSettingCfg.trigger:settings.value("trigger").toInt();
    fpdSettingCfg.PREP_clearAcqParamDelayTime=settings.value("PREP_clearAcqParamDelayTime").isNull()?fpdSettingCfg.PREP_clearAcqParamDelayTime:settings.value("PREP_clearAcqParamDelayTime").toInt();
    fpdSettingCfg.offsetCorrectOption=settings.value("offsetCorrectOption").isNull()?fpdSettingCfg.offsetCorrectOption:settings.value("offsetCorrectOption").toInt();
    fpdSettingCfg.gainCorrectOption=settings.value("gainCorrectOption").isNull()?fpdSettingCfg.gainCorrectOption:settings.value("gainCorrectOption").toInt();
    fpdSettingCfg.defectCorrectOption=settings.value("defectCorrectOption").isNull()?fpdSettingCfg.defectCorrectOption:settings.value("defectCorrectOption").toInt();
    fpdSettingCfg.softwareTriggerWaitTimeBeforeAcqImg
            = settings.value("softwareTriggerWaitBeforeAcqImg").isNull()?fpdSettingCfg.softwareTriggerWaitTimeBeforeAcqImg:settings.value("softwareTriggerWaitBeforeAcqImg").toInt();
    fpdSettingCfg.innerTriggerWaitTimeBeforeAcqImg
            = settings.value("innerTriggerWaitBeforeAcqImg").isNull()?fpdSettingCfg.innerTriggerWaitTimeBeforeAcqImg:settings.value("innerTriggerWaitBeforeAcqImg").toInt();
    settings.endGroup();

    settings.beginGroup(INI_GRP_USER_SETTINGS);
    systemSettingCfg.currExposureOpt = settings.value(INI_KEY_CURR_EXPOSURE_OPT, DEF_CURR_EXPOSURE_OPT).toInt();
    settings.endGroup();
}


/**
 * @brief SettingCfg::writeSettingConfig 写入data/settingCfg.ini文件中的数据
 * @param ssc 系统设置
 * @param fsc 探测器设置
 */
void SettingCfg::writeSettingConfig(SystemSettingCfg * ssc, FpdSettingCfg * fsc){
    /*
    systemSettingCfg.serialPortName= ssc->serialPortName;
    systemSettingCfg.fpdName=ssc->fpdName;
    systemSettingCfg.serialParity=ssc->serialParity;
    systemSettingCfg.serialBaudRate=ssc->serialBaudRate;
    systemSettingCfg.serialDataBits=ssc->serialDataBits;
    systemSettingCfg.serialStopBits=ssc->serialStopBits;
    systemSettingCfg.timeout=ssc->timeout;
    systemSettingCfg.numberOfRetries=ssc->numberOfRetries;
    systemSettingCfg.serverAddress=ssc->serverAddress;
    systemSettingCfg.fpdWorkDir=ssc->fpdWorkDir;
    systemSettingCfg.sleepTime=ssc->sleepTime;
    systemSettingCfg.shutdownTime=ssc->shutdownTime;
    systemSettingCfg.exposureTimeIndex=ssc->exposureTimeIndex;
    systemSettingCfg.imageDir=ssc->imageDir;
    fpdSettingCfg.trigger=fsc->trigger;
    fpdSettingCfg.PREP_clearAcqParamDelayTime=fsc->PREP_clearAcqParamDelayTime;
    fpdSettingCfg.offsetCorrectOption=fsc->offsetCorrectOption;
    fpdSettingCfg.gainCorrectOption=fsc->gainCorrectOption;
    fpdSettingCfg.defectCorrectOption=fsc->defectCorrectOption;
    */
    if(ssc)
    {
        DIY_LOG(LOG_INFO, "writeSettingConfig: ssc");
    }
    if(fsc)
    {
        DIY_LOG(LOG_INFO, "writeSettingConfig: fsc");
    }

    QSettings settings("data/settingCfg.ini", QSettings::IniFormat);
    if(ssc)
    {
        settings.beginGroup("System");
        settings.setValue("sleepTime", ssc->sleepTime);
        settings.setValue("shutdownTime", ssc->shutdownTime);
        settings.setValue("imageDir", ssc->imageDir);
        settings.endGroup();

        settings.beginGroup("RtuSerial");
        settings.setValue("serialPortName", ssc->serialPortName);
        settings.setValue("serialParity", ssc->serialParity);
        settings.setValue("serialBaudRate", ssc->serialBaudRate);
        settings.setValue("serialDataBits", ssc->serialDataBits);
        settings.setValue("serialStopBits", ssc->serialStopBits);
        settings.setValue("timeout", ssc->timeout);
        settings.setValue("numberOfRetries", ssc->numberOfRetries);
        settings.setValue("serverAddress", ssc->serverAddress);
        settings.setValue("exposureTimeIndex", ssc->exposureTimeIndex);
        settings.setValue(INI_KEY_RTUSERIAL_TUBE_VOL, ssc->tubeVol);
        settings.setValue(INI_KEY_RTUSERIAL_TUBE_AMT, ssc->tubeAmt);
        settings.endGroup();


        settings.beginGroup(INI_GRP_USER_SETTINGS);
        settings.setValue(INI_KEY_CURR_EXPOSURE_OPT, ssc->currExposureOpt);
        settings.endGroup();
    }
    settings.beginGroup("Fpd");
    if(ssc)
    {
        settings.setValue("fpdName", ssc->fpdName);
        settings.setValue("fpdWorkDir", ssc->fpdWorkDir);
    }
    if(fsc)
    {
        settings.setValue("trigger", fsc->trigger);
        settings.setValue("PREP_clearAcqParamDelayTime", fsc->PREP_clearAcqParamDelayTime);
        settings.setValue("offsetCorrectOption", fsc->offsetCorrectOption);
        settings.setValue("gainCorrectOption", fsc->gainCorrectOption);
        settings.setValue("defectCorrectOption", fsc->defectCorrectOption);
    }
    settings.endGroup();

}


/**
 * @brief SettingCfg::getSystemSettingCfg 获得系统设置的配置信息
 * @return
 */
SystemSettingCfg &SettingCfg::getSystemSettingCfg(){
    return systemSettingCfg;
}


/**
 * @brief SettingCfg::getFpdSettingCfg 获取探测器设置的配置信息
 * @return
 */
FpdSettingCfg &SettingCfg::getFpdSettingCfg(){
    return fpdSettingCfg;
}

static const char* DEF_EXPOSURE_OPTS_0_TITLE = "高";
static const int DEF_EXPOSURE_OPTS_0_VOL = DEF_HIGH_TUBE_VOL;
static const int DEF_EXPOSURE_OPTS_0_AMT = DEF_HIGH_TUBE_AMT;
static const int DEF_EXPOSURE_OPTS_0_DURA = DEF_HIGH_EXPOSURE_DURA_STEP;

static const char* DEF_EXPOSURE_OPTS_1_TITLE = "中";
static const int DEF_EXPOSURE_OPTS_1_VOL = DEF_MID_TUBE_VOL;
static const int DEF_EXPOSURE_OPTS_1_AMT = DEF_MID_TUBE_AMT;
static const int DEF_EXPOSURE_OPTS_1_DURA = DEF_MID_EXPOSURE_DURA_STEP;

static const char* DEF_EXPOSURE_OPTS_2_TITLE = "低";
static const int DEF_EXPOSURE_OPTS_2_VOL = DEF_LOW_TUBE_VOL; //be caful to adjust it according to MAX/MIN_TUBE_VOL
static const int DEF_EXPOSURE_OPTS_2_AMT = DEF_LOW_TUBE_AMT;; //be caful to adjust it according to MAX/MIN_TUBE_AMT
static const int DEF_EXPOSURE_OPTS_2_DURA = DEF_LOW_EXPOSURE_DURA_STEP; //be caful to adjust it according to MAX_EXPOSURE_DURA_STEP

static const char* DEF_EXPOSURE_OPTS_3_TITLE = "手动设置";

void SettingCfg::construct_default_exposure_opts()
{
    static exposure_opt_item_t def_exposure_opts[] =
    {
        {exposure_opt_type_auto, DEF_EXPOSURE_OPTS_0_TITLE, 0,
        DEF_EXPOSURE_OPTS_0_VOL, DEF_EXPOSURE_OPTS_0_AMT, DEF_EXPOSURE_OPTS_0_DURA},

        {exposure_opt_type_auto, DEF_EXPOSURE_OPTS_1_TITLE, 1,
        DEF_EXPOSURE_OPTS_1_VOL, DEF_EXPOSURE_OPTS_1_AMT, DEF_EXPOSURE_OPTS_1_DURA},

        {exposure_opt_type_auto, DEF_EXPOSURE_OPTS_2_TITLE, 2,
        DEF_EXPOSURE_OPTS_2_VOL, DEF_EXPOSURE_OPTS_2_AMT, DEF_EXPOSURE_OPTS_2_DURA},

        {exposure_opt_type_manual, DEF_EXPOSURE_OPTS_3_TITLE, 3,
        DEF_EXPOSURE_OPTS_0_VOL, DEF_EXPOSURE_OPTS_0_AMT, DEF_EXPOSURE_OPTS_0_DURA},
    };

    for(qulonglong idx = 0; idx < sizeof(def_exposure_opts)/sizeof(def_exposure_opts[0]); ++idx)
    {
        exposureOptsCfg.insert(def_exposure_opts[idx].idx,
                               &def_exposure_opts[idx]);
    }
}

/**
 * @brief SettingCfg::readBaseConfig 读取data/base.xml文件中的数据
 */
void SettingCfg::readBaseConfig(){
    QDomDocument doc;
    QFile file("data/base.xml");
    if (!file.open(QIODevice::ReadOnly))
         return;
    if(!doc.setContent(&file)){
         file.close();
         return;
    }
    file.close();
    QDomElement root = doc.documentElement();//读取根节点
    QDomNodeList fpdsList=doc.elementsByTagName("fpds");
    if(!fpdsList.isEmpty()){
        systemBaseCfg.fpdworkdirMap.clear();
        QDomElement fpds=fpdsList.item(0).toElement();
        QDomNodeList fpdList=fpds.elementsByTagName("fpd");
        if(!fpdList.isEmpty()){
            for(int i=0,count=fpdList.count();i<count;i++){
                QDomNode fpd=fpdList.item(i);
                QString key=fpd.namedItem("fpdname").toElement().text();
                QString value=fpd.namedItem("fpdworkdir").toElement().text();
                systemBaseCfg.fpdworkdirMap.insert(key,value);
                QString triggerMode=fpd.namedItem("fpdtriggermode").toElement().text();
                fpdBaseCfg.fpdTriggerModeMap.insert(key,triggerMode.split("/"));

                //printf("%s\n",key.toStdString().data());
                //printf("%s\n",value.toStdString().data());
            }
        }
    }

    read_exposure_opts_cfg(doc);
}

void SettingCfg::read_exposure_opts_cfg(QDomDocument &doc)
{
#define EXPOSURE_OPT_PTR_TYPE(e) (&((e).type))
#define EXPOSURE_OPT_PTR_TITLE(e) (&((e).title))
#define EXPOSURE_OPT_PTR_IDX(e) (&((e).idx))
#define EXPOSURE_OPT_PTR_VOL(e) (&((e).vol))
#define EXPOSURE_OPT_PTR_AMT(e) (&((e).amt))
#define EXPOSURE_OPT_PTR_DURA(e) (&((e).dura))

    clear_exposure_opts_cfg();
    QDomNodeList exposure_opts = doc.elementsByTagName(TAG_STR_EXPOSURE_OPTS);
    if(!exposure_opts.isEmpty())
    {
        QDomElement opts = exposure_opts.item(0).toElement();
        QDomNodeList opt_list = opts.elementsByTagName(TAG_STR_OPT);
        if(!opt_list.isEmpty())
        {
            for(int opt_idx = 0, count = opt_list.count(); opt_idx < count; ++opt_idx)
            {
                exposure_opt_item_t* opt_item;
                opt_item = new exposure_opt_item_t;
                if(!opt_item)
                {
                    DIY_LOG(LOG_ERROR, "new exposure_opt_item_t error!!!!!");
                    clear_exposure_opts_cfg();
                    construct_default_exposure_opts();
                    break;
                }
                init_exposure_opt_item(opt_item);
                read_cfg_helper_t helper[] =
                {
                    {TAG_STR_IDX, EXPOSURE_OPT_PTR_IDX(*opt_item)},
                    {TAG_STR_TYPE, EXPOSURE_OPT_PTR_TYPE(*opt_item)},
                    {TAG_STR_TITLE, EXPOSURE_OPT_PTR_TITLE(*opt_item)},
                    {TAG_STR_TUBE_VOL, EXPOSURE_OPT_PTR_VOL(*opt_item)},
                    {TAG_STR_TUBE_AMT, EXPOSURE_OPT_PTR_AMT(*opt_item)},
                    {TAG_STR_DURATION, EXPOSURE_OPT_PTR_DURA(*opt_item)},
                };

                QDomNode opt, dom_item;
                opt = opt_list.item(opt_idx);
                for(qulonglong h_idx = 0; h_idx < sizeof(helper)/sizeof(helper[0]); ++h_idx)
                {
                    dom_item = opt.namedItem(helper[h_idx].key_or_tag);
                    if(!dom_item.isNull())
                    {
                        if(TAG_STR_TITLE == helper[h_idx].key_or_tag)
                        {
                            *((QString*)(helper[h_idx].ptr)) = dom_item.toElement().text();
                        }
                        else if(TAG_STR_TYPE == helper[h_idx].key_or_tag)
                        {
                            *((exposure_opt_type_t*)(helper[h_idx].ptr))
                                    = (exposure_opt_type_t)(dom_item.toElement().text().toInt());
                        }
                        else
                        {
                            *((int*)(helper[h_idx].ptr))
                                    = dom_item.toElement().text().toInt();
                        }
                    }
                    else
                    {
                        DIY_LOG(LOG_WARN,
                                "Lack opt item %ls in config file.",
                                helper[h_idx].key_or_tag.utf16());
                    }
                }
                if(check_exposure_opt_value(opt_item))
                {
                    exposureOptsCfg.insert(opt_item->idx, opt_item);
                }
                else
                {
                    DIY_LOG(LOG_WARN, "Invalid opt item. Ignored.");
                    delete opt_item;
                }
            }
            if(0 == exposureOptsCfg.count())
            {
                DIY_LOG(LOG_WARN, "No valid opt items in config file, use default opts.");
                construct_default_exposure_opts();
            }
        }
        else
        {
            //default
            DIY_LOG(LOG_WARN, "Tag \"%s\" is not found in cfg file. Use default config.",
                    TAG_STR_OPT);
            construct_default_exposure_opts();
        }
    }
    else
    {
        //default
        DIY_LOG(LOG_WARN, "Tag \"%s\" is not found in cfg file. Use default config.",
                TAG_STR_EXPOSURE_OPTS);
        construct_default_exposure_opts();
    }
#undef EXPOSURE_OPT_PTR_TYPE
#undef EXPOSURE_OPT_PTR_TITLE
#undef EXPOSURE_OPT_PTR_IDX
#undef EXPOSURE_OPT_PTR_VOL
#undef EXPOSURE_OPT_PTR_AMT
#undef EXPOSURE_OPT_PTR_DURA
}

exposure_opts_t& SettingCfg::getExposureOptsCfg()
{
    return exposureOptsCfg;
}

/**
 * @brief SettingCfg::getSystemBaseCfg 获得系统设置中的基础信息
 * @return
 */
SystemBaseCfg& SettingCfg::getSystemBaseCfg(){
    return systemBaseCfg;
}

/**
 * @brief SettingCfg::getFpdBaseCfg 获得探测器设置中的基础数据
 * @return
 */
FpdBaseCfg& SettingCfg::getFpdBaseCfg(){
    return fpdBaseCfg;
}
