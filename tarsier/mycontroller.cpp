#include "mycontroller.h"
#include <QModbusRtuSerialMaster>
#include "settingcfg.h"
#include <QVariant>
#include <QMetaEnum>
#include <QDebug>


/**
 * @brief MyController::MyController 构造函数
 * @param parent
 */
MyController::MyController(QObject *parent) : QObject(parent){
    init();
}


/**
 * @brief MyController::~MyController 析构函数
 */
MyController::~MyController(){
    DisconnectionController();
    delete modbusDevice;
    modbusDevice=NULL;
}


/**
 * @brief MyController::init 初始化
 */
void MyController::init(){
     modbusDevice = new QModbusRtuSerialMaster(this);
     connect(modbusDevice,&QModbusClient::errorOccurred,this,&MyController::onModbusErrorOccurred);
     connect(modbusDevice, &QModbusClient::stateChanged,this, &MyController::onModbusStateChanged);
}


/**
 * @brief MyController::ConnectionController 连接下位机控制器
 * @return
 */
int MyController::ConnectionController(){
    if (!modbusDevice){
        return 1;//初始化失败
    }
    if(modbusDevice->state()!=QModbusDevice::ConnectedState){//当前状态是未连接完成
        QString serialPortName=SettingCfg::getInstance().getSystemSettingCfg().serialPortName;
        int serialParity=SettingCfg::getInstance().getSystemSettingCfg().serialParity;
        int serialBaudRate=SettingCfg::getInstance().getSystemSettingCfg().serialBaudRate;
        int serialDataBits=SettingCfg::getInstance().getSystemSettingCfg().serialDataBits;
        int serialStopBits=SettingCfg::getInstance().getSystemSettingCfg().serialStopBits;
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,QVariant(serialPortName));
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,serialParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,serialBaudRate);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,serialDataBits);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,serialStopBits);
        int timeout=SettingCfg::getInstance().getSystemSettingCfg().timeout;
        int numberOfRetries=SettingCfg::getInstance().getSystemSettingCfg().numberOfRetries;
        modbusDevice->setTimeout(timeout);//连接超时设置
        modbusDevice->setNumberOfRetries(numberOfRetries);//连接失败重试次数设置
        if(modbusDevice->connectDevice()){//连接设备成功
            return 0;
        }else{
            return 2;//连接失败
        }
    }
    return 0;//连接成功
}


/**
 * @brief MyController::DisconnectionController 断开下位机控制器
 * @return
 */
int MyController::DisconnectionController(){
    if(modbusDevice){
        modbusDevice->disconnectDevice();
    }
    return 0;//成功断开
}


/**
 * @brief MyController::readData 读取下位机控制器的数据
 * @param address 起始地址
 * @param size 个数
 * @param serverAddress 服务器地址
 */
void MyController::readData(int address, quint16 size, int serverAddress){
    if (!modbusDevice){
        return;
    }
    if (auto *reply = modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters,address,size), serverAddress)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &MyController::onReadReady);
        }else{
            delete reply;//广播回复立即返回
        }
    }else{
        printf("读取数据错误\n");
    }
}


/**
 * @brief MyController::writeData 给下位机控制器写入数据
 * @param address 起始地址
 * @param size 个数
 * @param serverAddress 服务器地址
 * @param data 提交的数据
 * @return 写入是否成功
 */
bool MyController::writeData(int address, quint16 size, int serverAddress, QVector<quint16> data){
    if (!modbusDevice){
        return false;
    }
    QModbusDataUnit writeUnit=QModbusDataUnit(QModbusDataUnit::HoldingRegisters,address,size);
    for (int i = 0,total = int(writeUnit.valueCount()); i <total; i++) {
        if (data.size() > i){
            writeUnit.setValue(i, data.at(i));
        }
    }
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, serverAddress))
    {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished,
                    this,
                    [this, reply, address]()
                    {
                        bool ret = true;
                        if (reply->error() == QModbusDevice::ProtocolError) {
                            ret = false;
                            emit modbusErrorOccurred(tr("Write response error: %1 (Mobus exception: 0x%2)")
                                                     .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));
                        } else if (reply->error() != QModbusDevice::NoError) {
                            ret = false;
                            emit modbusErrorOccurred(tr("Write response error: %1 (Mobus error code: 0x%2)").
                                                     arg(reply->errorString()).arg(reply->error(), -1, 16));
                        }
                        reply->deleteLater();

                        emit writeDataFinished(address, ret);
                    });
        }else {
            reply->deleteLater(); //广播回复立即返回
            emit writeDataFinished(address, true);
        }
        return true;
    }else {
        printf("写入数据错误\n ");
        return false;
    }
}


/**
 * @brief MyController::onModbusErrorOccurred 接收错误信息（包装后便于其他类使用）
 * @param newError 错误信息
 */
void MyController::onModbusErrorOccurred(QModbusDevice::Error newError){
    QMetaEnum  metaEnum = QMetaEnum ::fromType<QModbusDevice::Error>();
    emit modbusErrorOccurred(metaEnum.valueToKey(newError));
}

/**
 * @brief MyController::onModbusStateChanged Modbus的状态（包装后便于其他类使用）
 * @param state 状态
 */
void MyController::onModbusStateChanged(int state)
{
    emit modbusStateChanged(state);
}


/**
 * @brief MyController::onReadReady 准备读取数据
 */
void MyController::onReadReady(){
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply){
        return;
    }
    QMap<int,quint16> map;
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
//        qDebug()<<tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
//                                     .arg(QString::number(unit.value(i),
//                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            map.insert(unit.startAddress() + i, unit.value(i));
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        emit modbusErrorOccurred(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16));
    } else {
        emit modbusErrorOccurred(tr("Read response error: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16));
    }
    if(map.size() > 0){
        emit readDataFinished(map);
    }
    reply->deleteLater();
}




