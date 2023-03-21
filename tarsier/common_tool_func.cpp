#include "common_tool_func.h"
#include "logger.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QList>
#include <QProcess>

QString common_tool_get_curr_dt_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString dtstr = curDateTime.toString("yyyyMMddhhmmss");
    return dtstr;
}

bool set_dynamic_ip()
{
    QString cmd_str = "netsh";
    bool finished = false;
    // 获取本地网络接口列表
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // 遍历接口列表，查找需要设置IP地址的接口
    foreach (QNetworkInterface interface, interfaces)
    {
        int if_idx = interface.index();
        QNetworkInterface::InterfaceFlags if_f = interface.flags();
        QNetworkInterface::InterfaceType if_t = interface.type();
        QString if_name = interface.name();
        QString if_hr_name = interface.humanReadableName();
        QString if_hd_addr = interface.hardwareAddress();
        DIY_LOG(LOG_INFO, "==================================");
        DIY_LOG(LOG_INFO,
                QString("Interface id: %1, flags: %2, type: %3, name: %4, hr_name: %5, hd_addr: %6")
                .arg(if_idx).arg(if_f).arg(if_t).arg(if_name).arg(if_hr_name).arg(if_hd_addr));
        // 过滤非活动接口和Loopback接口
        if (!(if_f & QNetworkInterface::IsUp)
                || (if_f & QNetworkInterface::IsLoopBack)
                //|| ((if_t != QNetworkInterface::Ethernet) && (if_t != QNetworkInterface::Wifi))
                || ((if_t != QNetworkInterface::Wifi))
                )
        {
            continue;
        }
        QStringList cmd_args;
        cmd_args << "interface" << "ip" << "set" << "address" << QString("%1").arg(if_idx) << "dhcp";
        {
            QString cmd_line = cmd_str;
            foreach(const QString& s, cmd_args) cmd_line += " " + s;
            DIY_LOG(LOG_INFO, QString("QProcess cmd: %1").arg(cmd_line));
        }
        bool f_ret = true;
        QProcess qp;
        qp.start(cmd_str, cmd_args);
        bool qp_wf_ret = qp.waitForFinished(10000);
        QProcess::ExitStatus ex_s = qp.exitStatus();
        if(!qp_wf_ret)
        {
            DIY_LOG(LOG_ERROR, QString("Set dynamic ip on %1, process not exit properly.").arg(if_hr_name));
            f_ret = false;
        }
        if(ex_s != QProcess::NormalExit)
        {
            DIY_LOG(LOG_ERROR, QString("Set dynamic ip on %1, process crashed.").arg(if_hr_name));
            f_ret = false;
        }
        else
        {
            int ex_c = qp.exitCode();
            DIY_LOG(LOG_ERROR, QString("Set dynamic ip on %1, exit code: %2.").arg(if_hr_name).arg(ex_c));
            if(ex_c != 0) f_ret = false;
        }
        return f_ret;
    }
    DIY_LOG(LOG_INFO, QString("Set dynamic ip fail: no available interface."));
    return false;
}

bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask, QString gw)
{
    QString cmd_str = "netsh";
    bool finished = false;
    // 获取本地网络接口列表
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // 遍历接口列表，查找需要设置IP地址的接口
    foreach (QNetworkInterface interface, interfaces)
    {
        int if_idx = interface.index();
        QNetworkInterface::InterfaceFlags if_f = interface.flags();
        QNetworkInterface::InterfaceType if_t = interface.type();
        QString if_name = interface.name();
        QString if_hr_name = interface.humanReadableName();
        QString if_hd_addr = interface.hardwareAddress();
        DIY_LOG(LOG_INFO, "==================================");
        DIY_LOG(LOG_INFO,
                QString("Interface id: %1, flags: %2, type: %3, name: %4, hr_name: %5, hd_addr: %6")
                .arg(if_idx).arg(if_f).arg(if_t).arg(if_name).arg(if_hr_name).arg(if_hd_addr));
        // 过滤非活动接口和Loopback接口
        if (!(if_f & QNetworkInterface::IsUp)
                || (if_f & QNetworkInterface::IsLoopBack)
                //|| ((if_t != QNetworkInterface::Ethernet) && (if_t != QNetworkInterface::Wifi))
                || ((if_t != QNetworkInterface::Wifi))
                )
        {
            continue;
        }
        QStringList cmd_args;
        cmd_args << "interface" << "ip" << "set" << "address"
                 << QString("%1").arg(if_idx)
                 << "static" << ipaddr_str << addr_mask << gw;
        //{
            QString cmd_line = cmd_str;
            foreach(const QString& s, cmd_args) cmd_line += " " + s;
            DIY_LOG(LOG_INFO, QString("QProcess cmd: %1").arg(cmd_line));
        //}
        bool f_ret = true;
        QProcess qp;
        qp.start(cmd_str, cmd_args);
        bool qp_wf_ret = qp.waitForFinished(10000);
        QProcess::ExitStatus ex_s = qp.exitStatus();
        if(!qp_wf_ret)
        {
            DIY_LOG(LOG_ERROR, QString("Set ip addr %1 process not exit properly.").arg(ipaddr_str));
            f_ret = false;
        }
        if(ex_s != QProcess::NormalExit)
        {
            DIY_LOG(LOG_ERROR, QString("Set ip addr %1 crashed.").arg(ipaddr_str));
            f_ret = false;
        }
        else
        {
            int ex_c = qp.exitCode();
            DIY_LOG(LOG_INFO, QString("Set ip addr %1 finished, exit code: %2").arg(ipaddr_str).arg(ex_c));
            if(ex_c != 0) f_ret = false;
        }
        return f_ret;

        // 获取接口的IP地址列表
        QList<QNetworkAddressEntry> entries = interface.addressEntries();
        // 遍历IP地址列表，设置需要设置的IP地址
        foreach (QNetworkAddressEntry entry, entries)
        {
            QAbstractSocket::NetworkLayerProtocol prot = entry.ip().protocol();
            QString addr = entry.ip().toString();
            DIY_LOG(LOG_INFO, QString("----prot: %1, addr: %2").arg(prot).arg(addr));
            // 判断IP地址类型是否为IPv4
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                QNetworkAddressEntry new_entry = entry;
                // 设置IP地址和子网掩码
                QHostAddress new_ip(ipaddr_str);
                QHostAddress new_netmask("255.255.255.0");
                //entry.setIp(new_ip);
                //entry.setNetmask(new_netmask);
                new_entry.setIp(new_ip);
                new_entry.setNetmask(new_netmask);
                int e_idx = entries.indexOf(entry);
                // 更新接口的IP地址列表
                entries.replace(e_idx, entry);
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
