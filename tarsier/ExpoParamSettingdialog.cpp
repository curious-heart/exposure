#include "ExpoParamSettingdialog.h"
#include "ui_ExpoParamSettingdialog.h"
#include "logger.h"
#include <QDesktopWidget>
#include <QMessageBox>

ExpoParamSettingDialog::ExpoParamSettingDialog(QWidget *parent,
                                               expo_params_validator_t* validator) :
    QDialog(parent),
    ui(new Ui::ExpoParamSettingDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    if(validator && validator->dura_list)
    {
        m_validator = *validator;
        m_dura_idx = m_validator.def_dura_idx;
    }
    else
    {
        DIY_LOG(LOG_ERROR, "Validator is not a valid value.....");
        m_validator.valid = false;
        m_dura_idx = 0;
    }

    if(m_validator.valid)
    {
        QString hintStr;
        hintStr = ui->volHintLabel->text()
                + QString(" [%1, %2]").arg(m_validator.min_vol).arg(m_validator.max_vol);
        ui->volHintLabel->setText(hintStr);

        hintStr = ui->amtHintLabel->text()
                + QString(" [%1, %2]").arg(m_validator.min_amt).arg(m_validator.max_amt);
        ui->amtHintLabel->setText(hintStr);
    }

    this->resize(QApplication::desktop()->width()/2,
               QApplication::desktop()->height()/3);

    move((QApplication::desktop()->width() - this->width())/2,
         QApplication::desktop()->height()/20);

    ui->cancelBtn->setText("");
    ui->okBtn->setText("");

    ui->expoDuraDecBtn->setStyleSheet("width: 80; height: 80; border-image: url(:/images/timeSub-able.png)");
    ui->expoDuraIncBtn->setStyleSheet("width: 80; height: 80; border-image: url(:/images/timeAdd-able.png)");
}

ExpoParamSettingDialog::~ExpoParamSettingDialog()
{
    delete ui;
}

bool ExpoParamSettingDialog::validate_params(expo_params_collection_t *params)
{
    if(!m_validator.valid || !params)
    {
        return false;
    }
    if(params->vol > m_validator.max_vol || params->vol < m_validator.min_vol
            || params->amt > m_validator.max_amt || params->amt < m_validator.min_amt
            || params->dura_idx > m_validator.max_dura_idx || params->dura_idx < m_validator.min_dura_idx)
    {
        return false;
    }
    return true;
}

void ExpoParamSettingDialog::update_dialog_params_display(expo_params_collection_t * params)
{
    if(!validate_params(params))
    {
        return;
    }

    ui->tubeVolLineEdit->setText(QString("%1").arg(params->vol));
    ui->tubeVolLineEdit->setCursorPosition(0);
    ui->tubeAmtLineEdit->setText(QString("%1").arg(params->amt));
    ui->tubeAmtLineEdit->setCursorPosition(0);
    ui->expoDuraLineEdit->setText(QString("%1").arg(m_validator.dura_list[params->dura_idx]));
    m_dura_idx = params->dura_idx;
}

void ExpoParamSettingDialog::on_okBtn_clicked()
{
    expo_params_collection_t params;

    params.vol = ui->tubeVolLineEdit->text().toInt();
    params.amt = ui->tubeAmtLineEdit->text().toInt();
    params.dura_idx = m_dura_idx;

    if(!validate_params(&params))
    {
        QMessageBox::critical(this, "!!!",
                              QString("输入参数非法:%1, %2, %3.").arg(params.vol).arg(params.amt).arg(params.dura_idx));
        return;
    }
    else
    {
        emit userInputDone(params);
        emit maskWidgetClosed();
        this->close();
    }
}

void ExpoParamSettingDialog::on_cancelBtn_clicked()
{
    emit maskWidgetClosed();
    this->close();
}

void ExpoParamSettingDialog::on_expoDuraIncBtn_clicked()
{
    if(!m_validator.valid)
    {
        return;
    }

    if(m_dura_idx >= m_validator.max_dura_idx)
    {
        m_dura_idx = m_validator.max_dura_idx;
    }
    else
    {
        ++m_dura_idx;
    }
    ui->expoDuraLineEdit->setText(QString("%1").arg(m_validator.dura_list[m_dura_idx]));
}

void ExpoParamSettingDialog::on_expoDuraDecBtn_clicked()
{
    if(!m_validator.valid)
    {
        return;
    }

    if(m_dura_idx <= m_validator.min_dura_idx)
    {
        m_dura_idx = m_validator.min_dura_idx;
    }
    else
    {
        --m_dura_idx;
    }
    ui->expoDuraLineEdit->setText(QString("%1").arg(m_validator.dura_list[m_dura_idx]));
}
