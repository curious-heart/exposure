#ifndef COMMON_TOOL_FUNC_H
#define COMMON_TOOL_FUNC_H

#include <QString>
#include <QNetworkInterface>

typedef enum
{
    IP_SET_TYPE_IPV4_DYNAMIC = 0,
    IP_SET_TYPE_IPV4_FIXED,
}ip_set_type_t;;

typedef enum
{
    /*every bit corresponds to one kind of intf.*/
    IP_INTF_OTHER = 0,
    IP_INTF_WIFI = 0x01,
    IP_INTF_ETHERNET = 0x02,
}ip_intf_type_t;

QString common_tool_get_curr_dt_str();

/*intf_l contains the result. caller should pass in a pointer to a list.*/
void get_q_network_intf_by_type(ip_intf_type_t targ_l_intf_t, QList<QNetworkInterface> * intf_l,
                                bool is_up = true);
bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask = "255.255.255.0", QString gw = "");
bool set_dynamic_ip();
bool set_host_ip_address(ip_intf_type_t intf, ip_set_type_t set_type,
                         QString ip_addr = "", QString ip_mask = "255.255.255.0", QString gw = "");
bool set_host_ip_address(int if_idx, ip_set_type_t set_type,
                         QString ip_addr = "", QString ip_mask = "255.255.255.0", QString gw = "");

bool set_host_wifi_or_eth_ip_addr(ip_set_type_t set_type,
                         QString ip_addr = "", QString ip_mask = "255.255.255.0", QString gw = "");

#endif // COMMON_TOOL_FUNC_H
