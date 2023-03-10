#pragma once
#ifndef _NET_COM_H_
#define _NET_COM_H_

/* 类型定义 */
#ifndef WINAPI
#define WINAPI      __stdcall
#endif

typedef		char				CHAR;
typedef     int                 BOOL;
typedef		short				SHORT;
typedef		short				INT16;
typedef 	int 				INT32;
typedef 	long				LONG;
typedef		unsigned char		UCHAR;
typedef     unsigned char       UINT8;
typedef     unsigned short      UINT16;
typedef 	unsigned int		UINT32;
typedef		unsigned short		USHORT;
typedef     unsigned long		ULONG;
typedef		unsigned long long 	UINT64;

typedef BOOL (WINAPI *FP_EVENT_CALLBACK)(CHAR parameter);
typedef BOOL (WINAPI *FP_STATUS_CALLBACK)(void* pFPStatus);
typedef BOOL (WINAPI *RB_STATUS_CALLBACK)(CHAR nRBStatus);
typedef BOOL (WINAPI *IMAGE_RECEIVED_CALLBACK)(CHAR* pszFileName);
typedef BOOL (WINAPI *COMMAND_CALLBACK)(INT32 nMain,INT32 nSub, BOOL bResult, CHAR* pDiscription);
typedef BOOL (WINAPI *FP_INFO_CALLBACK)(CHAR nEvent,CHAR* pInfoBuffer);

/* 宏定义 */
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0

#define CONNECT_DIR           (CHAR)0
#define CONNECT_PC            (CHAR)1
#define CONNECT_FP1           (CHAR)2
#define CONNECT_FP2           (CHAR)3
#define CONNECT_WIFI          (CHAR)4
#define CONNECT_WIRE          (CHAR)5
#define CONNECT_NONE          (CHAR)0xff

#define FP_OPEND              (CHAR)1
#define FP_CLOSED             (CHAR)0

#define EVENT_LINKUP          (CHAR)1//探测器连接Attach
#define EVENT_LINKDOWN        (CHAR)2//探测器断开连接Attach
#define EVENT_BUSY            (CHAR)3//表示探测器正在忙碌，无法执行当前命令
#define EVENT_CMDSTART        (CHAR)4//探测器开始执行命令
#define EVENT_IMAGEVALID      (CHAR)5//收到探测器图片（仅trig上图相关的命令时才会触发）
#define EVENT_CMDEND          (CHAR)6//当前命令执行完毕
#define EVENT_READY           (CHAR)7//准备好曝光
#define EVENT_EXPOSE          (CHAR)8//曝光
#define EVENT_EXPEND          (CHAR)9//曝光结束
#define EVENT_AED_A1          (CHAR)10//AED进入A1
#define	EVENT_AED_A2          (CHAR)11//AED进入A2
#define EVENT_AEC_STOP		  (CHAR)12//Aec stop
#define EVENT_OFFSETDONE	  (CHAR)13
#define EVENT_XWINEND		  (CHAR)14
#define	EVENT_HEARTBEAT       (CHAR)20//探测器心跳Attach
#define EVENT_CalInterrupt    (CHAR)21//校准中断到来
#define EVENT_LINKUPEX        (CHAR)26//探测器连接Detach
#define EVENT_LINKDOWNEX      (CHAR)27//探测器断开连接Detach
#define EVENT_HEARTBEATEX     (CHAR)28//探测器心跳Detach

#define EVENT_UPLOADFILESTEP  (CHAR)30
#define EVENT_IMAGESTART      (CHAR)50
#define	EVENT_IMAGEEND        (CHAR)51
#define EVENT_BATTLOW1        (CHAR)52
#define	EVENT_BATTLOW2        (CHAR)53

#define BINNING_1x1           (CHAR)0
#define BINNING_2x2           (CHAR)1
#define BINNING_3x3           (CHAR)2
#define BINNING_4x4           (CHAR)3
#define BINNING_6x6           (CHAR)4

//Acq_mode
#define ACQ_SCAN              (CHAR)2	
#define ACQ_READ_OFFSET       (CHAR)3
#define ACQ_READ_OFFSET_READY (CHAR)4	
#define ACQ_READ_IMAGE        (CHAR)6	
#define ACQ_READ_IMAGE_READY  (CHAR)7

#define FP_TPL_1x1 (CHAR)0x01
#define FP_TPL_2x2 (CHAR)0x02
#define FP_TPL_3x3 (CHAR)0x03
#define FP_TPL_4x4 (CHAR)0x04
#define FP_TPL_6x6 (CHAR)0x05

#define IMG_CALIB_RAW         (CHAR)0x00
#define IMG_CALIB_OFFSET      (CHAR)0x01
#define IMG_CALIB_GAIN        (CHAR)0x02
#define IMG_CALIB_DEFECT      (CHAR)0x04

#define STATUS_NULL           (CHAR)0
#define STATUS_IDLE           (CHAR)0x01
#define STATUS_HST            (CHAR)0x02
#define STATUS_AED1           (CHAR)0x03
#define STATUS_AED2           (CHAR)0x04
#define STATUS_RECOVER		  (CHAR)0x05
#define STATUS_OLAED1		  (CHAR)0x06
#define STATUS_OLAED2		  (CHAR)0x07
#define STATUS_CBCT           (CHAR)0x08
#define STATUS_DST		      (CHAR)0x09

#define COM_SUCCESS            (INT32)1000
#define COM_INIT_FAIL          (INT32)1001
#define COM_NO_TPL             (INT32)1002
#define COM_FP_BUSY            (INT32)1003
#define COM_LK_BREAK           (INT32)1004
#define COM_LK_ERR             (INT32)1005
#define COM_TIMEOUT            (INT32)1006
#define COM_TP_ALARM           (INT32)1007

#define FP_TYPE_ERR            (CHAR)10
#define FP_TYPE_3543           (CHAR)11
#define FP_TYPE_4343           (CHAR)12
#define FP_TYPE_1723           (CHAR)13
#define FP_TYPE_2929           (CHAR)14
#define FP_TYPE_2925           (CHAR)15
#define FP_TYPE_A843           (CHAR)16
#define FP_TYPE_1613		   (CHAR)17
#define FP_TYPE_3543B          (CHAR)18
#define FP_TYPE_4343ZF         (CHAR)19
#define FP_TYPE_3543ZF         (CHAR)20
#define FP_TYPE_3025ZF         (CHAR)21
#define FP_TYPE_6543           (CHAR)22
#define FP_TYPE_2121           (CHAR)23

#define FP_COMPATIBLE_VER      (CHAR)1
#define ZM_COMPATIBLE_VER      (CHAR)2
#define ERR_COMPATIBLE_VER      (CHAR)0xFF

#define MAX_CONF_SIZE                   0x100
#define META_DATA_LEN                   40
#define SHOCK_REC_NUM                   50

#define AEC_NUM_1  (CHAR)0x01
#define AEC_NUM_2  (CHAR)0x02
#define AEC_NUM_3  (CHAR)0x04
#define AEC_NUM_4  (CHAR)0x08
#define AEC_NUM_5  (CHAR)0x10

#define COM_API __declspec(dllexport)

/* 结构体定义 */

typedef struct 
{
	UINT16  usRow;
	UINT16  usCol;
	UINT16  usPix ;
	UINT16  usType ;
}TImageMode,*pTImageMode;

typedef struct
{
	UINT16  usTopRowShift;
	UINT16  usLeftColShift;
	UINT16  usBottomRowShift;
	UINT16  usRightColShift;
}TImageShiftMode, *pTImageShiftMode;

typedef struct
{
	INT16 Temp;            
	INT16 Hum;             
}TFpTempHum;
typedef struct
{
	UINT16 Remain;
	UINT16 full;
}TFpBatInfo;
typedef struct
{
	UCHAR       ucSignal_level;
	UCHAR       ucNoise_level;
	UINT16      usFrequency;
	//INT32		iMode;
	UINT16		iMode;
	UINT16		power_level;
	INT32       iBitrates;
}TWifiStatus;

typedef struct
{
	TWifiStatus tWifiStatus;
	TFpTempHum  tFpTempHum;
	TFpBatInfo  tBatInfo1;
	TFpBatInfo  tBatInfo2;
}TFPStat;
typedef struct
{
	UINT16 Remain;
	UINT16 full;
	UINT16 cycles;
	UINT16 serialNum;		
	UINT8 partNum[16]; 
	UINT8 reserve[16];
}TFpBatInfoEx;
typedef struct
{
	TWifiStatus tWifiStatus;
	TFpTempHum  tFpTempHum;
	TFpBatInfoEx  tBatInfo1Ex;
	TFpBatInfoEx  tBatInfo2Ex;
}TFPStatex;
typedef struct
{
	UINT16 wRepeatTimes;  //set重复次数
	UINT16 wCmdDelayTime; //set间延时
	UINT16 wXwinTime;     //xwin时间

	UINT16 wAutoScrubEnable;   //无用
	UINT16 wXwinTimeH;  // wAutoScrubInterval; 
	UINT16 wRefreshEnable;     //0 
	
	UINT16 wXWin_g_delay;//  triger参数的第n个group delay配置为xwin。值为0到7 
	UINT16 wPreProcessing; 
	UINT32 dwRefreshCycle;   //刷新模板周期	
}TFPUserCfg;

typedef struct
{
	UCHAR dwBoardSn[32];       // 序列号
	UCHAR dwMcuVer[16];
	UCHAR dwFpgaVer[16];       // 硬件版本号
	UCHAR dwSwVer[16];         // 软件版本号
	UINT32 dwUsage;         //  使用时间（统计方法）
	UINT32 dwFreeFallTimes;  //  自由落体次数（摔落）
	UINT32 dwExposureDose;   // 曝光剂量（统计方法：图像素累加平均）
	UINT32  dwFpIp;    
	UCHAR  dwFpMac[6];           
	UCHAR ucConMod;           //连接模式(rb or not)
}TFPInfo;

typedef struct  
{
	UINT16 wTubeReady;
	UINT16 wFPAck;
	UINT16 wXwin;
	UINT16 wExposure;
	UINT16 wHSMODE;
	UINT16 wPort;
	UINT16 wXwinH;
}TRBConf;
typedef struct  
{
	UCHAR rbInfo[17];
}TRBInfo;

typedef struct
{
	TFPUserCfg usrCfg;
	UINT16 awFpgaReg[MAX_CONF_SIZE];//FPGA寄存器定义见2
	UINT16 awFpgaReg1[MAX_CONF_SIZE];
	UINT16 awFpgaReg2[MAX_CONF_SIZE];
	UINT16 awFpgaReg3[MAX_CONF_SIZE];
	UINT16 awFpgaReg4[MAX_CONF_SIZE];
}TFpgaFullCfg;

typedef struct
{
	CHAR FPPsn[32];
	UINT32 FPIP;
	CHAR connect;
	CHAR opened;
}TComFpNode;

typedef struct
{
	TComFpNode tFpNode[16];
	CHAR ncount;
}TComFpList;

typedef struct
{
    CHAR essid[64]; 
    CHAR key[64];
	CHAR channel[64]; 
}TWifiConf;

typedef struct
{
	CHAR   license;      //0:permanent  1:  Authorized permission 	
	CHAR   remainDays;   //Remaining days  0x1F mean > 30 days
	CHAR   permission;   //0:yes  1:no	
}TLicenseInfo;

typedef struct
{
	UINT16  usRow;
	UINT16  usCol;
}TImageSize;

typedef struct
{
	UINT16 u16TubeReadyTime;
	UINT16 u16ExposureTime;
	UINT16 u16Port;
}TCBConf;

typedef struct
{
	CHAR cFpSn[32];       // 序列号
	UCHAR  cFpIp[4];
	UCHAR  cFpMac[6];
	//CHAR cMcuVer[16];
	//CHAR cFpgaVer[16];       // 硬件版本号
	//CHAR cCpuVer[16];         // 软件版本号
	UINT32 u32Usage;         //  使用时间（统计方法）
	UINT32 u32ExposureDose;   // 曝光剂量（统计方法：图像素累加平均）
	UINT16 u16FreeFallTimes;  //  自由落体次数（摔落）
}TFPBaseInfo;

typedef struct
{
	INT32 w;
	INT32 x;
	INT32 y;
	INT32 z;
}TQuaternionVectorData;
typedef struct
{
	INT32 h;
	INT32 r;
	INT32 p;
}TEulerVectorData;
typedef struct
{
	INT32 x;
	INT32 y;
	INT32 z;
}TAngularVelocityData;
typedef struct
{
	INT32 x;
	INT32 y;
	INT32 z;
}TGravityVectorData;
typedef struct
{
	TQuaternionVectorData tQuaterV; 
	TEulerVectorData tEulerV;
	TAngularVelocityData tAngVelocity;
	TGravityVectorData tGravityV;
}TMotionFeatures;
typedef struct
{
	UINT16 year;
	UINT8 mon;
	UINT8 day;
	UINT8 hour;
	UINT8 min;
	UINT8 shockLevel;  //1：1.4K   2：4.0K
}TShockDate;
typedef struct
{
	UINT16 shockCount;
	TShockDate dateInfo[SHOCK_REC_NUM];
}TShockInfo;

typedef struct
{
	UINT8 Meta[META_DATA_LEN];
}TMetaData;
typedef struct
{
	UINT8 IsOn;
	UINT32  dwIpStr;
	UINT32  dwIpEnd;
}TDhcpCfg;
/* 接口声明 */
#ifdef __cplusplus
extern "C" {
#endif
	COM_API BOOL COM_Init();
	COM_API BOOL COM_Uninit();
	COM_API BOOL COM_SetCfgFilePath(CHAR* path);
	COM_API BOOL COM_List(TComFpList* ptComFpList);
	COM_API BOOL COM_ListAdd(CHAR* psn);
	COM_API BOOL COM_ListDel(CHAR* psn);
	COM_API BOOL COM_ListClr();
	COM_API BOOL COM_Open(CHAR *psn);
	COM_API BOOL COM_Close();
	COM_API BOOL COM_StopNet();
	COM_API BOOL COM_StartNet();
	COM_API BOOL COM_RegisterEvCallBack(CHAR nEvent,FP_EVENT_CALLBACK funcallback);	
	COM_API BOOL COM_SetPreCalibMode(CHAR nCalMode);
	COM_API CHAR COM_GetPreCalibMode();
	COM_API BOOL COM_SetCalibMode(CHAR nCalMode);
	COM_API CHAR COM_GetCalibMode();
	COM_API BOOL COM_HstAcq();
	COM_API BOOL COM_AedAcq();	
	COM_API BOOL COM_Trigger();
	COM_API BOOL COM_Prep();
	COM_API BOOL COM_Acq();
	COM_API BOOL COM_PrepAcq();
	COM_API BOOL COM_SetAcq();
	COM_API BOOL COM_ComAcq();	
	COM_API BOOL COM_ExposeReq();	
	COM_API BOOL COM_AedTrigger();
	COM_API BOOL COM_Stop();
	COM_API BOOL COM_Dst();
	COM_API BOOL COM_Dacq();
	COM_API BOOL COM_Cbct();
	COM_API BOOL COM_Dexit();
	COM_API BOOL COM_SetMetaData(TMetaData tMetaData);
	COM_API BOOL COM_GetMetaData(TMetaData* ptMetaData);
	COM_API BOOL COM_GetPreImg();
	COM_API BOOL COM_AedAcqOffLine();
	COM_API BOOL COM_AcqOffLineImage();
	COM_API UINT32 COM_GetNumOffLineImg();
	
	COM_API BOOL COM_GetImageMode(TImageMode* ptImageMode);
	COM_API BOOL COM_GetImageModeV(TImageMode* ptImageMode);
	COM_API BOOL COM_GetImageShiftMode(TImageShiftMode* ptImageShiftMode);
	COM_API BOOL COM_GetImageName(CHAR* name);
	COM_API BOOL COM_GetImageID(UINT32 *pimgID);
	COM_API BOOL COM_GetImage(CHAR* pImageBuff);
	COM_API BOOL COM_GetImageV(CHAR* pImageBuff);
	COM_API BOOL COM_ResetFP();	
	COM_API BOOL COM_FpTurnOff();
	COM_API INT32 COM_GetErrNo();
	COM_API BOOL COM_DhcpActivate(CHAR enableflag);
	COM_API BOOL COM_DhcpSetCfg(TDhcpCfg* tDhcpCfg);
	COM_API BOOL COM_DhcpGetCfg(TDhcpCfg* tDhcpCfg);

	COM_API BOOL COM_SetFPConf(TFPUserCfg* ptFpUserCfg);
	COM_API BOOL COM_GetFPConf(TFPUserCfg* ptFpUserCfg);
	COM_API BOOL COM_SetRBConf(TRBConf* ptRbConf);
	COM_API BOOL COM_GetRBConf(TRBConf* ptRbConf);
	COM_API BOOL COM_SetWifiMode(BOOL ApMode);
	COM_API CHAR COM_GetWifiMode();
	COM_API BOOL COM_SetWifiConf(TWifiConf* ptWifiConf);
	COM_API BOOL COM_GetWifiConf(TWifiConf* ptWifiConf);
	COM_API BOOL COM_SetWifiConfEx(TWifiConf* ptWifiConfAp, TWifiConf* ptWifiConfSta);
	COM_API BOOL COM_GetWifiConfEx(TWifiConf* ptWifiConfAp, TWifiConf* ptWifiConfSta);
	COM_API BOOL COM_SetWifiCountry(CHAR* pCountry);
	COM_API BOOL COM_GetWifiCountry(CHAR* pCountry);
	COM_API BOOL COM_SetApEssid(CHAR* pessid);
	COM_API BOOL COM_GetApEssid(CHAR* pessid);
	COM_API BOOL COM_DefFPConf();
	COM_API BOOL COM_DefRBConf();
	COM_API BOOL COM_DefFPTpl();
	COM_API BOOL COM_SetXwin(UINT32 xwin);
	COM_API BOOL COM_GetXwin(UINT32* xwin);
	COM_API BOOL COM_SetTrailTime(USHORT msec);
	COM_API BOOL COM_SetDynamicPara(UINT32 xwin, UINT16 repeat, CHAR binMode);
	COM_API BOOL COM_SetBinningMode(CHAR cbinningMode);
	COM_API BOOL COM_GetBinningMode(CHAR* cbinningMode);
	COM_API BOOL COM_SetIfsRef(CHAR cbinningMode, UCHAR cIfs, UCHAR cRef);
	COM_API BOOL COM_GetIfsRef(CHAR cbinningMode, UCHAR* cIfs, UCHAR* cRef);
	COM_API BOOL COM_SetFpIpNetmask(UINT32 Ip,UINT32 netmask);
	COM_API BOOL COM_GetFpIpNetmask(UINT32* pIp, UINT32* pnetmask);
	COM_API BOOL COM_SetSenValue(USHORT senValue,CHAR* ppassword);
	COM_API BOOL COM_GetSenValue(USHORT* psenValue);
	COM_API BOOL COM_SetClientSn(CHAR* pClientSn, CHAR* ppassword);
	COM_API BOOL COM_GetClientSn(CHAR* pClientSn);
	COM_API BOOL COM_SetClientPn(CHAR* pClientPn, CHAR* ppassword);
	COM_API BOOL COM_GetClientPn(CHAR* pClientPn);
	COM_API BOOL COM_SetNickname(CHAR* pNickname);
	COM_API BOOL COM_GetNickname(CHAR* pNickname);
	COM_API BOOL COM_SetExtBattDefaultCapacity(int iCapacity);
	COM_API BOOL COM_GetExtBattDefaultCapacity(int* piCapacity);
	COM_API BOOL COM_SetAecEnable(UINT16 aecGroup);
	COM_API BOOL COM_SetAecThreshold(CHAR aecNum,UINT16 valueset);

	COM_API BOOL COM_GetFPsn(CHAR* psn);
	COM_API BOOL COM_GetFPsnEx(CHAR index, CHAR* psn);
	COM_API CHAR COM_GetFPCurStatus();
	COM_API CHAR COM_GetFPCurStatusEx(CHAR* psn);
	COM_API CHAR COM_GetFPWireState();
	COM_API CHAR COM_GetFPWireStateEx(CHAR* psn);
	COM_API UINT32 COM_GetFpPowerMode();
	COM_API UINT32 COM_GetFpPowerModeEx(CHAR* psn);
	COM_API UCHAR COM_GetFpWorkState();
	COM_API UCHAR COM_GetFpPendingState();
	COM_API BOOL COM_ClearPendingState();
	COM_API BOOL COM_GetFPStatus(TFPStat* ptFPStat);
	COM_API BOOL COM_GetFPStatusEx(TFPStat* ptFPStat, CHAR* psn);
	COM_API BOOL COM_GetFPStatusP(TFPStatex* ptFPStatex);
	COM_API BOOL COM_GetFPStatusPex(TFPStatex* ptFPStatex, CHAR* psn);
	COM_API BOOL COM_GetConnectEssid(CHAR* pessid);
	COM_API BOOL COM_GetConnectEssidEx(CHAR* pessid, CHAR* psn);
	COM_API BOOL COM_QuaternionActivate(CHAR enableflag);
	COM_API BOOL COM_GetFPMotionFeatures(TMotionFeatures* tMotionFeatures);
	COM_API BOOL COM_GetFPShock(TShockInfo* pShockInfo);
	COM_API BOOL COM_ClearFPShock(CHAR* ppassword);
	COM_API BOOL COM_SetPacketPauseTime(USHORT pauseT_us);
	COM_API BOOL COM_SetFpSelfStart(CHAR enableflag);
	COM_API BOOL COM_SetFpTime2TurnOffAfterDisc(UINT32 time_mins);
	COM_API BOOL COM_SetFpAutoAedOffline(CHAR enableflag);

	COM_API BOOL COM_SetAllTpl();
	COM_API BOOL COM_UploadOffsetTpl(CHAR* path);
	COM_API BOOL COM_DownloadOffsetTpl(CHAR* path);
	COM_API BOOL COM_UploadGainTpl(CHAR* path);
	COM_API BOOL COM_DownloadDefectMap(CHAR* pData);
	COM_API BOOL COM_DownloadDefectMapV(CHAR* pData);
	COM_API BOOL COM_DownloadGainTpl(CHAR* path);
	COM_API BOOL COM_UploadDefectTpl(CHAR* path);
	COM_API BOOL COM_UploadAedTOffsetTpl(CHAR* path);
	COM_API BOOL COM_DownloadAedTOffsetTpl(CHAR* path);
	COM_API BOOL COM_DownloadDefectTpl(CHAR* path);
	COM_API BOOL COM_UploadFPZMTpl(CHAR TplType, CHAR * Tplpath);
	COM_API BOOL COM_DownLoadFPZMTpl(CHAR TplType, CHAR * Tplpath);
	COM_API BOOL COM_SetOffsetTpl(CHAR* path);
	COM_API BOOL COM_SetGainTpl(CHAR* path);
	COM_API BOOL COM_SetDefectTpl(CHAR* path);
	COM_API BOOL COM_GenOffsetTpl();
	COM_API BOOL COM_GenGainTpl();
	COM_API BOOL COM_GenDefectTpl();
	COM_API BOOL COM_CalibOffsetTpl(CHAR* pData);
	COM_API BOOL COM_CalibGainTpl(CHAR* pData);
	COM_API BOOL COM_CalibDefectTpl(CHAR* pData);
	COM_API BOOL COM_TplPathSet(CHAR* path);
	COM_API BOOL COM_TplPathGet(CHAR* path);
	COM_API BOOL COM_ImgPathSet(CHAR* path);
	COM_API BOOL COM_ImgPathGet(CHAR* path);
	COM_API BOOL COM_LogPathSet(CHAR* path);
	COM_API BOOL COM_LogPathGet(CHAR* path);
	COM_API INT32 COM_BatLow1Get();
	COM_API INT32 COM_BatLow2Get();
	COM_API BOOL COM_BatLow1Set(INT32 iBatLow);
	COM_API BOOL COM_BatLow2Set(INT32 iBatLow);
	COM_API BOOL COM_SdkLog(BOOL on);
	COM_API BOOL COM_EditDefectTpl(UINT16 x,UINT16 y,BOOL valid);
	COM_API BOOL COM_EditLineDefectTpl(UINT16 line, BOOL bRow, BOOL valid);

	COM_API BOOL COM_GetDllVer(CHAR* pcSDKVer);
	COM_API BOOL COM_GetRBInfo(TRBInfo* ptRBInfo);
	COM_API BOOL COM_GetFPInfo(TFPInfo* ptFPInfo);
	COM_API BOOL COM_GetRBStatus(CHAR* pcRBStatus);	

	COM_API CHAR COM_GetFPType();
	COM_API BOOL COM_SetFPType(CHAR ucFpType);
	COM_API CHAR COM_GetFPCompatibleVer();

	COM_API BOOL COM_FpVerUpgrade(CHAR* pcVerPath);
	COM_API BOOL COM_FpgaVerUpgrade(CHAR* pcVerPath);
	COM_API BOOL COM_McuVerUpgrade(CHAR* pcVerPath);	
	COM_API BOOL COM_RbVerUpgrade(CHAR* pcVerPath);

	COM_API BOOL COM_Register_FP_STATUS(FP_STATUS_CALLBACK fpStatus);
	COM_API BOOL COM_Register_RB_STATUS(RB_STATUS_CALLBACK rbStatus);
	COM_API BOOL COM_Register_IMAGE_RECEIVED(IMAGE_RECEIVED_CALLBACK imageReceived);
	COM_API BOOL COM_Register_COMMAND_CB(COMMAND_CALLBACK cmdCallback);

	COM_API BOOL COM_SendCMD(const CHAR* pSendData,INT32 nSize,UINT16 us_MCmd,UINT16 us_SCmd);
	COM_API BOOL COM_SetFPFullConf();
	COM_API BOOL COM_GetFPFullConf();
	COM_API BOOL COM_SetRBFullConf();
	COM_API BOOL COM_GetFullConf();	
	COM_API BOOL COM_GenAEDParam();
	COM_API BOOL COM_SetAedCorrKB();
	COM_API BOOL COM_PrintLog(const char* logText);
	COM_API BOOL COM_SetCaliIntNum(unsigned char usCount);
	COM_API BOOL COM_AEDTriggerByHst();
	COM_API BOOL COM_HstTriggerPre();
	COM_API BOOL COM_AEDTriggerByHstStop();
	COM_API UINT16 COM_GetMeanDose(UINT16* u16Image, UINT32 size);
	COM_API BOOL COM_SetAedTOffsetTpl(CHAR* path);
	COM_API BOOL COM_CalibAedTTpl(char* pData, USHORT u16LineNum);
	COM_API BOOL COM_GetFPLicense(TLicenseInfo* tLicenseInfo,CHAR choose = 0);
	COM_API CHAR COM_GetFPTypeFromHardware();
	COM_API BOOL COM_SetRecoverTime(unsigned char ucTime);
	COM_API UCHAR COM_GetRecoverTime();

#ifdef __cplusplus
}
#endif

#endif

