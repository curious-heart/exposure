#ifndef MYCONTROLLER_H
#define MYCONTROLLER_H

#include <QObject>
#include <QModbusDevice>
#include <QModbusDataUnit>

QT_BEGIN_NAMESPACE
class QModbusClient;
QT_END_NAMESPACE

class MyController : public QObject
{
    Q_OBJECT
public:
    explicit MyController(QObject *parent = nullptr);
    ~MyController();
    void init();
    int ConnectionController();
    int DisconnectionController();
    void readData(int address, quint16 size, int serverAddress);
    bool writeData(int address, quint16 size, int serverAddress, QVector<quint16> data);
private:
    QModbusClient *modbusDevice;

signals:
    /**
     * @brief modbusErrorOccurred 下位机操作中出现错误
     * @param errorInfo 错误信息
     */
    void modbusErrorOccurred(QString errorInfo);
    /**
     * @brief modbusStateChanged modbus状态已经改变
     * @param state 状态码
     */
    void modbusStateChanged(int state);
    /**
     * @brief readDataFinished 读取下位机数据已经完成
     * @param map 获得的数据
     */
    void readDataFinished(QMap<int,quint16> map);
    /**
     * @brief writeDataFinished 向下位机写入数据已经完成
     * @param cmd_addr 写入地址；ret 写入结果。
     */
    void writeDataFinished(int cmd_addr, bool ret);

private slots:
    void onModbusErrorOccurred(QModbusDevice::Error newError);
    void onModbusStateChanged(int state);
    void onReadReady();
};

#endif // MYCONTROLLER_H
