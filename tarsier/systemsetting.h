#ifndef SYSTEMSETTING_H
#define SYSTEMSETTING_H

#include <QDialog>

namespace Ui {
class SystemSetting;
}

class SystemSetting : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSetting(QWidget *parent = nullptr);
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

private slots:
    void on_sysOk_clicked();

    void on_sysCancel_clicked();

private:
    Ui::SystemSetting *ui;
};

#endif // SYSTEMSETTING_H
