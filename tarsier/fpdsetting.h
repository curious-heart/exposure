#ifndef FPDSETTING_H
#define FPDSETTING_H

#include <QDialog>
#include <QMap>
#include <QList>

namespace Ui {
class FpdSetting;
}

typedef struct
{
    QMap<QString, int> trigger_mode_list; //key: trigger mode name; value: enum or macro value.
}fpd_series_capabilites_t;

typedef QMap<QString, fpd_series_capabilites_t*> fpd_series_list_cap_t;

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
    fpd_series_list_cap_t m_fpd_series_list_cap;
    void clear_m_fpd_mfg_list_cap();
    void setup_fpd_capabilities_on_series();
    /*n: fpd name. return series name; if mfg is not null, mfg return mfg name.*/
    QString get_series_from_fpd_name(QString & n, QString* mfg = nullptr);
    fpd_series_capabilites_t* get_fpd_capabilities_from_name(QString &n);
};

/*PZM dose not have trigger mode definition. So we define it here by ourseleves.*/
typedef enum
{
    PZM_TRIGGER_MODE_AED = 11,
    PZM_TRIGGER_MODE_HST,
}PZM_trigger_mode_t;

#endif // FPDSETTING_H
