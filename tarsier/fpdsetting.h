#ifndef FPDSETTING_H
#define FPDSETTING_H

#include <QDialog>
#include "IRayInclude.h"

namespace Ui {
class FpdSetting;
}

class FpdSetting : public QDialog
{
    Q_OBJECT

public:
    explicit FpdSetting(QWidget *parent = nullptr);
    ~FpdSetting();
    void updateTriggerMode();

signals:
    /**
     * @brief maskWidgetClosed 关闭
     */
    void maskWidgetClosed();

private slots:
    void on_trigger_currentIndexChanged(int index);
    void on_ok_clicked();
    void on_cancel_clicked();   

private:
    Ui::FpdSetting *ui;
};

#endif // FPDSETTING_H
