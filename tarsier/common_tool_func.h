#ifndef COMMON_TOOL_FUNC_H
#define COMMON_TOOL_FUNC_H

#include <QString>

typedef enum
{
    IP_SET_TYPE_IPV4_DYNAMIC = 0,
    IP_SET_TYPE_IPV4_FIXED,
}ip_set_type_t;;

typedef enum
{
    IP_INTF_OTHER = 0,
    IP_INTF_WIFI,
    IP_INTF_ETHERNET,
}ip_intf_type_t;

QString common_tool_get_curr_dt_str();

bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask = "255.255.255.0", QString gw = "");
bool set_dynamic_ip();
bool set_host_ip_address(ip_intf_type_t intf, ip_set_type_t set_type, QString ip_addr = "", QString ip_mask = "255.255.255.0", QString gw = "");

#endif // COMMON_TOOL_FUNC_H
