#include "settingcfg.h"
#include <QSettings>
#include <QFile>
#include <QtXml/QDomDocument>
#include <iostream>

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
}


/**
 * @brief SettingCfg::writeSettingConfig 写入data/settingCfg.ini文件中的数据
 * @param ssc 系统设置
 * @param fsc 探测器设置
 */
void SettingCfg::writeSettingConfig(SystemSettingCfg * ssc, FpdSettingCfg * fsc){
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
    QSettings settings("data/settingCfg.ini", QSettings::IniFormat);
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

    settings.endGroup();
    settings.beginGroup("Fpd");
    settings.setValue("fpdName", ssc->fpdName);
    settings.setValue("fpdWorkDir", ssc->fpdWorkDir);
    settings.setValue("trigger", fsc->trigger);
    settings.setValue("PREP_clearAcqParamDelayTime", fsc->PREP_clearAcqParamDelayTime);
    settings.setValue("offsetCorrectOption", fsc->offsetCorrectOption);
    settings.setValue("gainCorrectOption", fsc->gainCorrectOption);
    settings.setValue("defectCorrectOption", fsc->defectCorrectOption);
    settings.endGroup();

}


/**
 * @brief SettingCfg::getSystemSettingCfg 获得系统设置的配置信息
 * @return
 */
SystemSettingCfg SettingCfg::getSystemSettingCfg(){
    return systemSettingCfg;
}


/**
 * @brief SettingCfg::getFpdSettingCfg 获取探测器设置的配置信息
 * @return
 */
FpdSettingCfg SettingCfg::getFpdSettingCfg(){
    return fpdSettingCfg;
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
}


/**
 * @brief SettingCfg::getSystemBaseCfg 获得系统设置中的基础信息
 * @return
 */
SystemBaseCfg SettingCfg::getSystemBaseCfg(){
    return systemBaseCfg;
}

/**
 * @brief SettingCfg::getFpdBaseCfg 获得探测器设置中的基础数据
 * @return
 */
FpdBaseCfg SettingCfg::getFpdBaseCfg(){
    return fpdBaseCfg;
}
