#include <QApplication>
#include "myfpd.h"
#include <iostream>
#include <fstream>

//构造函数
MyFPD::MyFPD(){
    hM_Initilized=false;
    detectorID=0;
    lastError=Err_TaskTimeOut;
    //使用事件的信号状态配合回调函数、curCmdId、lastError实现同步的invoke请求，
    waitAckEvent = CreateEvent(NULL, false, false, NULL);
}

//析构函数
MyFPD::~MyFPD(){
    Destroy();
    FreeIRayLibrary();
    if (waitAckEvent){
        CloseHandle(waitAckEvent);
        waitAckEvent = NULL;
    }
}


/**
 * @brief MyFPD::LoadIRayLibrary 载入IRay 库文件
 * @return 0:成功，非0：错误码
 */
int MyFPD::LoadIRayLibrary(){
    hModule=OpenDLib("FpdSys.DLL");
    if(hModule==NULL){
        printf("\r\n----Load SDK DLL failed! ErrCode = %d----",Err_LoadDllFailed);
        return Err_LoadDllFailed;
    }
    g_fpCreate = (FnCreate)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_CREATE);
    g_fpDestroy = (FnDestroy)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_DESTROY);
    g_fpGetAttr = (FnGetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_GETATTR);
    g_fpSetAttr = (FnSetAttr)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_SETATTR);
    g_fpInvoke = (FnInvoke)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_INVOKE);
    g_fpAbort=(FnAbort)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_ABORT);
    g_fpGetErrInfo=(FnGetErrInfo)GetProcAddress(hModule, IRAY_FPD_PROC_NAME_GET_ERROR_INFO);
    if(g_fpCreate==NULL || g_fpDestroy==NULL || g_fpGetAttr==NULL || g_fpSetAttr==NULL ||
            g_fpInvoke==NULL || g_fpAbort==NULL || g_fpGetErrInfo==NULL){
        printf("\r\n----Get exported function failed! ErrCode = %d----",Err_LoadDllFailed);
        FreeIRayLibrary();
        return Err_LoadDllFailed;
    }
    hM_Initilized=true;//设置hModule及相关接口初始化情况
    return Err_OK;
}


/**
 * @brief MyFPD::FreeIRayLibrary 卸载IRay库文件
 */
void MyFPD::FreeIRayLibrary(){
    if (hModule)
    {
        CloseDLib(hModule);
        hModule = NULL;
    }
}


/**
 * @brief MyFPD::Create 创建
 * @param pszWorkDir 平板设备的工作目录
 * @param fpCallback SDK事件回调的函数
 * @return 0:成功，非0：错误码
 */
FPDRESULT MyFPD::Create(const char* pszWorkDir, FnCallback fpCallback){
    if (!hM_Initilized){
        int ret=LoadIRayLibrary();
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
    strncpy(var.val.strVal, strValue, IRAY_MAX_STR_LEN);
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
        memcpy(result.strVal, var.val.strVal, IRAY_MAX_STR_LEN);
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
        return Err_NotInitialized;
    }
    ErrorInfo eInfo;
    FPDRESULT ret=g_fpGetErrInfo(nErrorCode,&eInfo);
    if(ret==Err_OK){
        errorInfo=eInfo.szDescription;
    }
    return ret;
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
    if (Err_TaskPending == ret)
    {
        printf("\r\n----Test command invoke is pending, please wait task result event.----");
    }
    if (Err_OK == ret)
    {
        printf("\r\n----Test command invoke succeed!----");
    }
    else
    {
        printf("\r\n----Test command invoke failed! ErrCode = %d----", ret);
    }
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
    FPDRESULT ret = Invoke( cmdId, pars, nParCount);
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
    switch (nEventID){
        case Evt_TaskResult_Succeed://任务成功
            lastError = Err_OK;
            if (nParam1==curCmdId){//判断返回的命令与调用的命令是否一致
                SetEvent(waitAckEvent);//设置事件为有信号
            }
        case Evt_TaskResult_Failed://任务失败
            lastError = nParam2;//任务失败时，nParam2返回错误码
            if (nParam1==curCmdId){//判断返回的命令与调用的命令是否一致
                SetEvent(waitAckEvent);//设置事件为有信号
            }
            break;
        default:
            break;
    }
}

