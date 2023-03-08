#ifndef EXPOPARAMSETTINGDIALOG_H
#define EXPOPARAMSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ExpoParamSettingDialog;
}

class ExpoParamSettingDialog : public QDialog
{
    Q_OBJECT

public:
    typedef struct
    {
        int vol, amt, dura_idx;
    }expo_params_collection_t;

    typedef struct
    {
        int min_vol, max_vol, def_vol;
        int min_amt, max_amt, def_amt;
        int min_dura_idx, max_dura_idx, def_dura_idx;
        const float * dura_list;
        int dura_list_len;
        bool valid;
    }expo_params_validator_t;

    explicit ExpoParamSettingDialog(QWidget *parent = nullptr,
                                    expo_params_validator_t* validator = nullptr);
    ~ExpoParamSettingDialog();

    void update_dialog_params_display(expo_params_collection_t * params);

signals:
    void maskWidgetClosed();
    void userInputDone(expo_params_collection_t params);

private slots:
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

    void on_expoDuraIncBtn_clicked();

    void on_expoDuraDecBtn_clicked();

private:
    Ui::ExpoParamSettingDialog *ui;
    expo_params_validator_t m_validator;
    int m_dura_idx;
    bool validate_params(expo_params_collection_t *params);
};

#endif // EXPOPARAMSETTINGDIALOG_H
