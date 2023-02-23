#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "systemsetting.h"
#include "exitsystem.h"
#include "fpdsetting.h"
#include "myfpd.h"
#include "mycontroller.h"
#include "settingcfg.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "maskwidget.h"
#include <QDateTime>
#include <QKeyEvent>
#include <QDebug>
#include <powrprof.h>
#include <QDir>
#include <QFileDialog>
#include <QSlider>
#include <QMessageBox>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QHostInfo>
#include <QStyle>

#include <QMouseEvent>
#include <QDesktopServices>
#include <tlhelp32.h>

#include "input/inputmethod.h"

#include "logger.h"

enum Enm_Connect_State
{
    Connected = 0,
    Connecting = 1,
    Disconnected = 2,
    Disconnecting = 3,
    ConnectError = 4,
};
enum Enm_Controller_Address
{
    HSV = 0,                            //软硬件版本
    OTA = 1,                            //OTA升级
    BaudRate = 2,                       //波特率
    ServerAddress = 3,                  //设备地址
    State = 4,                          //状态
    VoltSet = 5,  					 	//5管电压设置值
    FilamentSet = 6,					//6 灯丝电压设置（决定灯丝电流决定管电流）
    ExposureTime = 7,                   //曝光时间
    Voltmeter = 8,                      //管电压
    Ammeter = 9,                        //管电流
    RangeIndicationStatus = 10,         //范围指示状态
    ExposureStatus = 11,                //曝光状态
    RangeIndicationStart = 12,          //范围指示启动
    ExposureStart = 13,                 //曝光启动
    BatteryLevel = 14,                  //电池电量
    BatteryVoltmeter = 15,
    OilBoxTemperature = 16,             //电池电压高位
    Poweroff = 17,							//17 关机请求
    Fixpos = 18, 								//18 油盒温度低位
    Fixval = 19, 								//19 充能状态
    Workstatus = 20,						//20充能状态
    exposureCount = 21,                     //曝光次数
};

static MyFPD *fpd=NULL;
static ImageOperation *imageOperation=NULL;
static QString imagePath=NULL;
static QDateTime T1;//The last connected time or acquisition time
static bool subsetSpecified=false;
//static float exposureTimeList[30]={0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 0.25, 0.30,
//                                   0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80,
//                                   0.85, 0.90, 0.95, 1.00, 1.05, 1.10, 1.15, 1.20, 1.25, 1.30};

#define MAXSTEP 17
static float exposureTimeList[MAXSTEP]={0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80,
                                        0.85, 0.90, 0.95, 1.00, 1.05, 1.10, 1.15, 1.20, 1.25, 1.30};
static int exposureTimeIndex=3;
static int exposureStatus=0;//未启动曝光
//static int rangeStatus=0;//范围指示未启动
static bool dDriveState;//D盘是否存在
static QImage *img=NULL;
static QString imageNum="";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    systemSetting=new SystemSetting(this);
    exitSystem=new ExitSystem(this);
    fpdSetting=new FpdSetting(this);
    controller=new MyController(this);
    maskWidget=new MaskWidget(this);
    lowBatteryWarning=new LowBatteryWarning(this);
    fpd=new MyFPD(this);
    imageOperation=new ImageOperation(this);
    readExposureStatusTimer=new QTimer(this);
    readRangeStatusTimer=new QTimer(this);
    readVoltmeterAndAmmeterTimer=new QTimer(this);
    readBatteryLevelTimer=new QTimer(this);
    readChargeStateTimer=new QTimer(this);
    checkSleepAndShutdownTimer=new QTimer(this);
    readFpdBatteryLevelTimer=new QTimer(this);
    tenMinutesTimer=new QTimer(this);
    chargeStateImg=new QImage();
    showImg=new QImage();
    //showImgTemp=new QImage();
    //    mTCPServer = new QTcpServer();
    //    mTCPSocket=new QTcpSocket();
    mWebSocketServer=new QWebSocketServer("WebSocketServer",QWebSocketServer::NonSecureMode);
    mWebSocket=new QWebSocket();
    sleepState=false;
    fpdCreateState=false;
    imageShowState=false;
    dDriveState=false;
    InitActions();

    ui->volSet->installEventFilter(this);
    ui->amSet->installEventFilter(this);

    //m_inp_md = new InputMethod(this);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete controller;
    controller=NULL;
    delete systemSetting;
    systemSetting=NULL;
    delete exitSystem;
    exitSystem=NULL;
    delete fpdSetting;
    fpdSetting=NULL;
    delete maskWidget;
    maskWidget=NULL;
    delete lowBatteryWarning;
    lowBatteryWarning=NULL;
    delete readExposureStatusTimer;
    readExposureStatusTimer=NULL;
    delete readRangeStatusTimer;
    readRangeStatusTimer=NULL;
    delete readVoltmeterAndAmmeterTimer;
    readVoltmeterAndAmmeterTimer=NULL;
    delete readBatteryLevelTimer;
    readBatteryLevelTimer=NULL;
    delete readChargeStateTimer;
    readChargeStateTimer=NULL;
    delete checkSleepAndShutdownTimer;
    checkSleepAndShutdownTimer=NULL;
    delete readFpdBatteryLevelTimer;
    readFpdBatteryLevelTimer=NULL;
    delete tenMinutesTimer;
    tenMinutesTimer=NULL;
    delete chargeStateImg;
    chargeStateImg=NULL;
    //    delete mTCPServer;
    //    mTCPServer=NULL;
    //    delete mTCPSocket;
    //    mTCPSocket=NULL;
    delete showImg;
    showImg=NULL;
    delete fpd;
    fpd=NULL;
    delete imageOperation;
    imageOperation=NULL;

    //delete m_inp_md;
}

void MainWindow::InitActions(){
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(1024, 600);
    //ui->scrollArea->setFixedSize(780,470);
    ui->preview->setFixedSize(780,470);
    ui->chargeState->setFixedSize(129,32);
    ui->batteryLevel->setFixedSize(129,32);
    ui->fpdbatteryLevel->setFixedSize(129,32);
    ui->voltmeter->setFixedSize(129,32);
    ui->ammeter->setFixedSize(129,32);


    wwVal= new QLabel(ui->preview);
    wwVal->setText("WW:");
    wwVal->setGeometry(700,10,80,20);
    wwVal->setStyleSheet("color:rgb(1,91,206);");
    wlVal= new QLabel(ui->preview);
    wlVal->setText("WL:");
    wlVal->setGeometry(700,30,80,20);
    wlVal->setStyleSheet("color:rgb(1,91,206);");
    wwwlButton = new QPushButton(ui->preview);
    //wwwlButton->setText("窗宽窗位");
    wwwlButton->setCheckable(true);
    wwwlButton->setGeometry(700,60,58,59);
    wwwlButton->setFlat(true);
    wwwlButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/wwwl.png)");
    connect(wwwlButton, &QPushButton::clicked, this,&MainWindow::onWWWLButtonClicked);
    zoomButton = new QPushButton(ui->preview);
    //zoomButton->setText("缩放");
    zoomButton->setCheckable(true);
    zoomButton->setGeometry(700,120,58,59);
    zoomButton->setFlat(true);
    zoomButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/zoom.png)");
    connect(zoomButton, &QPushButton::clicked, this,&MainWindow::onZoomButtonClicked);
    translationButton = new QPushButton(ui->preview);
    //translationButton->setText("平移");
    translationButton->setCheckable(true);
    translationButton->setGeometry(700,180,58,59);
    translationButton->setFlat(true);
    translationButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/translation.png)");
    connect(translationButton, &QPushButton::clicked, this,&MainWindow::onTranslationButtonClicked);
    rotateButton = new QPushButton(ui->preview);
    //rotateButton->setText("旋转");
    rotateButton->setGeometry(700,240,58,59);
    rotateButton->setFlat(true);
    rotateButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/rotate.png)");
    connect(rotateButton, &QPushButton::clicked, this,&MainWindow::onRotateButtonClicked);
    antiColorButton = new QPushButton(ui->preview);
    //antiColorButton->setText("反色");
    antiColorButton->setCheckable(true);
    antiColorButton->setGeometry(700,300,58,59);
    antiColorButton->setFlat(true);
    antiColorButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/invert.png)");
    connect(antiColorButton, &QPushButton::clicked, this,&MainWindow::onAntiColorButtonClicked);
    resetButton = new QPushButton(ui->preview);
    //resetButton->setText("重置");
    resetButton->setGeometry(700,360,58,59);
    resetButton->setFlat(true);
    resetButton->setStyleSheet("width:58px;height:59px;border-image: url(:/images/reset.png)");
    connect(resetButton, &QPushButton::clicked, this,&MainWindow::onRestButtonClicked);

    wwVal->hide();
    wlVal->hide();
    rotateButton->hide();
    wwwlButton->hide();
    antiColorButton->hide();
    zoomButton->hide();
    translationButton->hide();
    resetButton->hide();



    //loadBrightContrastAdjuster();//使用窗宽窗位调节，取消原有的亮度对比度调节组件的加载

    QString info = QHostInfo::localHostName();
    QHostInfo ip = QHostInfo::fromName(info);
    foreach(QHostAddress addr, ip.addresses())
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
            ui->IPaddr->setText("IP:" + addr.toString());
    }

    //加载qss
    QFile file(":/qss/main-style.qss");
    file.open(QFile::ReadOnly);
    QString qss = file.readAll();
    qApp->setStyleSheet(qss);
    file.close();
    fpdConnectState=Enm_Connect_State::Disconnected;
    controllerConnectState=Enm_Connect_State::Disconnected;
    ui->systemSetting->setEnabled(true);
    ui->fpdSetting->setEnabled(true);
    ui->exposure->setEnabled(false);
    ui->range->setEnabled(false);
    ui->addTime->setEnabled(false);
    ui->subTime->setEnabled(false);
    ui->systemSetting->setText(NULL);
    ui->fpdSetting->setText(NULL);
    ui->connect->setText(NULL);
    ui->saveImage->setText(NULL);
    ui->saveImage->setEnabled(false);
    ui->exitSystem->setText(NULL);
    ui->exposure->setText(NULL);
    ui->range->setText(NULL);
    ui->addTime->setText(NULL);
    ui->subTime->setText(NULL);
    if(!(chargeStateImg->load("images/red.png"))){
        delete chargeStateImg;
    }
    ui->chargeState->setAlignment(Qt::AlignCenter);
    ui->chargeState->setPixmap(QPixmap::fromImage(*chargeStateImg));
    //ui->batteryLevel->setValue(100);
    //ui->batteryLevel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#90EE90}");
    ui->batteryLevel->setText(QString("%1%").arg(100));
    ui->fpdbatteryLevel->setText(QString("%1%").arg(100));
    ui->voltmeter->setText(QString("%1").arg(0));
    ui->ammeter->setText(QString("%1").arg(0));
    //onFpdConnectStateChanged(Enm_Connect_State::Connected);//调试用
    //onControllerConnectStateChanged(Enm_Connect_State::Connected);//调试用
    //connect(ui->systemSetting, &QPushButton::clicked, systemSetting, &QPushButton::show);
    //connect(ui->fpdSetting, &QPushButton::clicked, fpdSetting, &QPushButton::show);
    //connect(ui->exitSystem, &QPushButton::clicked, exitSystem, &QPushButton::show);
    connect(systemSetting, &SystemSetting::fpdAndControllerConnect, this,&MainWindow::onConnectFpdAndController);
    connect(systemSetting, &SystemSetting::maskWidgetClosed, this,&MainWindow::onQDialogClosed);
    connect(fpdSetting, &FpdSetting::maskWidgetClosed, this,&MainWindow::onQDialogClosed);
    connect(exitSystem, &ExitSystem::maskWidgetClosed, this,&MainWindow::onQDialogClosed);
    connect(lowBatteryWarning, &LowBatteryWarning::maskWidgetClosed, this,&MainWindow::onQDialogClosed);
    connect(lowBatteryWarning, &LowBatteryWarning::tenMinutesTimerStarted, this,&MainWindow::onTenMinutesTimerStarted);

    connect(controller,&MyController::modbusErrorOccurred,this,&MainWindow::onErrorOccurred);
    connect(controller,&MyController::readDataFinished,this,&MainWindow::onReadControllerDataFinished);
    connect(fpd,&MyFPD::fpdErrorOccurred,this,&MainWindow::onErrorOccurred);
    connect(imageOperation,&ImageOperation::imageOperationErrorOccurred,this,&MainWindow::onErrorOccurred);
    connect(readExposureStatusTimer, &QTimer::timeout, this, &MainWindow::onReadExposureStatusTimerOutTime);
    connect(readRangeStatusTimer, &QTimer::timeout, this, &MainWindow::onReadRangeStatusTimerOutTime);
    connect(readVoltmeterAndAmmeterTimer, &QTimer::timeout, this, &MainWindow::onReadVoltmeterAndAmmeterTimerOutTime);
    connect(readBatteryLevelTimer, &QTimer::timeout, this, &MainWindow::onReadBatteryLevelTimerOutTime);
    connect(readChargeStateTimer, &QTimer::timeout, this, &MainWindow::onReadChargeStateTimerOutTime);
    connect(readFpdBatteryLevelTimer, &QTimer::timeout, this, &MainWindow::onReadFpdBatteryLevelTimerOutTime);
    connect(tenMinutesTimer, &QTimer::timeout, this, &MainWindow::onTenMinutesTimerOutTime);
    //    connect(imageOperation,&ImageOperation::imageSaveFinshed,this,&MainWindow::onImageSaveFinshed);
    connect(imageOperation,&ImageOperation::imageCreateFinshed,this,&MainWindow::onImageCreateFinshed);
    connect(ui->preview,&ImageLabel::imageLoaded,this,&MainWindow::onImageLoaded);
    connect(ui->preview,&ImageLabel::wwwlChanged,this,&MainWindow::onWwwlChanged);

    exposureTimeIndex=SettingCfg::getInstance().getSystemSettingCfg().exposureTimeIndex;
    ui->exposureSetting->setText(QString("%1").arg(exposureTimeList[exposureTimeIndex]));
    timestamp=QDateTime::currentDateTime();
    connect(checkSleepAndShutdownTimer, &QTimer::timeout, this, &MainWindow::onCheckSleepAndShutdownTimerOutTime);
    if(!checkSleepAndShutdownTimer->isActive()){
        checkSleepAndShutdownTimer->setInterval(5*1000);
        checkSleepAndShutdownTimer->start();
    }
    ConnectionControllerAndSetting();//软件启动后，连接下位机

    //监听65432端口
    //    if(!mTCPServer->listen(QHostAddress::Any, 65432))
    //    {
    //         QMessageBox::information(this, "QT网络TCP通信", "服务器端监听65432端口失败！");
    //         return;
    //    }
    //    connect(mTCPServer, SIGNAL(newConnection()), this, SLOT(onNewConnectioned()));
    //监听65432端口
    if(!mWebSocketServer->listen(QHostAddress::Any,65432)){
        QMessageBox::information(this, "QT网络WebSocket通信", "服务器端监听65432端口失败！");
        return;
    }
    connect(mWebSocketServer,SIGNAL(newConnection()),this,SLOT(onNewConnectioned()));
    //判断d盘是否存在
    dDriveState=dDriveExist();
}

/**
 * @brief MainWindow::loadBrightContrastAdjuster 载入亮度对比度调节器
 */
void MainWindow::loadBrightContrastAdjuster()
{
    brightAdd = new QPushButton(ui->preview);
    //brightAdd->setText("亮度+");
    brightAdd->setGeometry(700,100,24,24);
    brightAdd->setFlat(true);
    brightAdd->setStyleSheet("background-repeat: no-repeat;background-position: center;background-image: url(:/images/up.png)");
    connect(brightAdd, SIGNAL(clicked()), this,SLOT(onBrightAddClicked()));
    brightLabel = new QLabel(ui->preview);
    brightLabel->setText("亮度");
    QFont font( "Microsoft YaHei", 12, 60);
    brightLabel->setFont(font);
    brightLabel->setStyleSheet("color:#015BCE");
    brightLabel->setGeometry(680,200,50,20);
    brightSlider=new QSlider(Qt::Vertical,ui->preview);
    brightSlider->setMinimum(-100);
    brightSlider->setMaximum(100);
    brightSlider->setPageStep(10);
    brightSlider->setValue(0);
    brightSlider->setGeometry(730,135,10,150);
    connect(brightSlider, &QSlider::valueChanged, this,&MainWindow::onBrightChanged);
    brightSub = new QPushButton(ui->preview);
    //brightSub->setText("亮度-");
    brightSub->setGeometry(700,300,24,24);
    brightSub->setFlat(true);
    brightSub->setStyleSheet("background-repeat: no-repeat;background-position: center;background-image: url(:/images/down.png)");
    connect(brightSub, SIGNAL(clicked()), this,SLOT(onBrightSubClicked()));
    contrastSub = new QPushButton(ui->preview);
    //contrastSub->setText("对比度-");
    contrastSub->setGeometry(220,430,24,24);
    contrastSub->setFlat(true);
    contrastSub->setStyleSheet("background-repeat: no-repeat;background-position: center;background-image: url(:/images/left.png)");
    connect(contrastSub, SIGNAL(clicked()), this,SLOT(onContrastSubClicked()));
    contrastLabel = new QLabel(ui->preview);
    contrastLabel->setText("对比度");
    contrastLabel->setFont(font);
    contrastLabel->setStyleSheet("color:#015BCE");
    contrastLabel->setGeometry(375,400,50,20);
    contrastSlider=new QSlider(Qt::Horizontal,ui->preview);
    contrastSlider->setMinimum(0);
    contrastSlider->setMaximum(100);
    contrastSlider->setPageStep(5);
    contrastSlider->setValue(0);
    contrastSlider->setGeometry(300,430,200,10);
    connect(contrastSlider, &QSlider::valueChanged, this,&MainWindow::onContrastChanged);
    contrastAdd = new QPushButton(ui->preview);
    //contrastAdd->setText("对比度+");
    contrastAdd->setGeometry(520,430,24,24);
    contrastAdd->setFlat(true);
    contrastAdd->setStyleSheet("background-repeat: no-repeat;background-position: center;background-image: url(:/images/right.png)");
    connect(contrastAdd, SIGNAL(clicked()), this,SLOT(onContrastAddClicked()));
    brightAdd->hide();
    brightLabel->hide();
    brightSlider->hide();
    brightSub->hide();
    contrastSub->hide();
    contrastLabel->hide();
    contrastSlider->hide();
    contrastAdd->hide();
}


/**
 * @brief MainWindow::MyCallbackHandler 探测器回调函数
 * @param nDetectorID
 * @param nEventID
 * @param nEventLevel
 * @param pszMsg
 * @param nParam1
 * @param nParam2
 * @param nPtrParamLen
 * @param pParam
 */
void MainWindow::MyCallbackHandler(int nDetectorID, int nEventID, int nEventLevel,
                                   const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam)
{
    fpd->MyCallback(nDetectorID, nEventID, nEventLevel, pszMsg, nParam1, nParam2, nPtrParamLen, pParam);
    switch (nEventID){
    case Evt_Image:{//任务成功
        IRayImage * pImg = (IRayImage*)pParam;
        const unsigned short * pImageData = pImg->pData;
        int nImageWidth = pImg->nWidth;
        int nImageHeight = pImg->nHeight;
        //            int nImageBytesPerPixel=pImg->nBytesPerPixel;
        //            int nImageSize = nImageWidth * nImageHeight * nImageBytesPerPixel;
        //            IRayVariantMapItem * pImageTagItem = pImg->propList.pItems;
        //            int nItemCnt = pImg->propList.nItemCount;
        //            while (nItemCnt--){
        //                //parse the image frame no
        //                if (Enm_ImageTag_FrameNo == pImageTagItem->nMapKey){
        //                    int nFrameNo = pImageTagItem->varMapVal.val.nVal;
        //                    break;
        //                }
        //                pImageTagItem++;
        //            }
        //            SaveFile(pImageData, nImageSize);

        img=new QImage((uchar*)pImageData, nImageWidth, nImageHeight, QImage::Format_Grayscale16);
        if(!img->isNull()){
            QDateTime curDateTime=QDateTime::currentDateTime();
            imageNum=curDateTime.toString("yyyyMMddhhmmss");
            if(dDriveState){//如果D盘存在，自动保存图片到D盘
                QString day=curDateTime.toString("yyyyMMdd");
                QString dirPath="D:/"+day;
                QDir qdir;
                if(!qdir.exists(dirPath)){
                    qdir.mkdir(dirPath);
                }
                QString path=dirPath+"/tiffImage"+imageNum+".tif";
                img->save(path);
            }
            emit imageOperation->imageCreateFinshed(img,imageNum);
        }
        //        QDateTime curDateTime=QDateTime::currentDateTime();
        //        QString path=QDir::currentPath() + "/image/tiffImage"+curDateTime.toString("yyyyMMddhhmmss")+".tif";
        //        bool flag = img.save(path);
        //        if(flag){
        //            emit imageOperation->imageSaveFinshed(path);
        //        }
        break;
    }
    default:
        break;
    }
}

void MainWindow::SaveFile(const void* pData, unsigned size)
{
    if (!pData || (0 == size)) return;
    HANDLE hFile = CreateFile(L"image.raw", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD nWriten = 0;
    WriteFile(hFile, pData, size, &nWriten, NULL);
    CloseHandle(hFile);
}


/**
 * @brief MainWindow::ConnectionFPD 连接探测器
 * @return
 */
int MainWindow::ConnectionFPD(){
    QString deb_ip_str = ui->IPaddr->text().mid(3);
    QString fpdDllPath="FpdSys";
    QString fpdWorkDir=QDir::currentPath() + "/" + SettingCfg::getInstance().getSystemSettingCfg().fpdWorkDir;
    //qDebug()<<"basepath="<<QCoreApplication::applicationDirPath();
    //qDebug()<<"fpdWorkDir=0"<<fpdWorkDir<<"0==="<<(fpdWorkDir=="");
    if(fpdWorkDir==""){
        return Err_OK;
    }
    int ret=0;
    if(!fpdCreateState){
        ret=fpd->Create(fpdDllPath,fpdWorkDir.toStdString().c_str(),MyCallbackHandler);
        if (ret!=Err_OK){
            //qDebug()<<"Create error="<<ret;
            fpd->GetErrorInfo(ret);
            return ret;
        }
        fpdCreateState=true;
    }
    DIY_LOG(LOG_INFO, "Create fpd ok.");

    QString str = ui->IPaddr->text().mid(3);
    //    QString str = ui->IPaddr->text().split('.').last();
    QByteArray IP = str.toLocal8Bit();
    ret=fpd->SetAttr(Cfg_HostIP, IP.data());
    if (ret!=Err_OK){
        DIY_LOG(LOG_ERROR, "SetAttr(Cfg_HostIP) error: %d", ret);
        fpd->GetErrorInfo(ret);
        return ret;
    }
    DIY_LOG(LOG_ERROR, "SetAttr(Cfg_HostIP) ok.");

    statusBar()->showMessage(QString("正在连接探测器..."));
    DIY_LOG(LOG_INFO, "SyncInvoke(Cmd_Connect, 30000)...");
    ret=fpd->SyncInvoke(Cmd_Connect,30000);
    if (ret!=Err_OK){
        //qDebug()<<"Cmd_Connect error="<<ret;
        fpd->GetErrorInfo(ret);
        statusBar()->showMessage(QString("探测器连接失败！"), 5000);
        return ret;
    }
    DIY_LOG(LOG_INFO, "SyncInvoke(Cmd_Connect,...) OK");

    //设置探测器触发模式
    AttrResult ar;
    ret=fpd->GetAttr(Attr_UROM_TriggerMode,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        disconnect_works();
        return ret;
    }
    DIY_LOG(LOG_INFO, "Get Attr_UROM_TriggerMode ok.");

    int trigger=SettingCfg::getInstance().getFpdSettingCfg().trigger;
    if(ar.nVal!=trigger){
        DIY_LOG(LOG_INFO, "Nedd to update trigger.");
        ret=fpd->SetAttr(Attr_UROM_TriggerMode_W,trigger);
        if (ret!=Err_OK){
            fpd->GetErrorInfo(ret);
            disconnect_works();
            return ret;
        }
        DIY_LOG(LOG_INFO, "Set Attr_UROM_TriggerMode ok.");

        ret=fpd->SyncInvoke(Cmd_WriteUserRAM,25000);
        if (ret!=Err_OK){
            fpd->GetErrorInfo(ret);
            disconnect_works();
            return ret;
        }
        DIY_LOG(LOG_INFO, "SyncInvoke Cmd_WriteUserRAM ok.");
    }
    //    T1= QDateTime::currentDateTime();
    //    ret=SetCalibrationOptions();//连接成功设置模版+
    //    if (ret!=Err_OK){
    //        fpd->GetErrorInfo(ret);
    //        return ret;
    //    }
    {
        AttrResult ar;
        int ret=fpd->GetAttr(Attr_State,ar);
        int state=ar.nVal;
        if(Enm_DetectorState::Enm_State_Ready == state)
        {
            statusBar()->showMessage(QString("探测器已连接！"), 5000);
        }
    }
    return ret;
}


/**
 * @brief MainWindow::DisconnectionFPD 断开探测器
 * @return
 */
int MainWindow::DisconnectionFPD(){
    QString fpdWorkDir=SettingCfg::getInstance().getSystemSettingCfg().fpdWorkDir;
    if(fpdWorkDir==""){
        return Err_OK;
    }
    int ret=fpd->SyncInvoke(Cmd_Disconnect,30000);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    ret=fpd->Destroy();
    if(ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    ret=fpd->FreeIRayLibrary();

    fpdCreateState=false;
    return ret;
}



/**
 * @brief MainWindow::PreOffsetCalibrationTemplate PreOffset校正模版
 * @return
 */
int MainWindow::PreOffsetCalibrationTemplate(){
    QDateTime T2= QDateTime::currentDateTime();//The last Offset generating time
    while(true){
        QDateTime T= QDateTime::currentDateTime();//获取当前时间
        if((!subsetSpecified&&T1.secsTo(T)>20*60) || (subsetSpecified&&T1.secsTo(T)>20*60&&T2.secsTo(T)>60*60)){
            AttrResult ar;
            int ret=fpd->GetAttr(Attr_State,ar);
            if (ret!=Err_OK){
                fpd->GetErrorInfo(ret);
                //return ret;
            }
            int state=ar.nVal;
            if(state!=Enm_DetectorState::Enm_State_Ready){
                statusBar()->showMessage(QString("不能进行PreOffset校正模版,FPD状态为:%1").arg(state), 5000);
                //return 2;
            }
            if(!subsetSpecified){
                ret=fpd->SyncInvoke(Cmd_SetCaliSubset,"Mode1",3000);
                if (ret!=Err_OK){
                    fpd->GetErrorInfo(ret);
                    //return ret;
                }
                subsetSpecified=true;
            }
            ret=fpd->SyncInvoke(Cmd_OffsetGeneration,3000);
            if (ret!=Err_OK){
                fpd->GetErrorInfo(ret);
                //return ret;
            }
            T2=QDateTime::currentDateTime();
        }
        Sleep(60*1000);//每分钟检查一次
    }

    return 0;
}

bool software_trigger_now = false;
/**
 * @brief MainWindow::SoftwareTrigger Software触发模式
 * @return
 */
int MainWindow::SoftwareTrigger(){
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    int state=ar.nVal;
    if(state!=Enm_DetectorState::Enm_State_Ready){
        statusBar()->showMessage(QString("FPD状态为:%1").arg(state), 5000);
        return 2;
    }
    ret=fpd->SyncInvoke(Cmd_Clear,3000);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
     //ControllerExposure();
    //Sleep(2000);//等待曝光结束
    //物理曝光完成后，调用Cmd_StartAcq采集图像数据，回调函数返回图像数据
    //ret=fpd->Invoke(Cmd_StartAcq);
    software_trigger_now = true;
    return ret;
}

void acq_image()
{
    if(fpd)
    {
        Sleep(6000);
        fpd->Invoke(Cmd_StartAcq);
    }
}

/**
 * @brief MainWindow::InnerTrigger Inner 触发模式适用于奕瑞静态探测器系列，探测器和高压发生器无联动信号，探
 * 测器内置曝光检测模块，能够检查曝光并自动完成采集动作。
 * @return
 */
int MainWindow::InnerTrigger(){
    int errorCode;
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        errorCode  = 1;
    }
    int state=ar.nVal;
    if(state!=Enm_DetectorState::Enm_State_Ready){
        statusBar()->showMessage(QString("FPD状态为:%1").arg(state), 5000);
        errorCode  = 2;
    }
    ret=fpd->Invoke(Cmd_StartAcq);
    if(ret!=Err_TaskPending){
        fpd->GetErrorInfo(ret);
        statusBar()->showMessage(QString("FPD错误代码为:%1").arg(ret), 5000);
        errorCode  = 3;
    }
    //  ControllerExposure();
    statusBar()->showMessage(QString("FPD错误代码为:%1").arg(errorCode), 5000);
    //探测器自动检测物理曝光，曝光后回调函数返回图像数据
    return ret;
}


/**
 * @brief MainWindow::OuterTrigger Outer 触发模式适用于奕瑞静态探测器系列的单帧采集，软件在 Ready 之后，用户
 * 只控制高压发生器出光即可完成图像采集任务，不需任何软件界面操作。探测器和高压
 * 发生器需要联动，包括探测器通知高压发生器曝光，以及发生器通知探测器曝光完成这
 * 两个同步电平信号的线缆连接。
 * @return
 */
int MainWindow::OuterTrigger(){
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    int state=ar.nVal;
    if(state!=Enm_DetectorState::Enm_State_Ready){
        statusBar()->showMessage(QString("FPD状态为:%1").arg(state), 5000);
        return 2;
    }
    //  ControllerExposure();  从外部曝光
    //探测器和高压发生器需要联动，包括探测器通知高压发生器曝光，以及发生器通知探测器曝光完成这两个同步电平信号的线缆连接。
    return ret;
}


/**
 * @brief MainWindow::FreesyncTrigger Freesync 触发模式适用于奕瑞静态探测器无线系列的单帧采集，软件在 Ready 之
 * 后，用户只需控制高压发生器出光，探测器内置曝光检测模块，不需任何软件界面操
 * 作，同时无需探测器和高压发生器联动。
 * @return
 */
int MainWindow::FreesyncTrigger(){
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    int state=ar.nVal;
    if(state!=Enm_DetectorState::Enm_State_Ready){
        statusBar()->showMessage(QString("FPD状态为:%1").arg(state), 5000);
        return 2;
    }
    //  ControllerExposure();
    //探测器和高压发生器需要联动，包括探测器通知高压发生器曝光，以及发生器通知探测器曝光完成这两个同步电平信号的线缆连接。
    return ret;
}


/**
 * @brief MainWindow::PREPTrigger PREP 触发模式适用于奕瑞静态探测器系列的单帧采集，特点和 Outer 模式相似；相
 * 对于 Outer 触发模式，探测器采用固定等待曝光时间的方式，从而节省了发生器通知探
 * 测器曝光完成这个环节；
 * @return
 */
int MainWindow::PREPTrigger(){
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    int state=ar.nVal;
    if(state!=Enm_DetectorState::Enm_State_Ready){
        statusBar()->showMessage(QString("FPD状态为:%1").arg(state), 5000);
        return 2;
    }
    ret=fpd->SetAttr(Cfg_ClearAcqParam_DelayTime,3000);//清空和采图之间的时间间隔
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    ret=fpd->Invoke(Cmd_ClearAcq);
    if(ret!=Err_TaskPending){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    //物理曝光后，到达设置的延迟时间，探测器自动回调函数返回图像数据
    // ControllerExposure();
    return ret;
}


/**
 * @brief MainWindow::ControllerExposure 控制下位机曝光
 */
void MainWindow::ControllerExposure(){
    QVector<quint16> qv;
    qv.append(2);
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    bool w_ret;
    w_ret = controller->writeData(Enm_Controller_Address::ExposureStart,1,serverAddress,qv);
    DIY_LOG(LOG_INFO, "controller->writeData ret: %d", w_ret);
    exposureStatus=0;//重置为下位机未开始曝光
    if(!readExposureStatusTimer->isActive()){
        readExposureStatusTimer->setInterval(1000);
        readExposureStatusTimer->start();
    }
}


/**
 * @brief MainWindow::SetCalibrationOptions 设置Offset、Gain、Defect校正选项
 * @return
 */
int MainWindow::SetCalibrationOptions(){
    AttrResult ar;
    int ret=fpd->GetAttr(Attr_State,ar);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    FpdSettingCfg fpdSettingCfg=SettingCfg::getInstance().getFpdSettingCfg();
    ret=fpd->SyncInvoke(Cmd_SetCorrectOption,fpdSettingCfg.offsetCorrectOption,1000);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    ret=fpd->SyncInvoke(Cmd_SetCorrectOption,fpdSettingCfg.gainCorrectOption,1000);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    ret=fpd->SyncInvoke(Cmd_SetCorrectOption,fpdSettingCfg.defectCorrectOption,1000);
    if (ret!=Err_OK){
        fpd->GetErrorInfo(ret);
        return ret;
    }
    return ret;
}


/**
 * @brief MainWindow::keyPressEvent 监听键盘事件
 * @param k
 */
void MainWindow::keyPressEvent(QKeyEvent * k){
    qDebug()<<"键盘事件："<<k->key();
    if(k->key()==Qt::Key_Pause && fpdConnectState==Enm_Connect_State::Connected){//暂停键
        on_exposure_clicked();
    }else if(k->key()==Qt::Key_VolumeUp && controllerConnectState==Enm_Connect_State::Connected){//音量+
        on_exposure_clicked();
        // on_addTime_clicked();
    }else if(k->key()==Qt::Key_VolumeDown && controllerConnectState==Enm_Connect_State::Connected){//音量-
        // on_subTime_clicked();
    }else if(k->key()==Qt::Key_D){
        //ui->preview->brightAddImage();
    }else if(k->key()==Qt::Key_A){
        //ui->preview->brightSubImage();
    }else if(k->key()==Qt::Key_W){
        //ui->preview->contrastAddImage();
    }else if(k->key()==Qt::Key_S){
        //ui->preview->contrastSubImage();
    }else if(k->key()==Qt::Key_Plus){
        ui->preview->OnZoomInImage();
    }else if(k->key()==Qt::Key_Minus){
        ui->preview->OnZoomOutImage();
    }
}

FPDRESULT MainWindow::disconnect_works(bool conn_then)
{
    DIY_LOG(LOG_INFO, "on_connect_clicked: now is connected, begin to disconnect...");
    FPDRESULT ret=DisconnectionFPD();
    if(false == conn_then)
    {
        if(ret==0){
            onFpdConnectStateChanged(Enm_Connect_State::Disconnected);
            if(readFpdBatteryLevelTimer->isActive()){
                readFpdBatteryLevelTimer->stop();
            }
        }
    }
    return ret;
}

/**
 * @brief MainWindow::on_connect_clicked 连接按钮槽函数（仅连接探测器）
 */
void MainWindow::on_connect_clicked(){
    if(fpd){
        if(fpdConnectState==Enm_Connect_State::Disconnected){
            DIY_LOG(LOG_INFO, "on_connect_clicked: now is disconnected, begin to connect...");
            FPDRESULT ret=ConnectionFPD();
            if(ret==0){
                onFpdConnectStateChanged(Enm_Connect_State::Connected);
                onReadFpdBatteryLevelTimerOutTime();//第一次获得探测器电量
                if(!readFpdBatteryLevelTimer->isActive()){
                    readFpdBatteryLevelTimer->setInterval(60*1000);
                    readFpdBatteryLevelTimer->start();
                }
            }
        }else{
            disconnect_works();
        }
    }
}


/**
 * @brief MainWindow::onFpdConnectStateChanged 探测器状态变化
 * @param state 状态码
 */
void MainWindow::onFpdConnectStateChanged(int state){
    fpdConnectState=state;
    if(state==Enm_Connect_State::Connected){//已连接
        statusBar()->showMessage("探测器已连接", 5000);
        ui->connect->setStyleSheet("border-image: url(:/images/disconnect.png)");
        if(controllerConnectState==Enm_Connect_State::Connected){
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-able.png)");
            ui->exposure->setEnabled(true);
        }
        ui->systemSetting->setStyleSheet("border-image: url(:/images/systemSetting-disable.png)");
        ui->systemSetting->setEnabled(false);
        ui->fpdSetting->setStyleSheet("border-image: url(:/images/fpdSetting-disable.png)");
        ui->fpdSetting->setEnabled(false);
    }else if(state==Enm_Connect_State::Connecting){
        statusBar()->showMessage("探测器连接中", 5000);
        //ui->connect->setText("连接中");
        ui->connect->setEnabled(false);//设置按钮不可用
    }else if(state==Enm_Connect_State::Disconnected){//已断开
        statusBar()->showMessage("探测器已断开", 5000);
        ui->connect->setStyleSheet("border-image: url(:/images/connect.png)");
        ui->exposure->setStyleSheet("border-image: url(:/images/exposure-disable.png)");
        ui->exposure->setEnabled(false);
        ui->systemSetting->setStyleSheet("border-image: url(:/images/systemSetting-able.png)");
        ui->systemSetting->setEnabled(true);
        ui->fpdSetting->setStyleSheet("border-image: url(:/images/fpdSetting-able.png)");
        ui->fpdSetting->setEnabled(true);
    }else if(state==Enm_Connect_State::Disconnecting){//断开中
        statusBar()->showMessage("探测器断开中", 5000);
        ui->connect->setEnabled(false);//设置按钮不可用
    }else{

    }
    QString fpdWorkDir=SettingCfg::getInstance().getSystemSettingCfg().fpdWorkDir;
    if(fpdWorkDir==""){
        if(controllerConnectState==Enm_Connect_State::Connected){
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-able.png)");
            ui->exposure->setEnabled(true);
        }else{
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-disable.png)");
            ui->exposure->setEnabled(false);
        }
    }
}


/**
 * @brief MainWindow::onControllerConnectStateChanged 下位机控制器状态变化
 * @param state 状态码
 */
void MainWindow::onControllerConnectStateChanged(int state){
    controllerConnectState=state;
    if(state==Enm_Connect_State::Connected){//已连接
        statusBar()->showMessage("下位机控制器已连接", 5000);
        if(fpdConnectState==Enm_Connect_State::Connected){
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-able.png)");
            ui->exposure->setEnabled(true);
        }
        ui->range->setStyleSheet("border-image: url(:/images/range-able.png)");
        ui->range->setEnabled(true);
        ui->addTime->setStyleSheet("border-image: url(:/images/timeAdd-able.png)");
        ui->addTime->setEnabled(true);
        ui->subTime->setStyleSheet("border-image: url(:/images/timeSub-able.png)");
        ui->subTime->setEnabled(true);
    }else if(state==Enm_Connect_State::Connecting){
        statusBar()->showMessage("下位机控制器连接中", 5000);
    }else if(state==Enm_Connect_State::Disconnected){//已断开
        statusBar()->showMessage("下位机控制器已断开", 5000);
        ui->exposure->setStyleSheet("border-image: url(:/images/exposure-disable.png)");
        ui->exposure->setEnabled(false);
        ui->range->setStyleSheet("border-image: url(:/images/range-disable.png)");
        ui->range->setEnabled(false);
        ui->addTime->setStyleSheet("border-image: url(:/images/timeAdd-disable.png)");
        ui->addTime->setEnabled(false);
        ui->subTime->setStyleSheet("border-image: url(:/images/timeSub-disable.png)");
        ui->subTime->setEnabled(false);
    }else if(state==Enm_Connect_State::Disconnecting){//断开中
        statusBar()->showMessage("下位机控制器断开中", 5000);
    }else{

    }
    QString fpdWorkDir=SettingCfg::getInstance().getSystemSettingCfg().fpdWorkDir;
    if(fpdWorkDir==""){
        if(controllerConnectState==Enm_Connect_State::Connected){
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-able.png)");
            ui->exposure->setEnabled(true);
        }else{
            ui->exposure->setStyleSheet("border-image: url(:/images/exposure-disable.png)");
            ui->exposure->setEnabled(false);
        }
    }
}


/**
 * @brief MainWindow::onErrorOccurred 错误信息展示
 * @param errorInfo 错误信息
 */
void MainWindow::onErrorOccurred(QString errorInfo){
    statusBar()->clearMessage();
    statusBar()->showMessage(errorInfo, 5000);
}


/**
 * @brief MainWindow::onReadControllerDataFinished 读取下位机控制器中的数据完成
 * @param map 返回的值
 */
void MainWindow::onReadControllerDataFinished(QMap<int, quint16> map){

    quint16 exposureTime=NULL;
    quint16 batteryVoltmeter=NULL;
    quint16 oilBoxTemperature=NULL;
    quint16 baudRate=NULL;
    quint16 serverAddress=NULL;
    QMap<int, quint16>::Iterator iter = map.begin();
    while (iter != map.end()) {
        switch (iter.key()) {
        case Enm_Controller_Address::BaudRate://波特率
        {
            baudRate=iter.value();
        }
            break;
        case Enm_Controller_Address::ServerAddress://设备地址
        {
            serverAddress=iter.value();
        }
            break;

        case Enm_Controller_Address::ExposureTime://曝光时间低位
        {
            exposureTime=iter.value();
            int loop = 0;
            for(;loop<40; loop++)
            {
                if((exposureTimeList[loop] * 1000) == exposureTime)
                    break;
            }
            if(exposureTimeIndex != loop)
                exposureTimeIndex = loop;
        }
            break;
        case Enm_Controller_Address::Voltmeter://管电压
        {
            ui->voltmeter->setText(QString("%1").arg(iter.value()));
        }
            break;
        case Enm_Controller_Address::Ammeter://管电流
        {
            ui->ammeter->setText(QString("%1").arg(iter.value()));
        }
            break;
        case Enm_Controller_Address::RangeIndicationStatus://范围指示状态
        {
            //                if(rangeStatus!=iter.value()){//状态发生改变
            //                    rangeStatus=iter.value();
            //                    if(rangeStatus==1){
            //                        statusBar()->showMessage("范围指示已启动", 5000);
            //                        ui->range->setEnabled(false);
            //                    }else{
            //                        statusBar()->showMessage("范围指示已停止", 5000);
            //                        ui->range->setEnabled(true);
            //                    }
            //                }
            if(iter.value()==1){
                ui->range->setChecked(true);
                ui->range->setStyleSheet("border-image: url(:/images/range-able-check.png)");
                statusBar()->showMessage("范围指示已启动", 5000);
            }else{
                ui->range->setChecked(false);
                ui->range->setStyleSheet("border-image: url(:/images/range-able.png)");
                statusBar()->showMessage("范围指示已停止", 5000);
            }
        }
            break;
        case Enm_Controller_Address::ExposureStatus://曝光状态
        {
            exposureStatus=iter.value();
            if(exposureStatus==1){
                statusBar()->showMessage("下位机曝光中", 5000);
            }else{
                statusBar()->showMessage("下位机曝光已结束", 5000);
            }
        }
            break;
        case Enm_Controller_Address::RangeIndicationStart://范围指示启动
        {}
            break;
        case Enm_Controller_Address::ExposureStart://曝光启动
        {}
            break;
        case Enm_Controller_Address::BatteryLevel://电池电量   使用下位计算的电量
        {

            ui->batteryLevel->setText(QString("%1%").arg(iter.value()));//上位计算
            if(iter.value()<=100 && iter.value()>70){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#90EE90}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-green.png)");
            }else if(iter.value()<=70 && iter.value()>20){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#FFA500}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-orange.png)");
            }else if(iter.value()<=20 && iter.value()>=0){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#FF0000}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-red.png)");
            }
        }
            break;

        case Enm_Controller_Address::BatteryVoltmeter://电池电压   可上位计算电量
        {
            //int batteryLevel=0;
            int i=0;
            int index=-1;
            while(i<13){
                if(iter.value()>=(voltageList[i] * 6)){
                    //batteryLevel=batteryLevelList[i];
                    index=i;
                }
                i++;
            }
            int batteryVal=0;
            if(index>-1 && index<12){
                int startV=voltageList[index] * 6;
                int endV=voltageList[index+1] * 6;
                int startL=batteryLevelList[index];
                int endL=batteryLevelList[index+1];
                batteryVal=startL+((iter.value()-startV*6)/((endV-startV)*6/(float)(endL-startL)));
            }else if(index==12){
                batteryVal=100;
            }

            //ui->batteryLevel->setValue(iter.value());
            ui->batteryLevel->setText(QString("%1%").arg(batteryVal));
            if(batteryVal<=100 && batteryVal>70){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#90EE90}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-green.png)");
            }else if(batteryVal<=70 && batteryVal>20){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#FFA500}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-orange.png)");
            }else if(batteryVal<=20 && batteryVal>=0){
                //ui->batteryLevel->setStyleSheet("QProgressBar::chunk{background:#FF0000}");
                ui->batteryLevel->setStyleSheet("border-image: url(:/images/electric-red.png)");
            }
            SystemSettingCfg ssc=SettingCfg::getInstance().getSystemSettingCfg();
            if(!ssc.isAutoOff)
                break;
            if(iter.value()>(3200*6) && iter.value()<=(3700*6)){
                //对于3.7v低电量提示
                if(!promptState){
                    promptState=true;
                    maskWidget->show();
                    lowBatteryWarning->startShutdownTimer();
                    lowBatteryWarning->exec();
                }
            }else if(iter.value()<=(3200*6)){
                //低于3.2v强制关机
                system("shutdown -s -t 00");
                this->close();
            }

        }
            break;

        case Enm_Controller_Address::OilBoxTemperature://油盒温度低位
        {
            oilBoxTemperature=iter.value();
        }
            break;
        case Enm_Controller_Address::State://充能状态
        {

            if(iter.value() == 0){//正常
                ui->chargeState->setStyleSheet("border-image: url(:/images/green.png)");
                ui->chargeState->setText("");
            }else{
                ui->chargeState->setStyleSheet("border-image: url(:/images/red.png)");
                switch(iter.value())
                {
                case 0:
                    ui->chargeState->setText("");
                    break;
                case 1:
                    ui->chargeState->setText("高温");
                    break;
                case 2:
                    ui->chargeState->setText("低温");
                    break;
                case 3:
                    ui->chargeState->setText("内部错误");
                    break;
                case 4:
                    ui->chargeState->setText("电压低");
                    break;
                }

            }
        }
            break;
        default:
            break;
        }
        ++iter;
    }
    //    if(exposureTimeHigh && exposureTimeLow){
    //        quint32 sum = (exposureTimeHigh << 16)+(exposureTimeLow & 0xFFFF);
    //        float exposureTime=*((float*)&sum);
    //    }
    //    if(batteryVoltmeterHigh && batteryVoltmeterLow){
    //        quint32 sum = (batteryVoltmeterHigh << 16)+(batteryVoltmeterLow & 0xFFFF);
    //        float batteryVoltmeter=*((float*)&sum);
    //    }
    //    if(oilBoxTemperatureHigh && oilBoxTemperatureLow){
    //        quint32 sum = (oilBoxTemperatureHigh << 16)+(oilBoxTemperatureLow & 0xFFFF);
    //        float oilBoxTemperature=*((float*)&sum);
    //    }

    if(baudRate || serverAddress){
        SystemSettingCfg ssc=SettingCfg::getInstance().getSystemSettingCfg();
        FpdSettingCfg fsc=SettingCfg::getInstance().getFpdSettingCfg();
        if(baudRate){
            ssc.serialBaudRate=baudRate;
        }
        if(serverAddress){
            ssc.serverAddress=serverAddress;
        }
        SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
    }
}


/**
 * @brief MainWindow::on_exposure_clicked 曝光按钮槽函数
 */
void MainWindow::on_exposure_clicked(){
    //    float f=0.03;
    //    qDebug()<<"&f="<<&f;
    //    qDebug()<<"&f+1="<<&f+1;
    //    qDebug()<<"(quint16*)&f="<<(quint16*)&f;
    //    qDebug()<<"(quint16*)&f+1="<<(quint16*)&f+1;
    //    quint16 low = *((quint16*)&f);
    //    qDebug()<<"low="<<low;
    //    quint16 high = *((quint16*)&f+1);
    //    qDebug()<<"high="<<high;
    //    quint32 sum = (high << 16)+(low & 0xFFFF);
    //    float ff=*((float*)&sum);
    //    qDebug()<<"ff="<<ff;
    ui->exposure->setStyleSheet("border-image: url(:/images/exposure-disable.png)");
    ui->exposure->setEnabled(false);
    QString fpdWorkDir=SettingCfg::getInstance().getSystemSettingCfg().fpdWorkDir;
    if(fpdWorkDir==""){
        ControllerExposure();
        statusBar()->showMessage("已曝光", 5000);
    }else{
        int trigger=SettingCfg::getInstance().getFpdSettingCfg().trigger;
        if(trigger==Enm_TriggerMode::Enm_TriggerMode_Soft){
            SoftwareTrigger();
            DIY_LOG(LOG_INFO, "Soft trigger exposure");
        }else if(trigger==Enm_TriggerMode::Enm_TriggerMode_Inner){
            InnerTrigger();
        }else if(trigger==Enm_TriggerMode::Enm_TriggerMode_Outer){
            OuterTrigger();
        }else if(trigger==Enm_TriggerMode::Enm_TriggerMode_Prep){
            PREPTrigger();
        }else if(trigger==Enm_TriggerMode::Enm_TriggerMode_FreeSync){
            FreesyncTrigger();
        }
        ControllerExposure();
        statusBar()->showMessage("开始采集图像", 5000);
    }
    ui->exposure->setStyleSheet("border-image: url(:/images/exposure-able.png)");
    ui->exposure->setEnabled(true);
}


/**
 * @brief MainWindow::on_addTime_clicked 曝光时间+按钮槽函数
 */
void MainWindow::on_addTime_clicked(){
    if(0>exposureTimeIndex || (MAXSTEP-1)<=exposureTimeIndex){
        return;
    }
    exposureTimeIndex++;
    writeExposureTime();
}


/**
 * @brief MainWindow::on_subTime_clicked 曝光时间-按钮槽函数
 */
void MainWindow::on_subTime_clicked(){
    if(0 >= exposureTimeIndex || MAXSTEP<exposureTimeIndex){
        return;
    }
    exposureTimeIndex--;
    writeExposureTime();
}


/**
 * @brief MainWindow::writeExposureTime 给下位机写入曝光时间
 */
void MainWindow::writeExposureTime(){
    float es=exposureTimeList[exposureTimeIndex];
    //quint16 low = *((quint16*)&es);
    //quint16 high = *((quint16*)&es+1);
    QVector<quint16> qv;
    //qv.append(high);
    qv.append((quint16)(1000 * es));
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    bool writeSuccess=controller->writeData(Enm_Controller_Address::ExposureTime,1,serverAddress,qv);
    if(writeSuccess){
        ui->exposureSetting->setText(QString("%1").arg(es));
        //更新曝光时间的下标到配置文件
        SystemSettingCfg ssc=SettingCfg::getInstance().getSystemSettingCfg();
        FpdSettingCfg fsc=SettingCfg::getInstance().getFpdSettingCfg();
        ssc.exposureTimeIndex=exposureTimeIndex;
        SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
    }
}

void MainWindow::writeExposurekV(int kV)
{
    QVector<quint16> qv;
    qv.append(kV);
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    bool writeSuccess=controller->writeData(Enm_Controller_Address::VoltSet,1,serverAddress,qv);
    if(writeSuccess){
        ui->volSet->setText(QString("%1").arg(kV));
        ui->volSet->setStyleSheet("color: rgb(0, 255, 0)");
    }
}

void MainWindow::writeExposuremA(int ua)
{

    QVector<quint16> qv;
    qv.append(ua);
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    bool writeSuccess=controller->writeData(Enm_Controller_Address::FilamentSet,1,serverAddress,qv);
    if(writeSuccess){
        ui->amSet->setText(QString("%1").arg(ua));
        ui->amSet->setStyleSheet("color: rgb(0, 255, 0)");
    }
}

/**
 * @brief MainWindow::dDriveExist 判断D盘是否存在
 * @return
 */
bool MainWindow::dDriveExist()
{
    foreach (QFileInfo my_info, QDir::drives())
    {
        //        qDebug()<<my_info.absoluteFilePath();
        if(my_info.absoluteFilePath()=="D:/"){
            return true;
        }
    }
    return false;
}


/**
 * @brief MainWindow::on_range_clicked 范围指示启动
 */
void MainWindow::on_range_clicked(){
    bool checkedFlag=ui->range->isChecked();
    int val=checkedFlag?1:0;
    QVector<quint16> qv;
    qv.append(val);
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->writeData(Enm_Controller_Address::RangeIndicationStart,1,serverAddress,qv);
    //rangeStatus=0;//重置为未启动
    if(!readRangeStatusTimer->isActive()){
        readRangeStatusTimer->setInterval(1000);
        readRangeStatusTimer->start();
    }
}


/**
 * @brief MainWindow::onReadExposureStatusTimerOutTime 定时器超时调用读取曝光状态
 */
void MainWindow::onReadExposureStatusTimerOutTime(){
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->readData(Enm_Controller_Address::ExposureStatus,1,serverAddress);
}


/**
 * @brief MainWindow::onReadRangeStatusTimerOutTime 定时器超时调用读取范围指示状态
 */
void MainWindow::onReadRangeStatusTimerOutTime(){
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->readData(Enm_Controller_Address::RangeIndicationStatus,1,serverAddress);
}


/**
 * @brief MainWindow::onReadVoltmeterAndAmmeterTimerOutTime 定时器超时调用读取管电压、管电流
 */
void MainWindow::onReadVoltmeterAndAmmeterTimerOutTime(){
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->readData(Enm_Controller_Address::Voltmeter,2,serverAddress);//读取电流电压
}


/**
 * @brief MainWindow::onReadBatteryLevelTimerOutTime 定时器超时调用读取电池电量
 */
void MainWindow::onReadBatteryLevelTimerOutTime(){
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->readData(Enm_Controller_Address::BatteryLevel,1,serverAddress);//读取电池电量
}

/**
 * @brief MainWindow::onReadChargeStateTimerOutTime 定时器超时调用读取充能状态
 */
void MainWindow::onReadChargeStateTimerOutTime(){
    int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
    controller->readData(Enm_Controller_Address::State,1,serverAddress);//读取充能状态
}


/**
 * @brief MainWindow::onCheckSleepAndShutdownTimerOutTime 定时器超时检查睡眠和关机时间是否超时
 */
void MainWindow::onCheckSleepAndShutdownTimerOutTime(){
    QDateTime currentTime= QDateTime::currentDateTime();//获取当前时间
    QString sleepTime=SettingCfg::getInstance().getSystemSettingCfg().sleepTime;
    QString shutdownTime=SettingCfg::getInstance().getSystemSettingCfg().shutdownTime;
    if(sleepTime!="off"){
        int st=sleepTime.toInt();
        if(!sleepState && timestamp.secsTo(currentTime)>(st*60)){
            //qDebug()<<"==进入睡眠模式==";
            //SetSystemPowerState(false,true);
            sleepState=SetSuspendState(true,true,false);
        }
    }
    if(shutdownTime!="off"){
        int st=shutdownTime.toInt();
        if(timestamp.secsTo(currentTime)>(st*60)){
            //qDebug()<<"==自动关机==";
            system("shutdown -s -t 00");
        }
    }
}


/**
 * @brief MainWindow::onImageSaveFinshed 当图片保存完毕时
 * @param path 图片路径
 */
void MainWindow::onImageSaveFinshed(QString path){
    if((showImg->load(path))){
        QMatrix matrix;
        matrix.rotate(90);
        *showImg = showImg->transformed(matrix);
        ui->preview->loadImage(*showImg);
        brightSlider->setValue(0);
        contrastSlider->setValue(0);
    }
}

void MainWindow::onImageCreateFinshed(QImage *img,QString imageNum)
{
    showImg=img;
    QMatrix matrix;
    matrix.rotate(90);
    *showImg = showImg->transformed(matrix);
    ui->preview->loadImage(*showImg);
    // brightSlider->setValue(0);
    // contrastSlider->setValue(0);
    ui->ImageNum->setText("SN:" + imageNum);
}

void MainWindow::onKeyOrMouseEventOccurred(){
    //qDebug()<<"==key Or Mouse==";
    timestamp=QDateTime::currentDateTime();//时间戳重置为当前操作时间
    sleepState=false;
}

/**
 * @brief MainWindow::on_exitSystem_clicked 点击退出按钮
 */
void MainWindow::on_exitSystem_clicked()
{
    maskWidget->show();
    exitSystem->exec();
}

/**
 * @brief MainWindow::onQDialogClosed 关闭半透明层
 */
void MainWindow::onQDialogClosed()
{
    maskWidget->close();
}


/**
 * @brief MainWindow::on_systemSetting_clicked 点击系统设置按钮
 */
void MainWindow::on_systemSetting_clicked()
{
    maskWidget->show();
    systemSetting->exec();
}


/**
 * @brief MainWindow::on_fpdSetting_clicked 点击探测器设置按钮
 */
void MainWindow::on_fpdSetting_clicked()
{
    maskWidget->show();
    fpdSetting->updateTriggerMode();
    fpdSetting->exec();
}
/**
 * @brief MainWindow::onConnectFpdAndController 连接探测器和下位机控制器
 */
void MainWindow::onConnectFpdAndController()
{
    if(fpd){
        if(fpdConnectState==Enm_Connect_State::Disconnected){
            FPDRESULT ret=ConnectionFPD();
            if(ret==0){
                onFpdConnectStateChanged(Enm_Connect_State::Connected);
                onReadFpdBatteryLevelTimerOutTime();//第一次获得探测器电量
                if(!readFpdBatteryLevelTimer->isActive()){
                    readFpdBatteryLevelTimer->setInterval(60*1000);
                    readFpdBatteryLevelTimer->start();
                }
            }
        }else{
            //先断开探测器，然后重新连接
            FPDRESULT ret;
            ret = disconnect_works(true);
            if(ret==0){
                onFpdConnectStateChanged(Enm_Connect_State::Disconnected);
                if(readFpdBatteryLevelTimer->isActive()){
                    readFpdBatteryLevelTimer->stop();
                }
                ret=ConnectionFPD();
                if(ret==1){
                    onFpdConnectStateChanged(Enm_Connect_State::Connected);
                    if(!readFpdBatteryLevelTimer->isActive()){
                        readFpdBatteryLevelTimer->setInterval(60*1000);
                        readFpdBatteryLevelTimer->start();
                    }
                }
            }
        }
    }
    if(controller){
        if(controllerConnectState==Enm_Connect_State::Disconnected){
            ConnectionControllerAndSetting();
        }else{
            //先断开下位机控制器，然后重新连接
            int ret=controller->DisconnectionController();
            if(ret==0){
                onControllerConnectStateChanged(Enm_Connect_State::Disconnected);
                //断开后停止定时器
                if(readExposureStatusTimer->isActive()){
                    readExposureStatusTimer->stop();
                }
                if(readRangeStatusTimer->isActive()){
                    readRangeStatusTimer->stop();
                }
                if(readVoltmeterAndAmmeterTimer->isActive()){
                    readVoltmeterAndAmmeterTimer->stop();
                }
                if(readBatteryLevelTimer->isActive()){
                    readBatteryLevelTimer->stop();
                }
                if(readChargeStateTimer->isActive()){
                    readChargeStateTimer->stop();
                }
                ConnectionControllerAndSetting();
            }
        }
    }
}

/**
 * @brief MainWindow::ConnectionControllerAndSetting 连接下位机控制器并且设置数据
 * @return 0 成功  1失败
 */
int MainWindow::ConnectionControllerAndSetting(){
    int ret=controller->ConnectionController();
    if(ret==0){
        onControllerConnectStateChanged(Enm_Connect_State::Connected);
        writeExposureTime();//连接成功后写入曝光时间
        if(!readVoltmeterAndAmmeterTimer->isActive()){
            readVoltmeterAndAmmeterTimer->setInterval(1000);
            readVoltmeterAndAmmeterTimer->start();
        }
        //第一次读取电量，连接后立即读取
        int serverAddress=SettingCfg::getInstance().getSystemSettingCfg().serverAddress;
        controller->readData(Enm_Controller_Address::BatteryLevel,1,serverAddress);//读取电池电量
        if(!readBatteryLevelTimer->isActive()){
            readBatteryLevelTimer->setInterval(60*1000);
            readBatteryLevelTimer->start();
        }
        if(!readChargeStateTimer->isActive()){
            readChargeStateTimer->setInterval(1000);
            readChargeStateTimer->start();
        }
    }
    return ret;
}


/**
 * @brief MainWindow::on_saveImage_clicked 保存图片
 */
void MainWindow::on_saveImage_clicked()
{
    QString dir=SettingCfg::getInstance().getSystemSettingCfg().imageDir;
    QDateTime curDateTime=QDateTime::currentDateTime();
    QString imageName="Image"+imageNum+".tif";
    QString path=dir+imageName;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("图片保存"),
                                                    path,
                                                    tr("Image Files (*.tif)"));

    if (!fileName.isNull())
    {
        //fileName是文件名
        QString dirTemp=fileName.mid(0,fileName.lastIndexOf("/")+1);
        if(dirTemp!=dir){
            SystemSettingCfg ssc=SettingCfg::getInstance().getSystemSettingCfg();
            ssc.imageDir=dirTemp;
            FpdSettingCfg fsc=SettingCfg::getInstance().getFpdSettingCfg();
            SettingCfg::getInstance().writeSettingConfig(&ssc,&fsc);
        }
        if(!ui->preview->saveImage(fileName))
        {
            statusBar()->showMessage("图片保存失败", 2000);
        }else{
            statusBar()->showMessage("图片保存成功", 2000);
        }
    }
    else
    {
        //点的是取消
    }
}

/**
 * @brief MainWindow::onRotateButtonClicked 旋转图片
 */
void MainWindow::onRotateButtonClicked()
{
    ui->preview->rotateImage();
}

/**
 * @brief MainWindow::onWWWLButtonClicked 窗宽窗位
 * @param checked
 */
void MainWindow::onWWWLButtonClicked(bool checked)
{
    if(checked){
        ui->preview->operateImage(Enm_OperateType::Enm_WWWL_Operat);
        wwwlButton->setStyleSheet("border-image: url(:/images/wwwl_checked.png)");
        zoomButton->setChecked(false);
        zoomButton->setStyleSheet("border-image: url(:/images/zoom.png)");
        translationButton->setChecked(false);
        translationButton->setStyleSheet("border-image: url(:/images/translation.png)");
    }else{
        wwwlButton->setStyleSheet("border-image: url(:/images/wwwl.png)");
        ui->preview->operateImage(Enm_OperateType::Enm_No_Operat);
    }
}

/**
 * @brief MainWindow::onAntiColorButtonClicked 反色
 * @param checked
 */
void MainWindow::onAntiColorButtonClicked(bool checked)
{
    ui->preview->antiColorImage(checked);
    if(checked){
        antiColorButton->setStyleSheet("border-image: url(:/images/invert_checked.png)");
    }else{
        antiColorButton->setStyleSheet("border-image: url(:/images/invert.png)");
    }
}

/**
 * @brief MainWindow::onZoomButtonClicked 缩放
 * @param checked
 */
void MainWindow::onZoomButtonClicked(bool checked)
{
    if(checked){
        ui->preview->operateImage(Enm_OperateType::Enm_Zoom_Operat);
        zoomButton->setStyleSheet("border-image: url(:/images/zoom_checked.png)");
        wwwlButton->setChecked(false);
        wwwlButton->setStyleSheet("border-image: url(:/images/wwwl.png)");
        translationButton->setChecked(false);
        translationButton->setStyleSheet("border-image: url(:/images/translation.png)");
    }else{
        ui->preview->operateImage(Enm_OperateType::Enm_No_Operat);
        zoomButton->setStyleSheet("border-image: url(:/images/zoom.png)");
    }
}

/**
 * @brief MainWindow::onTranslationButtonClicked 平移
 * @param checked
 */
void MainWindow::onTranslationButtonClicked(bool checked)
{
    if(checked){
        ui->preview->operateImage(Enm_OperateType::Enm_Translation_Operate);
        translationButton->setStyleSheet("border-image: url(:/images/translation_checked.png)");
        wwwlButton->setChecked(false);
        wwwlButton->setStyleSheet("border-image: url(:/images/wwwl.png)");
        zoomButton->setChecked(false);
        zoomButton->setStyleSheet("border-image: url(:/images/zoom.png)");
    }else{
        ui->preview->operateImage(Enm_OperateType::Enm_No_Operat);
        translationButton->setStyleSheet("border-image: url(:/images/translation.png)");
    }
}

void MainWindow::onRestButtonClicked()
{
    wwwlButton->setChecked(false);
    wwwlButton->setStyleSheet("border-image: url(:/images/wwwl.png)");
    antiColorButton->setChecked(false);
    antiColorButton->setStyleSheet("border-image: url(:/images/invert.png)");
    zoomButton->setChecked(false);
    zoomButton->setStyleSheet("border-image: url(:/images/zoom.png)");
    translationButton->setChecked(false);
    translationButton->setStyleSheet("border-image: url(:/images/translation.png)");
    ui->preview->resetImage();
}

/**
 * @brief MainWindow::onContrastSubClicked 对比度-
 */
void MainWindow::onContrastSubClicked()
{
    int v=contrastSlider->value()-5;
    contrastSlider->setValue(v);
    //ui->preview->contrastSubImage();
}

/**
 * @brief MainWindow::onContrastAddClicked 对比度+
 */
void MainWindow::onContrastAddClicked()
{
    int v=contrastSlider->value()+5;
    contrastSlider->setValue(v);
    //ui->preview->contrastAddImage();
}

/**
 * @brief MainWindow::onBrightSubClicked 亮度-
 */
void MainWindow::onBrightSubClicked()
{
    int v=brightSlider->value()-10;
    brightSlider->setValue(v);
    //ui->preview->brightSubImage();
}

/**
 * @brief MainWindow::onBrightAddClicked 亮度+
 */
void MainWindow::onBrightAddClicked()
{
    int v=brightSlider->value()+10;
    brightSlider->setValue(v);
    //ui->preview->brightAddImage();
}

void MainWindow::onContrastChanged(int value)
{
    ui->preview->setContrastImage((float)value/100.00);
}

void MainWindow::onBrightChanged(int value)
{
    ui->preview->setBrightImage((float)value/100.00);
}

/**
 * @brief MainWindow::onNewConnectioned 收到新的连接
 */
void MainWindow::onNewConnectioned()
{
    //    mTCPSocket = mTCPServer->nextPendingConnection();
    //    connect(mTCPSocket, SIGNAL(readyRead()), this, SLOT(onReadyReaded()));
    mWebSocket=mWebSocketServer->nextPendingConnection();
    mWebSocket->sendTextMessage("connect success");
    connect(mWebSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
}

/**
 * @brief MainWindow::onReadyReaded socket数据已经读完
 */
//void MainWindow::onReadyReaded()
//{
//    QByteArray ba = mTCPSocket->readAll();
//    QJsonDocument jsonDocument = QJsonDocument::fromJson(ba);
//    const char *data;
//    if( jsonDocument.isNull() ){
//        data="error:no json";
//    }
//    QJsonObject jsonObject = jsonDocument.object();
//    int exp=jsonObject.value("exposure").toInt();
//    if(exp==1){
//        if(ui->exposure->isEnabled()){
//            on_exposure_clicked();
//            data="success:exposure success";
//        }else{
//           data="error:no exposure,button not enabled";
//        }
//    }else{
//        data="error:wrong json data";
//    }
//        mTCPSocket->write(data);
//}

void MainWindow::onTextMessageReceived(QString message)
{
    //qDebug()<<message;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toLocal8Bit().data());
    QString data;
    if( jsonDocument.isNull() ){
        data="error:no json";
    }
    QJsonObject jsonObject = jsonDocument.object();
    int exp=jsonObject.value("exposure").toInt();
    if(exp==1){
        if(ui->exposure->isEnabled()){
            on_exposure_clicked();
            data="success:exposure success";
        }else{
            data="error:no exposure,button not enabled";
        }
    }else{
        data="error:wrong json data";
    }
    mWebSocket->sendTextMessage(data);
}

/**
 * @brief MainWindow::onImageLoaded 图片载入
 */
void MainWindow::onImageLoaded()
{
    //使用窗宽窗位调节，取消原有的亮度对比度调节组件的显示
    //    if(!imageShowState){
    //        ui->saveImage->setEnabled(true);
    //        ui->saveImage->setStyleSheet("border-image: url(:/images/saveImage-able.png)");
    //        brightAdd->show();
    //        brightLabel->show();
    //        brightSlider->show();
    //        brightSub->show();
    //        contrastSub->show();
    //        contrastLabel->show();
    //        contrastSlider->show();
    //        contrastAdd->show();
    //        imageShowState=true;
    //    }
    //    brightSlider->setValue(0);
    //    contrastSlider->setValue(0);

    if(!imageShowState){
        ui->saveImage->setEnabled(true);
        ui->saveImage->setStyleSheet("border-image: url(:/images/saveImage-able.png)");
        wwVal->show();
        wlVal->show();
        rotateButton->show();
        wwwlButton->show();
        antiColorButton->show();
        zoomButton->show();
        translationButton->show();
        resetButton->show();
        imageShowState=true;
    }
}

/**
 * @brief MainWindow::onWwwlChanged wwwl发生改变
 * @param ww
 * @param wl
 */
void MainWindow::onWwwlChanged(int ww, int wl)
{
    wwVal->setText("WW:"+QString("%1").arg(ww));
    wlVal->setText("WL:"+QString("%1").arg(wl));
}

/**
 * @brief MainWindow::onReadFpdBatteryLevelTimerOutTime 定时获取探测器电量
 */
void MainWindow::onReadFpdBatteryLevelTimerOutTime()
{
    int fpdbatteryVal=fpd->GetBatteryLevel();
    ui->fpdbatteryLevel->setText(QString("%1%").arg(fpdbatteryVal));
    if(fpdbatteryVal<=100 && fpdbatteryVal>70){
        ui->fpdbatteryLevel->setStyleSheet("border-image: url(:/images/electric-green.png)");
    }else if(fpdbatteryVal<=70 && fpdbatteryVal>20){
        ui->fpdbatteryLevel->setStyleSheet("border-image: url(:/images/electric-orange.png)");
    }else if(fpdbatteryVal<=20 && fpdbatteryVal>=0){
        ui->fpdbatteryLevel->setStyleSheet("border-image: url(:/images/electric-red.png)");
    }
}

/**
 * @brief MainWindow::onTenMinutesTimerStarted 开始倒计时10分钟
 */
void MainWindow::onTenMinutesTimerStarted()
{
    if(!tenMinutesTimer->isActive()){
        tenMinutesTimer->setInterval(10*60*1000);
        tenMinutesTimer->start();
    }
}

/**
 * @brief MainWindow::onTenMinutesTimerOutTime 10分钟后触发
 */
void MainWindow::onTenMinutesTimerOutTime()
{
    if(tenMinutesTimer->isActive()){
        tenMinutesTimer->stop();
    }
    maskWidget->show();
    lowBatteryWarning->startShutdownTimer();
    lowBatteryWarning->exec();
}


void MainWindow::on_volSet_editingFinished()
{
    //int setVol = ui->voltmeter->text().toInt();
    int setVol = ui->volSet->text().toInt();
    DIY_LOG(LOG_INFO, "Input voltage: %dKV", setVol);
    if((setVol< 80 )&& (setVol>55))
        writeExposurekV(setVol);
    else
        writeExposurekV(70);
}


void MainWindow::on_amSet_editingFinished()
{
    //int setAm = ui->ammeter->text().toInt();
    int setAm = ui->amSet->text().toInt();
    DIY_LOG(LOG_INFO, "Input current: %dmA", setAm);
    if((setAm<= 6600 )&& (setAm>500))
        writeExposuremA(setAm);
    else
        writeExposuremA(2000);
}
