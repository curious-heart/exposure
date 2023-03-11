#ifndef FPDSETTING_H
#define FPDSETTING_H

#include "fpdmodels.h"
#include <QDialog>
#include <QMap>
#include <QList>

namespace Ui {
class FpdSetting;
}

class FpdSetting : public QDialog
{
    Q_OBJECT

public:
    /**fpd_models MUST NOT be nullptr**/
    explicit FpdSetting(QWidget *parent = nullptr, CFpdModels * fpd_models = nullptr);
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
    CFpdModels * m_fpd_models = nullptr; //this is passed from outside, so we do not need to release it.
};

#endif // FPDSETTING_H
