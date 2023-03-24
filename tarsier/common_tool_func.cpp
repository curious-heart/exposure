#include "common_tool_func.h"
#include "logger.h"

#include <QDateTime>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QList>
#include <QProcess>

#define SYSTEM_LIB_FUNC_RET_OK 0

static QNetworkInterface::InterfaceType local_intf_type_to_qnintf_type(ip_intf_type_t l_t)
{
    switch(l_t)
    {
       case IP_INTF_WIFI:
            return QNetworkInterface::Wifi;

       case IP_INTF_ETHERNET:
            return QNetworkInterface::Ethernet;

        default:
            return QNetworkInterface::Unknown;
    }
}

static ip_intf_type_t qnintf_type_to_local_intf_type(QNetworkInterface::InterfaceType q_t)
{
    switch(q_t)
    {
        case QNetworkInterface::Wifi:
            return IP_INTF_WIFI;

        case QNetworkInterface::Ethernet:
            return IP_INTF_ETHERNET;

        default:
            return IP_INTF_OTHER;
    }
}

void set_dhcp_on_intf_with_spec_ip(QString fixed_ip)
{
    //netsh interface ip set address 19 dhcp
    QString cmd_str = "";
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (const QNetworkInterface &interface, interfaces)
    {
        foreach(const QNetworkAddressEntry &e, interface.addressEntries())
        {
            if(e.ip().toString() == fixed_ip)
            {
                int intf_idx = interface.index();
                cmd_str += QString("netsh interface ip set address %1 dhcp && ").arg(intf_idx);
                break;
            }
        }
    }
    if(!cmd_str.isEmpty())
    {
        cmd_str.chop(3); //remove the tail "&& "
        DIY_LOG(LOG_INFO,
                QString("There some interfaces has ip %1, now we set them to dhcp.").arg(fixed_ip));
        DIY_LOG(LOG_INFO, QString("cmd: %1").arg(cmd_str));
        int sys_call_ret = system(cmd_str.toUtf8());
        DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
    }
    else
    {
        DIY_LOG(LOG_INFO, QString("No interface has ip %1.").arg(fixed_ip));
    }
}

void get_q_network_intf_by_type(ip_intf_type_t targ_l_intf_t, QList<QNetworkInterface> * intf_l,
                                bool is_up)
{
    if(!intf_l)
    {
        return;
    }

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (const QNetworkInterface &interface, interfaces)
    {
        int if_idx = interface.index();
        QNetworkInterface::InterfaceFlags if_f = interface.flags();
        QNetworkInterface::InterfaceType if_t = interface.type();
        QString if_name = interface.name();
        QString if_hr_name = interface.humanReadableName();
        QString if_hd_addr = interface.hardwareAddress();
        DIY_LOG(LOG_INFO, "interface + ==================================");
        QString if_info_str
                = QString("Interface id: %1, flags: %2, type: %3, name: %4, hr_name: %5, hd_addr: %6, ")
                .arg(if_idx).arg(if_f).arg(if_t).arg(if_name, if_hr_name, if_hd_addr);
        if_info_str += "ip: ";
        foreach(const QNetworkAddressEntry &e, interface.addressEntries())
        {
            if_info_str += e.ip().toString() + ", ";
        }
        DIY_LOG(LOG_INFO, if_info_str);
        DIY_LOG(LOG_INFO, "interface - ==================================");

        ip_intf_type_t cur_l_intf_t = qnintf_type_to_local_intf_type(if_t);
        // 过滤Loopback接口
        if((if_f & QNetworkInterface::IsLoopBack)
                || (is_up && !(if_f & QNetworkInterface::IsUp)))
        {
            continue;
        }
        if (cur_l_intf_t & targ_l_intf_t)
        {
            intf_l->append(interface);
        }
    }
}

bool interface_has_this_ip(const QNetworkInterface &intf, QString ip_addr)
{
    foreach(const QNetworkAddressEntry &e, intf.addressEntries())
    {
        if(e.ip().toString() == ip_addr)
        {
            return true;
        }
    }
    return false;
}

/*set the first proper interface.*/
bool set_host_wifi_or_eth_ip_addr(ip_set_type_t set_type, QString ip_addr, QString ip_mask, QString gw)
{
    QList<QNetworkInterface> q_intf_l;
    bool ret = false;
    QString info_str = QString("Set ip %1 (set_type)%2").arg(ip_addr).arg((int)set_type);
    q_intf_l.clear();
    get_q_network_intf_by_type((ip_intf_type_t)(IP_INTF_WIFI | IP_INTF_ETHERNET), &q_intf_l);
    if(q_intf_l.count() > 0)
    {
        DIY_LOG(LOG_INFO, QString("Found %1 proper interfaces:").arg(q_intf_l.count()));
        QString l_str = "";
        for(int i = 0; i < q_intf_l.count(); ++i)
        {
            l_str += QString("\n(if_idx)%1, %2, %3\n")
                    .arg(q_intf_l[i].index())
                    .arg(q_intf_l[i].humanReadableName(), q_intf_l[i].hardwareAddress());
        }
        DIY_LOG(LOG_INFO, l_str);
        if(q_intf_l.count() > 1)
        {
            DIY_LOG(LOG_INFO, "Now we try to set ip for the 1st interface of above.");
        }

        if(IP_SET_TYPE_IPV4_FIXED == set_type)
        {
            if(interface_has_this_ip(q_intf_l[0], ip_addr))
            {
                DIY_LOG(LOG_INFO,
                        QString("The 1st interface already has the specified IP %1."
                                  "So we do not need to set it again.").arg(ip_addr));
                q_intf_l.clear();
                return true;
            }
            else
            {
                DIY_LOG(LOG_INFO,
                        QString("Try to set fixed ip %1. We first check if there are any"
                                  " interface has the same ip, and if so, we clear it "
                                  "(set it to dhcp cfg).").arg(ip_addr));
                set_dhcp_on_intf_with_spec_ip(ip_addr);
            }
        }

        int intf_idx = q_intf_l[0].index();
        set_host_ip_address(intf_idx, set_type, ip_addr, ip_mask, gw);
        /*
         * fow now, we can't easily check if the interface has dhcp or dynamic ip setting.
         * so we just clear fixe ip before we set fixed ip and assume it work ok;
         * and for dhcp set call, just set it and assume it works ok.
        */
        ret = true;
        info_str += QString(" for (if_idx)%1: %2, %3")
                    .arg(intf_idx).arg(q_intf_l[0].humanReadableName(), q_intf_l[0].hardwareAddress());
        if(!ret)
        {
            DIY_LOG(LOG_ERROR, info_str + " fails!");
        }
        else
        {
            DIY_LOG(LOG_INFO, info_str + " succeefully.");
        }
    }
    else
    {
        info_str += QString(" fails: no proper interface found.");
        DIY_LOG(LOG_WARN, info_str);
    }
    q_intf_l.clear();

    return ret;
}

bool set_host_ip_address(int if_idx, ip_set_type_t set_type, QString ip_addr, QString ip_mask,
                         QString gw)
{
    QString cmd_line, cmd_str = "netsh";
    cmd_line = cmd_str + " interface" + " ip" + " set" + " address"
            + QString(" %1").arg(if_idx);
    if(IP_SET_TYPE_IPV4_DYNAMIC == set_type)
    {
        cmd_line += " dhcp";
    }
    else
    {
        cmd_line += QString(" static") + " " + ip_addr + " " + ip_mask + " " + gw;
    }
    int sys_call_ret = system(cmd_line.toUtf8());
    DIY_LOG(LOG_INFO, QString("IP set cmd: %1").arg(cmd_line));
    DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
    return (SYSTEM_LIB_FUNC_RET_OK == sys_call_ret);
}

bool set_host_ip_address(ip_intf_type_t intf, ip_set_type_t set_type, QString ip_addr,
                         QString ip_mask, QString gw)
{
    bool ret = false;
    QNetworkInterface::InterfaceType q_intf;
    q_intf = local_intf_type_to_qnintf_type(intf);
    if(QNetworkInterface::Unknown == q_intf)
    {
        DIY_LOG(LOG_ERROR, QString("Unknown interface type: %1").arg(intf));
        return ret;
    }

    bool found = false;
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
        QString if_info_str
                = QString("Interface id: %1, flags: %2, type: %3, name: %4, hr_name: %5, hd_addr: %6, ")
                .arg(if_idx).arg(if_f).arg(if_t).arg(if_name, if_hr_name, if_hd_addr);
        if_info_str += "ip: ";
        foreach(const QNetworkAddressEntry &e, interface.addressEntries())
        {
            if_info_str += e.ip().toString() + ", ";
        }
        DIY_LOG(LOG_INFO, if_info_str);
        // 过滤非活动接口和Loopback接口
        if (!(if_f & QNetworkInterface::IsUp)
                || (if_f & QNetworkInterface::IsLoopBack)
                || ((if_t != q_intf))
                )
        {
            continue;
        }
        QString cmd_line, cmd_str = "netsh";
        cmd_line = cmd_str + " interface" + " ip" + " set" + " address"
                + QString(" %1").arg(if_idx);
        if(IP_SET_TYPE_IPV4_DYNAMIC == set_type)
        {
            cmd_line += " dhcp";
        }
        else
        {
            cmd_line += QString(" static") + " " + ip_addr + " " + ip_mask + " " + gw;
        }
        int sys_call_ret = system(cmd_line.toUtf8());
        DIY_LOG(LOG_INFO, QString("IP set cmd: %1").arg(cmd_line));
        DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
        ret = (SYSTEM_LIB_FUNC_RET_OK  == sys_call_ret);
        found = true;
        break;
    }
    if(!found)
    {
        DIY_LOG(LOG_WARN, "No proper interface found for setting.");
    }
    return ret;
}

bool set_dynamic_ip()
{
    bool f_ret = false, found = false;;
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
        int sys_call_ret = system(cmd_line.toUtf8());
        DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
        f_ret = (SYSTEM_LIB_FUNC_RET_OK == sys_call_ret);
        found = true;
        break;

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
        return f_ret;
        */
    }
    if(!found)
    {
        DIY_LOG(LOG_INFO, QString("Set dynamic ip fail: no available interface."));
    }
    return f_ret;
}

bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask, QString gw)
{
    bool f_ret = false, found = false;
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
        int sys_call_ret = system(cmd_line.toUtf8());
        DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
        found = true;
        f_ret = (SYSTEM_LIB_FUNC_RET_OK == sys_call_ret);
        break;

        /*
        f_ret = true;
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
        */
    }
    if(!found)
    {
        DIY_LOG(LOG_INFO, QString("Set fixed ip %1 fail: no available interface.").arg(ipaddr_str));
    }
    return f_ret;
}

QString common_tool_get_curr_dt_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString dtstr = curDateTime.toString("yyyyMMddhhmmss");
    return dtstr;
}
