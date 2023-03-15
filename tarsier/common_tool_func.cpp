#include "common_tool_func.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QList>

QString common_tool_get_curr_dt_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString dtstr = curDateTime.toString("yyyyMMddhhmmss");
    return dtstr;
}

bool set_fixed_ip_address(QString ipaddr_str)
{
    bool finished = false;
    // 获取本地网络接口列表
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // 遍历接口列表，查找需要设置IP地址的接口
    foreach (QNetworkInterface interface, interfaces)
    {
        // 过滤非活动接口和Loopback接口
        if (!(interface.flags() & QNetworkInterface::IsUp)
                || (interface.flags() & QNetworkInterface::IsLoopBack))
        {
            continue;
        }
        // 获取接口的IP地址列表
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        // 遍历IP地址列表，设置需要设置的IP地址
        foreach (QNetworkAddressEntry entry, entries)
        {
            // 判断IP地址类型是否为IPv4
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                // 设置IP地址和子网掩码
                QHostAddress new_ip(ipaddr_str);
                QHostAddress new_netmask("255.255.255.0");
                entry.setIp(new_ip);
                entry.setNetmask(new_netmask);
                // 更新接口的IP地址列表
                entries.replace(entries.indexOf(entry), entry);
                //interface.setAddressEntries(entries);
                finished = true;
                break;
             }
         }
        if(finished)
        {
            break;
        }
    }
    return finished;
}
