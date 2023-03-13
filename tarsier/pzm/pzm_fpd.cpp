#include "pzm_fpd.h"
#include "logger.h"

CPZM_Fpd::CPZM_Fpd(QObject *parent, fpd_model_info_t* model)
    : QObject(parent), m_model_info(model)
{
    QString err_str;
    m_api_lib = new QLibrary();
    if(!m_api_lib)
    {
        err_str = "new library for PZM fail!!!";
        DIY_LOG(LOG_ERROR, err_str);
        emit fpdErrorOccurred(err_str);
        return;
    }
    m_api_lib->setFileName(model->api_lib_pfn);
    if(!m_api_lib->load())
    {
        err_str = QString("Load library %1 error: %2.").arg(model->api_lib_pfn, m_api_lib->errorString());
        DIY_LOG(LOG_ERROR, err_str);
        return;
    }
    DIY_LOG(LOG_INFO, QString("Load library %1 success.").arg(model->api_lib_pfn));
    resolve_lib_functions();

    BOOL ret;
    ret = reg_pzm_callbacks();
    if(!ret)
    {}
}

void CPZM_Fpd::resolve_lib_functions()
{
    m_hptr_COM_Init = (Fnt_COM_Init)m_api_lib->resolve(m_hstr_COM_Init);
    m_hptr_COM_Uninit = (Fnt_COM_Uninit)m_api_lib->resolve(m_hstr_COM_Uninit);
    m_hptr_COM_SetCfgFilePath = (Fnt_COM_SetCfgFilePath)m_api_lib->resolve(m_hstr_COM_SetCfgFilePath);
    m_hptr_COM_List = (Fnt_COM_List)m_api_lib->resolve(m_hstr_COM_List);
    m_hptr_COM_Open = (Fnt_COM_Open)m_api_lib->resolve(m_hstr_COM_Open);
    m_hptr_COM_Close = (Fnt_COM_Close)m_api_lib->resolve(m_hstr_COM_Close);
    m_hptr_COM_StartNet = (Fnt_COM_StartNet)m_api_lib->resolve(m_hstr_COM_StartNet);
    m_hptr_COM_StopNet = (Fnt_COM_StopNet)m_api_lib->resolve(m_hstr_COM_StopNet);
    m_hptr_COM_RegisterEvCallBack = (Fnt_COM_RegisterEvCallBack)m_api_lib->resolve(m_hstr_COM_RegisterEvCallBack);
    m_hptr_COM_SetCalibMode = (Fnt_COM_SetCalibMode)m_api_lib->resolve(m_hstr_COM_SetCalibMode);
    m_hptr_COM_GetFPsn = (Fnt_COM_GetFPsn)m_api_lib->resolve(m_hstr_COM_GetFPsn);
    m_hptr_COM_GetFPsnEx = (Fnt_COM_GetFPsnEx)m_api_lib->resolve(m_hstr_COM_GetFPsnEx);
    m_hptr_COM_GetFPCurStatus = (Fnt_COM_GetFPCurStatus)m_api_lib->resolve(m_hstr_COM_GetFPCurStatus);
    m_hptr_COM_GetFPCurStatusEx = (Fnt_COM_GetFPCurStatusEx)m_api_lib->resolve(m_hstr_COM_GetFPCurStatusEx);
    m_hptr_COM_GetImageMode = (Fnt_COM_GetImageMode)m_api_lib->resolve(m_hstr_COM_GetImageMode);
    m_hptr_COM_GetImage = (Fnt_COM_GetImage)m_api_lib->resolve(m_hstr_COM_GetImage);
    m_hptr_COM_AedAcq = (Fnt_COM_AedAcq)m_api_lib->resolve(m_hstr_COM_AedAcq);
    m_hptr_COM_AedTrigger = (Fnt_COM_AedTrigger)m_api_lib->resolve(m_hstr_COM_AedTrigger);
    m_hptr_COM_Stop = (Fnt_COM_Stop)m_api_lib->resolve(m_hstr_COM_Stop);
    m_hptr_COM_ExposeReq = (Fnt_COM_ExposeReq)m_api_lib->resolve(m_hstr_COM_ExposeReq);
}

CPZM_Fpd::~CPZM_Fpd()
{
    QString err_str;
    if(m_api_lib && m_api_lib->isLoaded())
    {
        if(m_api_lib->unload())
        {
            DIY_LOG(LOG_INFO, QString("Unload library %1 success.").arg(m_model_info->api_lib_pfn));
        }
        else
        {
            DIY_LOG(LOG_ERROR, QString("Unload library %ls failed.").arg(m_model_info->api_lib_pfn));
        }
    }
    delete m_api_lib;
    m_api_lib = nullptr;
}

BOOL CPZM_Fpd::reg_pzm_callbacks()
{
    BOOL ret = TRUE;
    //COM_SetCfgFilePath(m_model_info->cfg_file_pth.toLatin1().data());

    /*
    COM_RegisterEvCallBack(EVENT_LINKUPEX, FuncLinkexCallBack);
    COM_RegisterEvCallBack(EVENT_LINKDOWNEX, FuncBreakexCallBack);
    COM_RegisterEvCallBack(EVENT_HEARTBEATEX, FuncHeartBeatexCallBack);
    COM_RegisterEvCallBack(EVENT_LINKUP, FuncLinkCallBack);
    COM_RegisterEvCallBack(EVENT_LINKDOWN, FuncBreakCallBack);
    COM_RegisterEvCallBack(EVENT_IMAGEVALID, FuncImageCallBack);
    COM_RegisterEvCallBack(EVENT_HEARTBEAT, FuncHeartBeatCallBack);
    COM_RegisterEvCallBack(EVENT_READY, FuncReadyCallBack);
    COM_RegisterEvCallBack(EVENT_EXPOSE, FuncExposeCallBack);
    */

    //ret = COM_Init();
    if(!ret)
    {}

#ifdef FP_AUTO_CONNECT //auto connect at single fp
    COM_Open(NULL);
#endif

    //COM_SetCalibMode(IMG_CALIB_GAIN | IMG_CALIB_DEFECT);
    return TRUE;
}
