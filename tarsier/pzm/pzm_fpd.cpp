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

    BOOL ret;
    ret = init_lib();
    if(!ret)
    {}
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

BOOL CPZM_Fpd::init_lib()
{
    BOOL ret;
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
}
