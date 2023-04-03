#include "common_tool_func.h"
#include "myfpd.h"
#include "settingcfg.h"
#include <QDebug>

#include "logger.h"

/**
 * @brief MyFPD::MyFPD  构造函数
 * @param parent
 */
MyFPD::MyFPD(QObject *parent) : QObject(parent){
    hM_Initilized=false;
    detectorID=0;
    lastError=Err_TaskTimeOut;
    //使用事件的信号状态配合回调函数、curCmdId、lastError实现同步的invoke请求，
    waitAckEvent = CreateEvent(NULL, false, false, NULL);
    qLib=new QLibrary();
    //set_host_ip_address(IP_INTF_WIFI, IP_SET_TYPE_IPV4_DYNAMIC);
    FpdSettingCfg &fsc=SettingCfg::getInstance().getFpdSettingCfg();
    m_fpd_used_if_idx = set_host_wifi_or_eth_ip_addr(IP_SET_TYPE_IPV4_DYNAMIC, fsc.fpd_conn_media);
}


/**
 * @brief MyFPD::~MyFPD  析构函数
 */
MyFPD::~MyFPD(){
    Destroy();
    FreeIRayLibrary();
    if (waitAckEvent){
        CloseHandle(waitAckEvent);
        waitAckEvent = NULL;
    }
    delete qLib;
    qLib=NULL;
}


/**
 * @brief MyFPD::LoadIRayLibrary 载入IRay 库文件
 * @param dllPath dll文件路径
 * @return 0:成功，非0：错误码
 */
int MyFPD::LoadIRayLibrary(QString dllPath){
    //qLib=new QLibrary(dllPath);
    qLib->setFileName(dllPath);
    //qLib=new QLibrary("TiffParse");
    if(qLib->load()){
        //qDebug()<<tr("----Load SDK DLL Success----");
        DIY_LOG(LOG_INFO,"----Load SDK DLL Success----");
        g_fpCreate = (FnCreate)qLib->resolve(IRAY_FPD_PROC_NAME_CREATE);
        g_fpDestroy = (FnDestroy)qLib->resolve(IRAY_FPD_PROC_NAME_DESTROY);
        g_fpGetAttr = (FnGetAttr)qLib->resolve(IRAY_FPD_PROC_NAME_GETATTR);
        g_fpSetAttr = (FnSetAttr)qLib->resolve(IRAY_FPD_PROC_NAME_SETATTR);
        g_fpInvoke = (FnInvoke)qLib->resolve(IRAY_FPD_PROC_NAME_INVOKE);
        g_fpAbort=(FnAbort)qLib->resolve(IRAY_FPD_PROC_NAME_ABORT);
        g_fpGetErrInfo=(FnGetErrInfo)qLib->resolve(IRAY_FPD_PROC_NAME_GET_ERROR_INFO);
        if(g_fpCreate==NULL || g_fpDestroy==NULL || g_fpGetAttr==NULL || g_fpSetAttr==NULL ||
                g_fpInvoke==NULL || g_fpAbort==NULL || g_fpGetErrInfo==NULL){
            qDebug()<<tr("\r\n----Get exported function failed! ErrCode = ")<<Err_LoadDllFailed;
            FreeIRayLibrary();
            return Err_LoadDllFailed;
        }
        hM_Initilized=true;//设置hModule及相关接口初始化情况
        return Err_OK;
    }else{
        qDebug()<<tr("----Load SDK DLL Error----");
        qDebug()<<qLib->errorString();
        return Err_LoadDllFailed;
    }


//    qDebug()<<tr("----Load SDK DLL----")<<dllPath;
//    //LPCWSTR lpcwStr = dllPath.toStdWString().c_str();
//    hModule=OpenDLib("FpdSys.dll");
//    //hModule=LoadLibraryW(lpcwStr);
//    qDebug()<<(hModule==NULL);
//    qDebug()<<"GetLastError="<<(GetLastError());
//    if(hModule==NULL){
//        qDebug()<<tr("\r\n----Load SDK DLL failed! ErrCode =")<<Err_LoadDllFailed;
//        return Err_LoadDllFailed;
//    }
//    g_fpCreate = (FnCreate)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_CREATE);
//    g_fpDestroy = (FnDestroy)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_DESTROY);
//    g_fpGetAttr = (FnGetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_GETATTR);
//    g_fpSetAttr = (FnSetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_SETATTR);
//    g_fpInvoke = (FnInvoke)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_INVOKE);
//    g_fpAbort=(FnAbort)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_ABORT);
//    g_fpGetErrInfo=(FnGetErrInfo)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_GET_ERROR_INFO);
//    if(g_fpCreate==NULL || g_fpDestroy==NULL || g_fpGetAttr==NULL || g_fpSetAttr==NULL ||
//            g_fpInvoke==NULL || g_fpAbort==NULL || g_fpGetErrInfo==NULL){
//        qDebug()<<tr("\r\n----Get exported function failed! ErrCode = ")<<Err_LoadDllFailed;
//        FreeIRayLibrary();
//        return Err_LoadDllFailed;
//    }
//    hM_Initilized=true;//设置hModule及相关接口初始化情况
//    return Err_OK;
}


/**
 * @brief MyFPD::FreeIRayLibrary 卸载IRay库文件
 * @return 0:成功，非0：错误
 */
int MyFPD::FreeIRayLibrary(){
//    if (hModule)
//    {
//        CloseDLib(hModule);
//        hModule = NULL;
//    }
    if(qLib->isLoaded()){
        if(qLib->unload()){
            hM_Initilized=false;
            return Err_OK;
        }else{
            emit fpdErrorOccurred("卸载IRay库失败");
            return 1;
        }
    }else{
        hM_Initilized=false;
        return Err_OK;
    }
}


/**
 * @brief MyFPD::Create 创建
 * @param dllPath dll文件路径
 * @param pszWorkDir 平板设备的工作目录
 * @param fpCallback SDK事件回调的函数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Create(QString dllPath,const char* pszWorkDir, FnCallback fpCallback){
    if (!hM_Initilized){
        int ret=LoadIRayLibrary(dllPath);
        if(ret!=Err_OK){
            return ret;
        }
    }
    FPDRESULT ret=g_fpCreate(pszWorkDir,fpCallback,&detectorID);
    return ret;
}


/**
 * @brief MyFPD::Abort 中止
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Abort(){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    FPDRESULT ret=g_fpAbort(detectorID);
    return ret;
}


/**
 * @brief MyFPD::Destroy 销毁
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Destroy(){
    if(detectorID>0){
        FPDRESULT ret=g_fpDestroy(detectorID);
        if(ret!=Err_OK){
            return ret;
        }
        detectorID=0;
    }
    return Err_OK;
}


/**
 * @brief MyFPD::SetAttr 设置属性的值(int 类型)
 * @param nAttrID 指定设置的属性
 * @param nValue 属性的值
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SetAttr(int nAttrID, int nValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    IRayVariant var;
    var.vt = IVT_INT;
    var.val.nVal = nValue;
    FPDRESULT ret=g_fpSetAttr(detectorID,nAttrID,&var);
    return ret;
}


/**
 * @brief MyFPD::SetAttr 设置属性的值(float 类型)
 * @param nAttrID 指定设置的属性
 * @param fValue 属性的值
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SetAttr(int nAttrID, float fValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    IRayVariant var;
    var.vt = IVT_FLT;
    var.val.nVal = fValue;
    FPDRESULT ret=g_fpSetAttr(detectorID,nAttrID,&var);
    return ret;
}


/**
 * @brief MyFPD::SetAttr 设置属性的值(const char * 类型)
 * @param nAttrID 指定设置的属性
 * @param strValue 属性的值
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SetAttr(int nAttrID, const char * strValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    if (!strValue){
        return Err_InvalidParamValue;
    }
    IRayVariant var;
    var.vt = IVT_STR;
    //strncpy拷贝，被拷贝数据不足时补\0，保障字符串长度
    strncpy(var.val.strVal, strValue, IRAY_MAX_STR_LEN-1);
    FPDRESULT ret=g_fpSetAttr(detectorID,nAttrID,&var);
    return ret;
}


/**
 * @brief MyFPD::GetAttr 根据属性获得属性的值
 * @param nAttrID 属性
 * @param result 获得的结果
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::GetAttr(int nAttrID, AttrResult &result){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    IRayVariant var;
    FPDRESULT ret=g_fpGetAttr(detectorID,nAttrID,&var);
    if (ret!=Err_OK){
        memset(&result, 0, sizeof(result));
        return ret;
    }
    if(var.vt==IVT_INT){
        result.nVal=var.val.nVal;
    }else if(var.vt==IVT_FLT){
        result.fVal=var.val.fVal;
    }else if(var.vt==IVT_STR){
        memcpy(result.strVal, var.val.strVal, IRAY_MAX_STR_LEN-1);
    }
    return ret;
}


/**
 * @brief MyFPD::GetErrorInfo 通过错误码获得错误信息
 * @param nErrorCode 错误码
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::GetErrorInfo(int nErrorCode){
    if (!hM_Initilized){
        emit fpdErrorOccurred("探测器初始化失败");
        return Err_NotInitialized;
    }
    ErrorInfo eInfo;
    FPDRESULT ret=g_fpGetErrInfo(nErrorCode,&eInfo);
    if(ret==Err_OK){
        //errorInfo=eInfo.szDescription;
        emit fpdErrorOccurred(eInfo.szDescription);
    }else{
        emit fpdErrorOccurred("通过错误码获得错误信息失败");
    }
    return ret;
}

#define INVOKE_LOG_PRINT(cmdId, ret)\
{\
    if (Err_TaskPending == ret)\
    {\
        DIY_LOG(LOG_INFO, \
                QString("Test command %1 invoke is pending, please wait task result event.").arg(cmdId));\
    }\
    else if (Err_OK == ret)\
    {\
        DIY_LOG(LOG_INFO, QString("Test command %1 invoke succeed!").arg(cmdId));\
    }\
    else\
    {\
        DIY_LOG(LOG_ERROR, QString("Test command %1 invoke failed! ErrCode = %2.").arg(cmdId).arg(ret));\
    }\
}

/**
 * @brief MyFPD::Invoke 调用命令
 * @param cmdId 命令
 * @param pars 命令参数
 * @param nParCount 参数个数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId, IRayCmdParam pars[],int nParCount){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    ResetEvent(waitAckEvent);//事件设置为无信号
    curCmdId = cmdId;//局部变量的值复制到全局
    FPDRESULT ret=g_fpInvoke(detectorID, cmdId, pars, nParCount);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::Invoke 调用命令(无参数)
 * @param cmdId 命令
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    ResetEvent(waitAckEvent);
    curCmdId = cmdId;
    FPDRESULT ret = g_fpInvoke(detectorID, cmdId, NULL, 0);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::Invoke 调用命令(一个IVT_INT 参数)
 * @param cmdId 命令
 * @param nValue  (IVT_INT)int类型的参数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId, int nValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    ResetEvent(waitAckEvent);
    curCmdId = cmdId;
    IRayCmdParam param;
    param.pt = IPT_VARIANT;
    param.var.vt = IVT_INT;
    param.var.val.nVal = nValue;
    FPDRESULT ret = g_fpInvoke(detectorID, cmdId, &param, 1);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::Invoke 调用命令(两个IVT_INT 参数)
 * @param cmdId 命令
 * @param nValue1 (IVT_INT)int类型的参数1
 * @param nValue2 (IVT_INT)int类型的参数2
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId, int nValue1, int nValue2){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    ResetEvent(waitAckEvent);
    curCmdId = cmdId;
    IRayCmdParam param[2];
    param[0].pt = IPT_VARIANT;
    param[0].var.vt = IVT_INT;
    param[0].var.val.nVal = nValue1;
    param[1].pt = IPT_VARIANT;
    param[1].var.vt = IVT_INT;
    param[1].var.val.nVal = nValue2;
    FPDRESULT ret = g_fpInvoke(detectorID, cmdId, param, 2);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::Invoke 调用命令(一个IVT_FLT参数)
 * @param cmdId 命令
 * @param fValue  (IVT_FLT)float类型的参数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId, float fValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    ResetEvent(waitAckEvent);
    curCmdId = cmdId;
    IRayCmdParam param;
    param.pt = IPT_VARIANT;
    param.var.vt = IVT_FLT;
    param.var.val.fVal = fValue;
    FPDRESULT ret = g_fpInvoke(detectorID, cmdId, &param, 1);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::Invoke 调用命令(一个IVT_STR参数)
 * @param cmdId 命令
 * @param strValue (IVT_STR)const char*类型的参数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Invoke(int cmdId, const char* strValue){
    if (!hM_Initilized){
        return Err_NotInitialized;
    }
    if (!strValue){
        return Err_InvalidParamValue;
    }
    ResetEvent(waitAckEvent);
    curCmdId = cmdId;
    IRayCmdParam param;
    param.pt = IPT_VARIANT;
    param.var.vt = IVT_STR;
    strncpy(param.var.val.strVal, strValue, IRAY_MAX_STR_LEN-1);
    FPDRESULT ret = g_fpInvoke(detectorID, cmdId, &param, 1);
    INVOKE_LOG_PRINT(cmdId, ret);
    return ret;
}


/**
 * @brief MyFPD::WaitEvent 等待任务完成
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
int MyFPD::WaitEvent(int timeout){
    int wait = WaitForSingleObject(waitAckEvent, timeout);//获取信号状态，无信号：回调函数未实行完毕  有信号：回调函数实行完毕
    if (WAIT_TIMEOUT == wait){//超时
        return Err_TaskTimeOut;
    }else{
        return lastError;//获得回调函数返回的结果
    }
}

/**
 * @brief MyFPD::SyncInvoke 同步调用命令
 * @param cmdId 命令
 * @param pars 命令参数
 * @param nParCount 参数个数
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, IRayCmdParam pars[], int nParCount, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId, pars, nParCount);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}

/**
 * @brief MyFPD::SyncInvoke 同步调用命令(无参数)
 * @param cmdId 命令
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}


/**
 * @brief MyFPD::SyncInvoke 同步调用命令(一个IVT_INT 参数)
 * @param cmdId 命令
 * @param nValue (IVT_INT)int类型的参数
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, int nValue, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId,nValue);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}


/**
 * @brief MyFPD::SyncInvoke 同步调用命令(两个IVT_INT 参数)
 * @param cmdId 命令
 * @param nValue1 (IVT_INT)int类型的参数1
 * @param nValue2 (IVT_INT)int类型的参数2
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, int nValue1, int nValue2, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId,nValue1,nValue2);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}


/**
 * @brief MyFPD::SyncInvoke 同步调用命令(一个IVT_FLT参数)
 * @param cmdId 命令
 * @param fValue (IVT_FLT)float类型的参数
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, float fValue, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId,fValue);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}


/**
 * @brief MyFPD::SyncInvoke 同步调用命令(一个IVT_STR参数)
 * @param cmdId 命令
 * @param strValue (IVT_STR)const char*类型的参数
 * @param timeout 超时时间设置，单位：毫秒
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::SyncInvoke(int cmdId, const char *strValue, int timeout){
    lastError=Err_TaskTimeOut;
    FPDRESULT ret = Invoke( cmdId,strValue);
    if(ret==Err_TaskPending){//任务挂起，等待异步执行
        ret=WaitEvent(timeout);
    }
    return ret;
}



/**
 * @brief MyFPD::MyCallback 回调函数
 * @param nDetectorID 触发回调函数的探测器
 * @param nEventID 事件id
 * @param nEventLevel 事件级别
 * @param pszMsg 事件描述
 * @param nParam1 事件的参数 ，为特定事件定义
 * @param nParam2 事件的参数，为特定事件定义
 * @param nPtrParamLen 指针类型参数的字节计数，为特定事件定义
 * @param pParam 指针类型的参数，为特定事件定义
 */
void MyFPD::MyCallback(int nDetectorID, int nEventID, int nEventLevel, const char *pszMsg, int nParam1, int nParam2, int nPtrParamLen, void *pParam){
    Q_UNUSED(nDetectorID);
    Q_UNUSED(nEventLevel);
    Q_UNUSED(pszMsg);
    Q_UNUSED(nPtrParamLen);
    Q_UNUSED(pParam);
    int cmdid_for_dbg = curCmdId;
    switch (nEventID){
        case Evt_TaskResult_Succeed://任务成功
            DIY_LOG(LOG_INFO, QString("Evt_TaskResult_Succeed, curCmdID:%1.").arg(cmdid_for_dbg));
            lastError = Err_OK;
            if(nParam1==curCmdId){//判断返回的命令与调用的命令是否一致
                SetEvent(waitAckEvent);//设置事件为有信号
            }
            break;
        case Evt_TaskResult_Failed://任务失败
            DIY_LOG(LOG_INFO,
                    QString("Evt_TaskResult_Failed, curCmdID:%1, err code:%2.")
                    .arg(cmdid_for_dbg).arg(nParam2));
            lastError = nParam2;//任务失败时，nParam2返回错误码
            if(nParam1==curCmdId){//判断返回的命令与调用的命令是否一致
                SetEvent(waitAckEvent);//设置事件为有信号
            }
            break;
        case Evt_TaskResult_Canceled://任务取消
            DIY_LOG(LOG_INFO,
                    QString("Evt_TaskResult_Canceled, curCmdID:%1.").arg(cmdid_for_dbg));
            if(nParam1==curCmdId){//判断返回的命令与调用的命令是否一致
                SetEvent(waitAckEvent);//设置事件为有信号
            }
            break;
        default:
            DIY_LOG(LOG_INFO,
                    QString("Unknown event ID, nEventID:%1, curCmdID:%2. evtlevel:%3, pszMsg:%4, nParam1:%5, nParam2:%6.")
                    .arg(nEventID).arg(cmdid_for_dbg).arg(nEventLevel).arg(pszMsg).arg(nParam1).arg(nParam2));
            break;
    }
}

/**
 * @brief MyFPD::GetBatteryLevel 获得探测器电池电量
 * @return
 */
int MyFPD::GetBatteryLevel()
{
    AttrResult ar;
    int ret=GetAttr(Attr_Battery_Remaining,ar);
    if (ret!=Err_OK){
        GetErrorInfo(ret);
    }
    int level=ar.nVal;
    return level;
}


