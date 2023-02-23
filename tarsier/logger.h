#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QDateTime>
#include <QDebug>

enum LOG_LEVEL {
    LOG_DEBUG=0,//调试
    LOG_INFO,   //信息
    LOG_WARN,   //警告
    LOG_ERROR   //错误
};
#define DIY_LOG(level, fmt_str, ...) \
    {\
        QString log = QString::asprintf(fmt_str, ##__VA_ARGS__);\
        QString date=QDateTime::currentDateTime().toString("yyyy-MM-dd");\
        QString time=QDateTime::currentDateTime().toString("hh:mm:ss.zzz");\
        QString loc_str = QString(__FILE__) + QString("  [%1]").arg(__LINE__);\
        QString log_str = date + " " + time + " " + log + QString("....") + loc_str + "\n";\
        qDebug() << (log_str);\
    }
#endif // LOGGER_H
