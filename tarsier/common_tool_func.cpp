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
    //bool finished = false;
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
        //{
            QString cmd_line = cmd_str;
            foreach(const QString& s, cmd_args) cmd_line += " " + s;
            DIY_LOG(LOG_INFO, QString("QProcess cmd: %1").arg(cmd_line));
        //}
        bool f_ret = true;
        system(cmd_line.toUtf8());

        /*
         * QProcess needs administrator priviledge, so we use system lib fun instead.
         * But when call system, there is a blink of console window; no good method
         * to avoid it now...
        */

        /*
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
        */
        return f_ret;
    }
    DIY_LOG(LOG_INFO, QString("Set dynamic ip fail: no available interface."));
    return false;
}

bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask, QString gw)
{
    QString cmd_str = "netsh";
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
            DIY_LOG(LOG_INFO, QString("QProcess cmd(utf8)") + cmd_line.toUtf8());
        //}
        bool f_ret = true;
        system(cmd_line.toUtf8());

        /*
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
        */
        return f_ret;
    }
}
