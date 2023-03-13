#ifndef SYSTEMSETTING_H
#define SYSTEMSETTING_H

#include <QDialog>
#include "fpdmodels.h"

namespace Ui {
class SystemSetting;
}

class SystemSetting : public QDialog
{
    Q_OBJECT

public:
    /**fpd_models MUST NOT be nullptr**/
    explicit SystemSetting(QWidget *parent = nullptr, CFpdModels * fpd_models = nullptr);
    ~SystemSetting();
    QString getSerialPort(int nIndex, bool bValue);

signals:
    /**
     * @brief maskWidgetClosed 关闭
     */
    void maskWidgetClosed();
    /**
     * @brief fpdAndControllerConnect 连接探测器和下位机控制器
     */
    void fpdAndControllerConnect();

    /**/
    void systemSettingAccepted();

private slots:
    void on_sysOk_clicked();

    void on_sysCancel_clicked();

private:
    Ui::SystemSetting *ui;
    CFpdModels * m_fpd_models = nullptr; //this is passed from outside, so we do not need to release it.
};

#endif // SYSTEMSETTING_H
