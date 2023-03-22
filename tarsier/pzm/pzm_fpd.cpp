#include "pzm_fpd.h"
#include "logger.h"
#include "common_tool_func.h"

#include <QDir>
#include <QDateTime>

static CPZM_Fpd* sg_curr_pzm_fpd_obj = nullptr;
static TFPStat sg_tFPStat;
static const char* sg_PZM_status_str_map[] =
{
"STATUS_NULL",//#define STATUS_NULL           (CHAR)0
"STATUS_IDLE",//#define STATUS_IDLE           (CHAR)0x01
"STATUS_HST",//#define STATUS_HST            (CHAR)0x02
"STATUS_AED1",//#define STATUS_AED1           (CHAR)0x03
"STATUS_AED2",//#define STATUS_AED2           (CHAR)0x04
"STATUS_RECOVER",//#define STATUS_RECOVER		  (CHAR)0x05
"STATUS_OLAED1",//#define STATUS_OLAED1		  (CHAR)0x06
"STATUS_OLAED2",//#define STATUS_OLAED2		  (CHAR)0x07
"STATUS_CBCT",//#define STATUS_CBCT           (CHAR)0x08
"STATUS_DST",//#define STATUS_DST		      (CHAR)0x09
};

static const char* sg_pzm_host_ip_addr = "192.168.11.252";

CPZM_Fpd::CPZM_Fpd(QObject *parent)
    : QObject(parent)
{
    QString err_str;
    m_api_lib = new QLibrary();
    if(!m_api_lib)
    {
        err_str = "PZM: new QLibrary fail!!!";
        DIY_LOG(LOG_ERROR, err_str);
        emit fpdErrorOccurred(err_str);
        return;
    }
    if(!set_fixed_ip_address(sg_pzm_host_ip_addr))
    {
        DIY_LOG(LOG_ERROR, QString("Set host IP %1 error.").arg(sg_pzm_host_ip_addr));
    }
}

bool CPZM_Fpd::load_library()
{
    QString err_str;
    if(!m_model_info)
    {
        return false;
    }

    m_api_lib->setFileName(m_model_info->api_lib_pfn);
    if(!m_api_lib->load())
    {
        err_str = QString("PZM: Load library %1 error: %2.").arg(m_model_info->api_lib_pfn, m_api_lib->errorString());
        DIY_LOG(LOG_ERROR, err_str);
        return false;
    }
    m_lib_loaded = true;
    return true;
}

bool CPZM_Fpd::unload_library()
{
    bool ret = true;
    if(m_api_lib && m_api_lib->isLoaded())
    {
        if(m_api_lib->unload())
        {
            DIY_LOG(LOG_INFO, QString("PZM: Unload library %1 success.").arg(m_model_info->api_lib_pfn));
        }
        else
        {
            DIY_LOG(LOG_ERROR, QString("PZM: Unload library %ls failed.").arg(m_model_info->api_lib_pfn));
            ret = false;
        }
    }
    else
    {
        DIY_LOG(LOG_WARN, "PZM: Try to unload library, but library is not loaded");
    }
    m_lib_loaded = false;

    return ret;
}

CPZM_Fpd::~CPZM_Fpd()
{
    unload_library();
    delete m_api_lib;
    m_api_lib = nullptr;

    sg_curr_pzm_fpd_obj = nullptr;
}

#define RESOLVE_LIBRARY_AND_CHECK(ptr, fn_type, fn_name)\
{\
    (ptr) = (fn_type)(m_api_lib->resolve((fn_name)));\
    if(nullptr == (ptr))\
    {\
        DIY_LOG(LOG_ERROR, QString("PZM: Resolve api functin error: %1").arg((fn_name)));\
        return false;\
    }\
}
bool CPZM_Fpd::resolve_lib_functions()
{
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_Init, Fnt_COM_Init, m_hstr_COM_Init);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_Uninit, Fnt_COM_Uninit, m_hstr_COM_Uninit);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_SetCfgFilePath, Fnt_COM_SetCfgFilePath, m_hstr_COM_SetCfgFilePath);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_List, Fnt_COM_List, m_hstr_COM_List);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_Open, Fnt_COM_Open, m_hstr_COM_Open);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_Close, Fnt_COM_Close, m_hstr_COM_Close);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_StartNet, Fnt_COM_StartNet, m_hstr_COM_StartNet);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_StopNet, Fnt_COM_StopNet, m_hstr_COM_StopNet);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_RegisterEvCallBack, Fnt_COM_RegisterEvCallBack, m_hstr_COM_RegisterEvCallBack);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_SetCalibMode, Fnt_COM_SetCalibMode, m_hstr_COM_SetCalibMode);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPsn, Fnt_COM_GetFPsn, m_hstr_COM_GetFPsn);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPsnEx, Fnt_COM_GetFPsnEx, m_hstr_COM_GetFPsnEx);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPCurStatus, Fnt_COM_GetFPCurStatus, m_hstr_COM_GetFPCurStatus);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPCurStatusEx, Fnt_COM_GetFPCurStatusEx, m_hstr_COM_GetFPCurStatusEx);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetImageMode, Fnt_COM_GetImageMode, m_hstr_COM_GetImageMode);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetImage, Fnt_COM_GetImage, m_hstr_COM_GetImage);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_AedAcq, Fnt_COM_AedAcq, m_hstr_COM_AedAcq);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_AedTrigger, Fnt_COM_AedTrigger, m_hstr_COM_AedTrigger);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_Stop, Fnt_COM_Stop, m_hstr_COM_Stop);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_ExposeReq, Fnt_COM_ExposeReq, m_hstr_COM_ExposeReq);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_LogPathSet, Fnt_COM_LogPathSet, m_hstr_COM_LogPathSet);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_ImgPathSet, Fnt_COM_ImgPathSet, m_hstr_COM_ImgPathSet);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPStatus, Fnt_COM_GetFPStatus, m_hstr_COM_GetFPStatus);
    RESOLVE_LIBRARY_AND_CHECK(m_hptr_COM_GetFPStatusEx, Fnt_COM_GetFPStatusEx, m_hstr_COM_GetFPStatusEx);

    return true;
}
#undef RESOLVE_LIBRARY_AND_CHECK

#define REGISTER_EVT_CALL_BACK(evt, cb, cb_str)\
{\
    BOOL ret;\
    ret = m_hptr_COM_RegisterEvCallBack((evt), (cb));\
    if(!ret)\
    {\
        DIY_LOG(LOG_ERROR, QString("PZM: Register callback function error: %1").arg((cb_str)));\
        return false;\
    }\
}
bool CPZM_Fpd::reg_pzm_callbacks()
{
    REGISTER_EVT_CALL_BACK(EVENT_LINKUP, FuncLinkCallBack, "FuncLinkCallBack");
    REGISTER_EVT_CALL_BACK(EVENT_LINKUPEX, FuncLinkexCallBack, "FuncLinkexCallBack");
    REGISTER_EVT_CALL_BACK(EVENT_LINKDOWN, FuncBreakCallBack, "FuncBreakCallBack");
    REGISTER_EVT_CALL_BACK(EVENT_LINKDOWNEX, FuncBreakexCallBack, "FuncBreakexCallBack");
    REGISTER_EVT_CALL_BACK(EVENT_HEARTBEAT, FuncHeartBeatCallBack, "FuncHeartBeatCallBack");
    //REGISTER_EVT_CALL_BACK(EVENT_HEARTBEATEX, FuncHeartBeatexCallBack, "FuncHeartBeatexCallBack");
    REGISTER_EVT_CALL_BACK(EVENT_IMAGEVALID, FuncImageCallBack, "FuncImageCallBack");


    return true;

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

    //COM_SetCalibMode(IMG_CALIB_GAIN | IMG_CALIB_DEFECT);
}
#undef REGISTER_EVT_CALL_BACK
/*----------------------------------------------------------------------------*/
//Callback functions
#define PZM_HANDLER_NOT_EXIST()\
    else\
    {\
        DIY_LOG(LOG_ERROR, "PZM: handler object does not exist.");\
        return FALSE;\
    }\

BOOL CPZM_Fpd::FuncLinkCallBack(char nEvent)
{/*EVENT_LINKUP*/
    DIY_LOG(LOG_INFO, QString("PZM: FuncLinkCallBack(%1)").arg(nEvent));
    if(sg_curr_pzm_fpd_obj)
    {
        DIY_LOG(LOG_INFO, "PZM: connected");
        emit sg_curr_pzm_fpd_obj->pzm_fpd_comm_sig(EVENT_LINKUP);
        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

BOOL CPZM_Fpd::FuncLinkexCallBack(char npara)
{/*EVENT_LINKUPEX*/
    DIY_LOG(LOG_INFO, QString("PZM: FuncLinkexCallBack(%1)").arg(npara));
    if(sg_curr_pzm_fpd_obj)
    {
        CHAR sn_buf[PZM_SN_LEN + 1];
        sg_curr_pzm_fpd_obj->m_hptr_COM_GetFPsnEx(npara, sn_buf);
        sn_buf[PZM_SN_LEN] = '\0';
        DIY_LOG(LOG_INFO, QString("PZM: connected sn: %1: %2").arg(npara).arg(sn_buf));
        emit sg_curr_pzm_fpd_obj->pzm_fpd_comm_sig(EVENT_LINKUPEX, npara, QString(sn_buf));

        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

BOOL CPZM_Fpd::FuncBreakCallBack(char npara)
{/*EVENT_LINKDOWN*/
    DIY_LOG(LOG_INFO, QString("PZM: FuncBreakCallBack(%1)").arg(npara));
    if(sg_curr_pzm_fpd_obj)
    {
        CHAR sn_buf[PZM_SN_LEN + 1];
        sg_curr_pzm_fpd_obj->m_hptr_COM_GetFPsnEx(npara, sn_buf);
        sn_buf[PZM_SN_LEN] = '\0';
        DIY_LOG(LOG_INFO, QString("PZM: broken sn: %1: %2").arg(npara).arg(sn_buf));
        emit sg_curr_pzm_fpd_obj->pzm_fpd_comm_sig(EVENT_LINKUP, npara, QString(sn_buf));
        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

BOOL CPZM_Fpd::FuncBreakexCallBack(char npara)
{/*EVENT_LINKDOWNEX*/
    //This is "link broken down" event, not "closed" event. When normal close, this event is not released.

    DIY_LOG(LOG_INFO, QString("PZM: FuncBreakexCallBack(%1)").arg(npara));
    if(sg_curr_pzm_fpd_obj)
    {
        CHAR sn_buf[PZM_SN_LEN + 1];
        sg_curr_pzm_fpd_obj->m_hptr_COM_GetFPsnEx(npara, sn_buf);
        sn_buf[PZM_SN_LEN] = '\0';
        DIY_LOG(LOG_INFO, QString("PZM: broken sn: %1: %2").arg(npara).arg(sn_buf));
        emit sg_curr_pzm_fpd_obj->pzm_fpd_comm_sig(EVENT_LINKUPEX, npara, QString(sn_buf));
        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

BOOL WINAPI CPZM_Fpd::FuncHeartBeatCallBack(char nEvent)
{/*EVENT_HEARTBEAT*/
    static const int PZM_HB_INTERMAL = 5 * 2; //this cb is called every 500ms.
    static int hb_int_cnt;

    if(hb_int_cnt < PZM_HB_INTERMAL)
    {
        ++hb_int_cnt;
        return TRUE;
    }
    hb_int_cnt = 0;

    DIY_LOG(LOG_INFO, QString("PZM: FuncHeartBeatCallBack(%1)").arg(nEvent));
    if(sg_curr_pzm_fpd_obj)
    {
        CHAR fp_curr_status;
        BOOL api_ret;
        memset(&sg_tFPStat, 0, sizeof(sg_tFPStat));
        api_ret = sg_curr_pzm_fpd_obj->m_hptr_COM_GetFPStatus(&sg_tFPStat);
        if(!api_ret)
        {
            DIY_LOG(LOG_ERROR, "PZM: Get FPStatus error.");
        }

        fp_curr_status = sg_curr_pzm_fpd_obj->m_hptr_COM_GetFPCurStatus();
        DIY_LOG(LOG_INFO, QString("PZM: heart break, current status: %1:%2")
                .arg(fp_curr_status).arg(sg_PZM_status_str_map[(int)fp_curr_status]));

        /*processing........*/
        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

BOOL WINAPI CPZM_Fpd::FuncHeartBeatexCallBack(char npara)
{/*EVENT_HEARTBEATEX*/
    DIY_LOG(LOG_INFO, QString("PZM: FuncHeartBeatexCallBack(%1)").arg(npara));
    if(sg_curr_pzm_fpd_obj)
    {
        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

#define PX_BITS_TO_BYTES(p) (((p)%8)==0 ? ((p)/8) : (((p)-((p)%8)/8) + 1));

BOOL WINAPI CPZM_Fpd::FuncImageCallBack(char nEvent)
{/*EVENT_IMAGEVALID*/
    DIY_LOG(LOG_INFO, QString("PZM: FuncImageCallBack(%1)").arg(nEvent));
    if(sg_curr_pzm_fpd_obj)
    {
        BOOL api_ret;
        TImageMode tImageMode;
        api_ret = sg_curr_pzm_fpd_obj->m_hptr_COM_GetImageMode(&tImageMode);
        if(!api_ret)
        {
            DIY_LOG(LOG_ERROR, "PZM: get Image mode error");
            return FALSE;
        }

        int img_w = tImageMode.usCol; // sg_curr_pzm_fpd_obj->m_model_info->img_info.w;
        int img_h = tImageMode.usRow; //sg_curr_pzm_fpd_obj->m_model_info->img_info.h;
        int img_px_bytes = PX_BITS_TO_BYTES(tImageMode.usPix); //PX_BITS_TO_BYTES(sg_curr_pzm_fpd_obj->m_model_info->img_info.bits);
        DIY_LOG(LOG_INFO,
                QString("PZM: get image mode: w: %1, h: %2, px_bytes: %3; type: %4")
                .arg(img_w).arg(img_h).arg(img_px_bytes).arg(tImageMode.usType));

        qint64 img_size = img_w * img_h * img_px_bytes;
        CHAR* img_buf = new CHAR[img_size];
        if(!img_buf)
        {
            DIY_LOG(LOG_ERROR, "PZM: allocate memory for image fail!");
            return FALSE;
        }
        api_ret = sg_curr_pzm_fpd_obj->m_hptr_COM_GetImage(img_buf);
        if(!api_ret)
        {
            DIY_LOG(LOG_ERROR, "PZM GetImage error.");
            delete []img_buf;
            return FALSE;
        }
        DIY_LOG(LOG_INFO, "PZM: Get Image sucess.");
        sg_curr_pzm_fpd_obj->m_hptr_COM_Stop();

        /*
         * NOTE: img_buf is to be delete by the slot.
        */
        emit sg_curr_pzm_fpd_obj->pzm_fpd_img_received_sig(img_buf, img_w, img_h, img_px_bytes *2);

        return TRUE;
    }
    PZM_HANDLER_NOT_EXIST();
}

#undef PZM_HANDLER_NOT_EXIST

/*----------------------------------------------------------------------------*/
//exposed hanlers.
bool CPZM_Fpd::connect_to_fpd(fpd_model_info_t* model)
{
    QString err_str;

    m_model_info = model;
    if(!m_model_info)
    {
        DIY_LOG(LOG_ERROR, "PZM: fpd model pointer can't be null.");
        return false;
    }
    if(!load_library())
    {
        return false;
    }
    DIY_LOG(LOG_INFO, QString("PZM: Load library %1 success.").arg(model->api_lib_pfn));

    if(!resolve_lib_functions())
    {
        unload_library();
        return false;
    }
    DIY_LOG(LOG_INFO, "PZM: Resolve api functions success.");

    sg_curr_pzm_fpd_obj = this;

    bool ret;
    ret = reg_pzm_callbacks();
    if(!ret)
    {
        unload_library();
        return false;
    }
    DIY_LOG(LOG_INFO, "PZM: Register callback functions success.");

    BOOL api_ret;
    QString curr_app_abs_pth = QDir::currentPath();//QCoreApplication::applicationDirPath();
    QString file_abs_pth = curr_app_abs_pth + "/" + m_model_info->cfg_file_pth;
    QByteArray ba = file_abs_pth.toUtf8();
    char * pth = ba.data();
    api_ret = m_hptr_COM_SetCfgFilePath(pth);
    if(!api_ret)
    {
        DIY_LOG(LOG_ERROR,
                QString("PZM: Set cfg file path error: %1")
                .arg(m_model_info->cfg_file_pth));
        unload_library();
        return false;
    }
    DIY_LOG(LOG_INFO, QString("PZM: Set cfg file path: %1").arg(QString(pth)));

    file_abs_pth = curr_app_abs_pth + "/" + m_model_info->log_file_pth;
    ba = file_abs_pth.toUtf8();
    pth = ba.data();
    api_ret = m_hptr_COM_LogPathSet(pth);
    if(!api_ret)
    {
        DIY_LOG(LOG_WARN, QString("PZM: Set log file path error: %1").arg(m_model_info->log_file_pth));
    }
    DIY_LOG(LOG_INFO, QString("PZM: Set image file path: %1").arg(QString(pth)));

    file_abs_pth = curr_app_abs_pth + "/" + m_model_info->img_file_pth;
    ba = file_abs_pth.toUtf8();
    pth = ba.data();
    api_ret = m_hptr_COM_ImgPathSet(pth);
    if(!api_ret)
    {
        DIY_LOG(LOG_WARN, QString("PZM: Set image file path error: %1").arg(m_model_info->img_file_pth));
    }

    api_ret = m_hptr_COM_Init();
    if(!api_ret)
    {
        DIY_LOG(LOG_ERROR, "PZM: COM_Init fails.");
        unload_library();
        return false;
    }
    api_ret = m_hptr_COM_Open(nullptr);
    if(!api_ret)
    {
        DIY_LOG(LOG_ERROR, "PZM: COM_Open error!");
        return false;
    }
    DIY_LOG(LOG_INFO, "PZM: fpd opened.");

    api_ret = m_hptr_COM_SetCalibMode(IMG_CALIB_GAIN | IMG_CALIB_DEFECT);
    if(!api_ret)
    {
        DIY_LOG(LOG_WARN, "PZM: SetCaliMode error!");
    }
    else
    {
        DIY_LOG(LOG_INFO, "PZM: SetCaliMode ok!");
    }
    return true;
}

bool CPZM_Fpd::disconnect_from_fpd(fpd_model_info_t* /*model*/)
{
    BOOL api_ret;
    api_ret = m_hptr_COM_Close();
    if(!api_ret)
    {
        DIY_LOG(LOG_ERROR, "PZM: Close failes");
        return false;
    }
    DIY_LOG(LOG_INFO, "PZM: Closed.");

    api_ret = m_hptr_COM_Uninit();
    if(!api_ret)
    {
        DIY_LOG(LOG_ERROR, "PZM: Uninit failes");
        return false;
    }
    DIY_LOG(LOG_INFO, "PZM: Uninited.");

    return unload_library();
}

bool CPZM_Fpd::start_aed_acquiring()
{
    CHAR fp_curr_status;
    BOOL api_ret;

    fp_curr_status = m_hptr_COM_GetFPCurStatus();
    bool ret = true;
    switch(fp_curr_status)
    {
        case STATUS_IDLE:
        {
            DIY_LOG(LOG_INFO, "PZM: fpd is idle, now start aed acquiring.");
            api_ret = m_hptr_COM_AedAcq();
            if(!api_ret)
            {
                DIY_LOG(LOG_ERROR, "PZM: COM_AedAcq error!");
                ret = false;
            }
        }
        break;

        default:
        {
            DIY_LOG(LOG_ERROR,
                    QString("PZM: AedAcq works only in idle status, but current status is %1:%2.")
                    .arg(fp_curr_status).arg(sg_PZM_status_str_map[(int)fp_curr_status]));
            ret = false;
        }
        break;
    }

    return ret;
}
