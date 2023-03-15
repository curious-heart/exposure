#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QSlider>
#include <QTcpServer>
#include <QPushButton>
#include <QLabel>
#include <QWebSocketServer>
#include <QLineEdit>
#include "IRayInclude.h"
#include "imageoperation.h"
#include "lowbatterywarning.h"
#include "ExpoParamSettingdialog.h"
#include "fpdmodels.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
    class SystemSetting;
    class ExitSystem;
    class FpdSetting;
    class MaskWidget;
}
class MyFPD;
class MyController;
QT_END_NAMESPACE

class MyFPD;
class MyController;
class SystemSetting;
class ExitSystem;
class FpdSetting;
class MaskWidget;
//class InputMethod;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static void MyCallbackHandler(int nDetectorID, int nEventID, int nEventLevel,const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);
    static void SaveFile(const void* pData, unsigned size);

private slots:
    void on_connect_clicked();
    void onFpdConnectStateChanged(int state);
    void onControllerConnectStateChanged(int state);
    void onErrorOccurred(QString errorInfo);
    void onReadControllerDataFinished(QMap<int,quint16> map);
    void on_exposure_clicked();
    void on_range_clicked();
    void onReadExposureStatusTimerOutTime();
    void onReadRangeStatusTimerOutTime();
    void onReadVoltmeterAndAmmeterTimerOutTime();
    void onReadBatteryLevelTimerOutTime();
    void onReadChargeStateTimerOutTime();
    void onCheckSleepAndShutdownTimerOutTime();
    //void onImageSaveFinshed(QString path);
    void onImageCreateFinshed(QImage *img,QString imageNum, ImageOperation::img_op_type_t op_t = ImageOperation::IMG_OP_ROTATE_R_90);
    void on_exitSystem_clicked();
    void onQDialogClosed();
    void on_systemSetting_clicked();
    void on_fpdSetting_clicked();
    void onConnectFpdAndController();
    void on_saveImage_clicked();
    void onRotateButtonClicked();
    void onWWWLButtonClicked(bool checked);
    void onAntiColorButtonClicked(bool checked);
    void onZoomButtonClicked(bool checked);
    void onTranslationButtonClicked(bool checked);
    void onRestButtonClicked();
    void onContrastSubClicked();
    void onContrastAddClicked();
    void onBrightSubClicked();
    void onBrightAddClicked();
    void onContrastChanged(int value);
    void onBrightChanged(int value);
    void onNewConnectioned();
//    void onReadyReaded();
    void onTextMessageReceived(QString message);
    void onImageLoaded();
    void onWwwlChanged(int ww,int wl);
    void onReadFpdBatteryLevelTimerOutTime();
    void onTenMinutesTimerStarted();
    void onTenMinutesTimerOutTime();
    void onStartAcqWaitTimerTimeOut();

    void on_exposureSelCombox_currentIndexChanged(int index);
    void on_exposureUserInputDone(ExpoParamSettingDialog::expo_params_collection_t params);

    void on_exposureSelCombox_activated(int index);
    void on_systemSettingAccepted();

    void on_pzm_fpd_comm_sig(int evt, int sn_id = -1, QString sn_str = "");
    void on_pzm_fpd_img_received_sig(char* img, int width, int height, int bit_dep);

protected:
    FPDRESULT disconnect_works(bool part_disconn = false);



public slots:
    void onKeyOrMouseEventOccurred();
    void onWriteControllerDataFinished(int cmd_addr, bool ret);

signals:

private:
    void InitActions();
    void loadBrightContrastAdjuster();
    int ConnectionFPD();
    int DisconnectionFPD();
    int PreOffsetCalibrationTemplate();
    int SoftwareTrigger();
    int InnerTrigger();
    int OuterTrigger();
    int FreesyncTrigger();
    int PREPTrigger();
    void ControllerExposure();
    int SetCalibrationOptions();
    void keyPressEvent(QKeyEvent * k);
    int ConnectionControllerAndSetting();
    bool writeExposureTime(int idx, bool write_cfg_file = true);
    bool writeExposurekV(int kV, bool write_cfg_file = true);
    bool writeExposuremA(int ua, bool write_cfg_file = true);
    bool dDriveExist();
    void refresh_ip_addr();
    void update_controller_or_cfg_on_exposure_combox();
    void setup_exposure_options_combox();
    void update_exposure_parameters_display_on_main();
    bool write_exposure_params_to_controller_or_cfg(ExpoParamSettingDialog::expo_params_collection_t params, bool write_cfg = true);
    void goto_user_input_dialog();
    void update_fpd_handler_on_new_model(fpd_model_info_t* new_model);

private:
    Ui::MainWindow *ui;
    int fpdConnectState;
    int controllerConnectState;
    MyController *controller;
    SystemSetting *systemSetting;
    ExitSystem *exitSystem;
    FpdSetting *fpdSetting;
    MaskWidget *maskWidget;
    ExpoParamSettingDialog* expo_param_setting;
    LowBatteryWarning *lowBatteryWarning;
    CFpdModels * m_fpd_models;
    QTimer * readExposureStatusTimer;
    QTimer * readRangeStatusTimer;
    QTimer * readVoltmeterAndAmmeterTimer;
    QTimer * readBatteryLevelTimer;
    QTimer * readChargeStateTimer;
    QTimer * startAcqWaitTimer;
    QImage * chargeStateImg;
    QImage * showImg;
    //QImage * showImgTemp;
    QDateTime timestamp;
    QTimer * checkSleepAndShutdownTimer;
    bool sleepState;
    bool fpdCreateState;
    int voltageList[13]={2500, 3500, 3680, 3700, 3730, 3770, 3790, 3820, 3870, 3930, 4000, 4080, 4200};
    int batteryLevelList[13]={0, 5, 10, 15, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    QPushButton *rotateButton;
    QPushButton *wwwlButton;
    QPushButton *antiColorButton;
    QPushButton *zoomButton;
    QPushButton *translationButton;
    QPushButton *resetButton;
    QLabel *wwVal;
    QLabel *wlVal;

    QPushButton *brightAdd;
    QLabel *brightLabel;
    QSlider *brightSlider;
    QPushButton *brightSub;
    QPushButton *contrastSub;
    QLabel *contrastLabel;
    QSlider *contrastSlider;  
    QPushButton *contrastAdd;
    bool imageShowState;
    //QTcpServer* mTCPServer ;
    //QTcpSocket *mTCPSocket;
    QWebSocketServer *mWebSocketServer;
    QWebSocket* mWebSocket;
    QTimer * readFpdBatteryLevelTimer;
    QTimer *tenMinutesTimer;
    bool promptState=false;      //低电量是否已经提示 true是  false否；

    //InputMethod * m_inp_md;
    //QLineEdit * m_curr_editor = nullptr;
    /*This var point to an item in m_fpd_models, so do not delete it manually.*/
    fpd_model_info_t* m_curr_fpd_model = nullptr;

};

#endif // MAINWINDOW_H
