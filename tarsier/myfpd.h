#ifndef MYFPD_H
#define MYFPD_H

#include <QObject>
#include <QLibrary>
#include <Windows.h>
#include "include/IRayFpdSys.h"
#include "include/IRayFpdSysEx.h"

#define OpenDLib(libname)	LoadLibrary(L##libname)
#define CloseDLib			FreeLibrary

//union 存储空间共用
union AttrResult
{
    int   nVal;
    float fVal;
    char  strVal[IRAY_MAX_STR_LEN];
};

class MyFPD : public QObject
{
    Q_OBJECT
public:
    explicit MyFPD(QObject *parent = nullptr);
    ~MyFPD();
    int  LoadIRayLibrary(QString dllPath);
    int FreeIRayLibrary();
    FPDRESULT Create(QString dllPath,const char* pszWorkDir, FnCallback fpCallback);
    FPDRESULT Abort();
    FPDRESULT Destroy();
    FPDRESULT SetAttr(int nAttrID, int nValue);
    FPDRESULT SetAttr(int nAttrID, float fValue);
    FPDRESULT SetAttr(int nAttrID, const char* strValue);
    FPDRESULT GetAttr(int nAttrID, AttrResult& result);
    FPDRESULT GetErrorInfo(int nErrorCode);
    FPDRESULT Invoke(int cmdId, IRayCmdParam pars[], int nParCount);
    FPDRESULT Invoke(int cmdId);
    FPDRESULT Invoke(int cmdId, int nValue);
    FPDRESULT Invoke(int cmdId, int nValue1, int nValue2);
    FPDRESULT Invoke(int cmdId, float fValue);
    FPDRESULT Invoke(int cmdId, const char* strValue);
    int WaitEvent(int timeout);
    FPDRESULT SyncInvoke(int cmdId, IRayCmdParam pars[], int nParCount,int timeout);
    FPDRESULT SyncInvoke(int cmdId,int timeout);
    FPDRESULT SyncInvoke(int cmdId, int nValue,int timeout);
    FPDRESULT SyncInvoke(int cmdId, int nValue1, int nValue2,int timeout);
    FPDRESULT SyncInvoke(int cmdId, float fValue,int timeout);
    FPDRESULT SyncInvoke(int cmdId, const char* strValue,int timeout);
    void MyCallback(int nDetectorID, int nEventID, int nEventLevel,
        const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);
    int GetBatteryLevel();

signals:
    /**
     * @brief fpdErrorOccurred fpd操作中出现错误
     * @param errorInfo 错误信息
     */
    void fpdErrorOccurred(QString errorInfo);


private:
    //HMODULE hModule;
    QLibrary *qLib;
    bool hM_Initilized;
    FnCreate g_fpCreate;
    FnDestroy g_fpDestroy;
    FnGetAttr g_fpGetAttr;
    FnSetAttr g_fpSetAttr;
    FnInvoke g_fpInvoke;
    FnAbort   g_fpAbort;
    FnGetErrInfo g_fpGetErrInfo;
    int detectorID;
    //QString errorInfo;
    HANDLE waitAckEvent;
    int curCmdId;
    int lastError;

};

#endif // MYFPD_H
