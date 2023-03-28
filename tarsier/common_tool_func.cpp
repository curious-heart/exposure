#include <windows.h>

#include "common_tool_func.h"
#include "logger.h"


#include <QDateTime>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QList>
#include <QProcess>
#include <QDir>

static bool exec_external_process(QString cmd, QString cmd_args, bool as_admin = false)
{
    DIY_LOG(LOG_INFO, QString("exec_external_process: %1 %2, as_admin: %3")
                      .arg(cmd, cmd_args).arg((int)as_admin));
    bool ret = false;
    if(!cmd.isEmpty())
    {
        SHELLEXECUTEINFO shellInfo;
        memset(&shellInfo, 0, sizeof(shellInfo));
        shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        shellInfo.hwnd = NULL;
        std::wstring wlpstrstd_verb;
        if(as_admin)
        {
            wlpstrstd_verb = QString("runas").toStdWString();
        }
        else
        {
            wlpstrstd_verb = QString("open").toStdWString();
        }
        shellInfo.lpVerb = wlpstrstd_verb.c_str();
        std::wstring wlpstrstd_file = cmd.toStdWString();
        shellInfo.lpFile = wlpstrstd_file.c_str();
        std::wstring wlpstrstd_param = cmd_args.toStdWString();
        shellInfo.lpParameters = wlpstrstd_param.c_str();
        std::wstring wlpstrstd_currpth = QDir::currentPath().toStdWString();
        shellInfo.lpDirectory = wlpstrstd_currpth.c_str();
        shellInfo.nShow = SW_HIDE;
        BOOL bRes = ::ShellExecuteEx(&shellInfo);
        if(bRes)
        {
            ret = true;
            DIY_LOG(LOG_INFO, "ShellExecuteEx ok.");
        }
        else
        {
            ret = false;
            DWORD err = GetLastError();
            DIY_LOG(LOG_ERROR, QString("ShellExecuteEx return false, error: %1").arg((quint64)err));
        }
    }
    else
    {
        DIY_LOG(LOG_WARN, QString("ShellExecuteEx, cmd is empty!"));
    }
    return ret;
}

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
    foreach (const QNetworkInterface &intf, interfaces)
    {
        foreach(const QNetworkAddressEntry &e, intf.addressEntries())
        {
            if(e.ip().toString() == fixed_ip)
            {
                int intf_idx = intf.index();
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

QString get_ip_addr_by_if_idx(int if_idx)
{
    if(if_idx < 0)
    {
        DIY_LOG(LOG_ERROR, "Invalid if_idx: it should be > 0");
        return "";
    }
    QNetworkInterface q_if = QNetworkInterface::interfaceFromIndex(if_idx);
    QString s_ip_addr = "";
    int i = 0;
    foreach(const QNetworkAddressEntry &e, q_if.addressEntries())
    {
        if(e.ip().protocol() == QAbstractSocket::IPv4Protocol)
        {
            DIY_LOG(LOG_INFO, QString("The %1th IP address of if %2 is: %3").arg(i)
                              .arg(if_idx).arg(e.ip().toString()));
            ++i;

            if(s_ip_addr.isEmpty())
            {
                s_ip_addr = e.ip().toString();
            }
        }
    }
    if(s_ip_addr.isEmpty())
    {
        DIY_LOG(LOG_ERROR, QString("Can't obtain an valid IPv4 address of if %1").arg(if_idx));
    }
    return s_ip_addr;
}

void get_q_network_intf_by_type(ip_intf_type_t targ_l_intf_t, QList<QNetworkInterface> * intf_l,
                                bool is_up)
{
    if(!intf_l)
    {
        return;
    }

    QList<QNetworkInterface> intfs = QNetworkInterface::allInterfaces();
    foreach(const QNetworkInterface &intf, intfs)
    {
        int if_idx = intf.index();
        QNetworkInterface::InterfaceFlags if_f = intf.flags();
        QNetworkInterface::InterfaceType if_t = intf.type();
        QString if_name = intf.name();
        QString if_hr_name = intf.humanReadableName();
        QString if_hd_addr = intf.hardwareAddress();
        DIY_LOG(LOG_INFO, "intf + ==================================");
        QString if_info_str
                = QString("Interface id: %1, flags: %2, type: %3, name: %4, hr_name: %5, hd_addr: %6, ")
                .arg(if_idx).arg(if_f).arg(if_t).arg(if_name, if_hr_name, if_hd_addr);
        if_info_str += "ip: ";
        foreach(const QNetworkAddressEntry &e, intf.addressEntries())
        {
            if_info_str += e.ip().toString() + ", ";
        }
        DIY_LOG(LOG_INFO, if_info_str);
        DIY_LOG(LOG_INFO, "intf - ==================================");

        ip_intf_type_t cur_l_intf_t = qnintf_type_to_local_intf_type(if_t);
        // 过滤Loopback接口
        if((if_f & QNetworkInterface::IsLoopBack)
                || (is_up && !(if_f & QNetworkInterface::IsUp)))
        {
            continue;
        }
        if (cur_l_intf_t & targ_l_intf_t)
        {
            intf_l->append(intf);
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

/*if more than 1 interfaces are to be set to fixed ip, only the 1st is set.*/
int set_host_wifi_or_eth_ip_addr(ip_set_type_t set_type, ip_intf_type_t intf_t,
                                  QString ip_addr, QString ip_mask, QString gw)
{
    QList<QNetworkInterface> q_intf_l;
    int set_if_idx = -1;
    //bool ret = false;
    QString info_str = QString("Set ip %1 (set_type)%2").arg(ip_addr).arg((int)set_type);
    q_intf_l.clear();
    //get_q_network_intf_by_type((ip_intf_type_t)(IP_INTF_WIFI | IP_INTF_ETHERNET), &q_intf_l);
    get_q_network_intf_by_type(intf_t, &q_intf_l);
    if(q_intf_l.count() > 0)
    {
        set_if_idx = q_intf_l[0].index();
        /*
         * fow now, we can't easily check if the interface has dhcp or dynamic ip setting.
         * so we just clear fixe ip before we set fixed ip and assume it work ok;
         * and for dhcp set call, just set it and assume it works ok.
        */

        DIY_LOG(LOG_INFO, QString("Found %1 proper interfaces:").arg(q_intf_l.count()));
        QString l_str = "";
        for(int i = 0; i < q_intf_l.count(); ++i)
        {
            l_str += QString("\n(if_idx)%1, %2, %3\n")
                    .arg(q_intf_l[i].index())
                    .arg(q_intf_l[i].humanReadableName(), q_intf_l[i].hardwareAddress());
        }
        DIY_LOG(LOG_INFO, l_str);

        if(IP_SET_TYPE_IPV4_FIXED == set_type)
        {
            if(interface_has_this_ip(q_intf_l[0], ip_addr))
            {
                if(q_intf_l.count() > 1)
                {
                    DIY_LOG(LOG_INFO,
                            QString("There are more than 1 interfaces to be set to fixed IP %1."
                                    "We can set only the first interface.").arg(ip_addr));
                }
                DIY_LOG(LOG_INFO,
                        QString("This interface already has the specified IP %1, "
                                "so we do not need to do anything.").arg(ip_addr));
                q_intf_l.clear();
                return set_if_idx;
            }

            /*The first interface of type intf_t does not have the specified IP.*/
            DIY_LOG(LOG_INFO, QString("Interface (if_idx)%1: %2, %3 is to be set to fixed ip %4.")
                    .arg(set_if_idx)
                    .arg(q_intf_l[0].humanReadableName(), q_intf_l[0].hardwareAddress(), ip_addr));
            QList<QNetworkInterface> all_intfs = QNetworkInterface::allInterfaces();
            if(all_intfs.count() > 1)
            {
                DIY_LOG(LOG_INFO, "There are more than 1 interfaces in system."
                                  "Before set above interface to fixed ip, we'll firstly check and"
                                  "clear (i.e. set to dhcp) other interface of the same ip.");
                for(int i = 0; i < all_intfs.count(); i++)
                {
                    if(interface_has_this_ip(all_intfs[i], ip_addr))
                    {
                        int intf_i = all_intfs[i].index();
                        DIY_LOG(LOG_INFO,
                                QString("Set (if_idx)%1: %2, %3 to dhcp.")
                                .arg(intf_i).arg(all_intfs[i].humanReadableName(),
                                                 all_intfs[i].hardwareAddress()));
                        set_host_ip_address(intf_i, IP_SET_TYPE_IPV4_DYNAMIC);
                    }
                }
                DIY_LOG(LOG_INFO, QString("Check and clear over."));
            }

            DIY_LOG(LOG_INFO,
                    QString("Now set (if_idx)%1: %2, %3 to fixed ip %4.")
                    .arg(set_if_idx)
                    .arg(q_intf_l[0].humanReadableName(), q_intf_l[0].hardwareAddress(), ip_addr));
            //ret =
            set_host_ip_address(set_if_idx, set_type, ip_addr, ip_mask, gw);
        }
        else
        {
           //IP_SET_TYPE_IPV4_DYNAMIC
            DIY_LOG(LOG_INFO, "Set all required interfaces to dhcp:");
            for(int i = 0; i < q_intf_l.count(); i++)
            {
                int intf_i = q_intf_l[i].index();
                DIY_LOG(LOG_INFO,
                        QString("Set (if_idx)%1: %2, %3 to dhcp.")
                        .arg(intf_i)
                        .arg(q_intf_l[i].humanReadableName(), q_intf_l[i].hardwareAddress()));
                DIY_LOG(LOG_INFO, QString("Set interface %1 to dhcp").arg(intf_i));
                set_host_ip_address(q_intf_l[i].index(), set_type, ip_addr, ip_mask, gw);
            }
            //ret = true;
        }
    }
    else
    {
        info_str += QString(" fails: no proper interface found.");
        DIY_LOG(LOG_WARN, info_str);
    }
    q_intf_l.clear();

    return set_if_idx;
}

bool set_host_ip_address(int if_idx, ip_set_type_t set_type, QString ip_addr, QString ip_mask,
                         QString gw)
{
    QString cmd_str = "netsh", cmd_args;
    cmd_args= QString("interface") + " ip" + " set" + " address"
            + QString(" %1").arg(if_idx);
    if(IP_SET_TYPE_IPV4_DYNAMIC == set_type)
    {
        cmd_args += " dhcp";
    }
    else
    {
        cmd_args += QString(" static") + " " + ip_addr + " " + ip_mask + " " + gw;
    }
    DIY_LOG(LOG_INFO, QString("IP set cmd: %1").arg(cmd_str + cmd_args));
    bool exec_ret = exec_external_process(cmd_str, cmd_args, true);
    if(exec_ret)
    {
        DIY_LOG(LOG_INFO, "Set host ip address ok.");
    }
    else
    {
        DIY_LOG(LOG_ERROR, "Set host ip address error!");
    }
    return exec_ret;

    /*
    int sys_call_ret = system((cmd_str + cmd_args).toUtf8());
    DIY_LOG(LOG_INFO, QString("set IP cmd ret: %1").arg(sys_call_ret));
    return (SYSTEM_LIB_FUNC_RET_OK == sys_call_ret);
    */
}

/*not used now*/
bool set_dynamic_ip()
{
    bool f_ret = false, found = false;;
    QString cmd_str = "netsh";
    //bool finished = false;
    // 获取本地网络接口列表
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // 遍历接口列表，查找需要设置IP地址的接口
    foreach (QNetworkInterface intf, interfaces)
    {
        int if_idx = intf.index();
        QNetworkInterface::InterfaceFlags if_f = intf.flags();
        QNetworkInterface::InterfaceType if_t = intf.type();
        QString if_name = intf.name();
        QString if_hr_name = intf.humanReadableName();
        QString if_hd_addr = intf.hardwareAddress();
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

/*not used now*/
bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask, QString gw)
{
    bool f_ret = false, found = false;
    QString cmd_str = "netsh";
    // 获取本地网络接口列表
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    // 遍历接口列表，查找需要设置IP地址的接口
    foreach (QNetworkInterface intf, interfaces)
    {
        int if_idx = intf.index();
        QNetworkInterface::InterfaceFlags if_f = intf.flags();
        QNetworkInterface::InterfaceType if_t = intf.type();
        QString if_name = intf.name();
        QString if_hr_name = intf.humanReadableName();
        QString if_hd_addr = intf.hardwareAddress();
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

QString common_tool_get_curr_date_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString datestr = curDateTime.toString("yyyyMMdd");
    return datestr;
}

bool mkpth_if_not_exists(QString &pth_str)
{
    QDir data_dir(pth_str);
    bool ret = true;
    if(!data_dir.exists())
    {
        data_dir = QDir();
        ret = data_dir.mkpath(pth_str);
    }
    return ret;
}
