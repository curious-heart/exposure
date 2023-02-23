#ifndef MYFPD_H
#define MYFPD_H

#include <Windows.h>
#define OpenDLib(libname)	LoadLibrary(L##libname)
#define CloseDLib			FreeLibrary
#include "include/IRayFpdSys.h"
#include "include/IRayFpdSysEx.h"
#include <string>
#include <stdio.h>
using namespace :: std;

//union 存储空间共用
union AttrResult
{
    int   nVal;
    float fVal;
    char  strVal[IRAY_MAX_STR_LEN];
};


class MyFPD
{
public:
    MyFPD();
    ~MyFPD();
    int  LoadIRayLibrary();
    void FreeIRayLibrary();
    FPDRESULT Create(const char* pszWorkDir, FnCallback fpCallback);
    FPDRESULT Abort();
    FPDRESULT Destroy();
    FPDRESULT SetAttr(int nAttrID, int nValue);
    FPDRESULT SetAttr(int nAttrID, float fValue);
    FPDRESULT SetAttr(int nAttrID, const char* strValue);
    FPDRESULT GetAttr(int nAttrID, AttrResult& result);
    FPDRESULT GetErrorInfo(int nErrorCode);
    FPDRESULT Invoke(int cmdId, IRayCmdParam pars[], int nParCount);
    int WaitEvent(int timeout);
    FPDRESULT SyncInvoke(int cmdId, IRayCmdParam pars[], int nParCount,int timeout);
    void MyCallback(int nDetectorID, int nEventID, int nEventLevel,
        const char* pszMsg, int nParam1, int nParam2, int nPtrParamLen, void* pParam);
    FPDRESULT ConnectionFPD();
    FPDRESULT DisconnectionFPD();
private:
    HMODULE hModule;
    bool hM_Initilized;
    FnCreate g_fpCreate;
    FnDestroy g_fpDestroy;
    FnGetAttr g_fpGetAttr;
    FnSetAttr g_fpSetAttr;
    FnInvoke g_fpInvoke;
    FnAbort   g_fpAbort;
    FnGetErrInfo g_fpGetErrInfo;
    int detectorID;
    string errorInfo;
    HANDLE waitAckEvent;
    int curCmdId;
    int lastError;
};

#endif // MYFPD_H
