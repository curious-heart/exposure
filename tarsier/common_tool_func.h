#ifndef COMMON_TOOL_FUNC_H
#define COMMON_TOOL_FUNC_H

#include <QString>

QString common_tool_get_curr_dt_str();

bool set_fixed_ip_address(QString ipaddr_str, QString addr_mask = "255.255.255.0", QString gw = "");
bool set_dynamic_ip();

#endif // COMMON_TOOL_FUNC_H
