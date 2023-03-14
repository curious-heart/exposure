#include "fpdmodels.h"
#include "IRayInclude.h"
#include "pzm/sdk_4.1.16/ComApi/NetCom.h"
#include "logger.h"

/*
 * All supported FPD models info.
*/

static const char* FPD_MFG_IRAY = "iRay"; //奕瑞
static const char* IRAY_MODEL_MARS1417V3 = "Mars1417V3";
static const char* IRAY_MODEL_MARS1717 = "Mars1717";

static const char* FPD_MFG_PZM = "PZM"; //品臻
static const char* PZM_MODEL_4343Z = "4343Z";
static const char* PZM_MODEL_3543Z = "3543Z";

static const char* FPD_MFG_NONE = "None";
static const char* FPD_MODEL_NONE = FPD_MFG_NONE;
const char* FPD_NAME_NONE_UI_STR = "无";
const char* FPD_NAME_NONE_INTERNAL_STR = FPD_MFG_NONE;

CFpdModels::CFpdModels()
{
    setup_fpd_model_list();
}

CFpdModels::~CFpdModels()
{
    clear_fpd_model_list();
}

void CFpdModels::setup_fpd_model_list()
{
    /*Set fpd information according to series.*/
    fpd_model_info_t* info;
    clear_fpd_model_list();

    /*None----------------------------------------*/
    info = new(fpd_model_info_t);
    if(info)
    {
        info->sid = FPD_SID_NONE;
        info->mfg = FPD_MFG_NONE;
        info->api_lib_pfn = "";
        info->img_info = {0, 0, 0};
        info->trigger_mode_list.clear();
        info->def_trigger_mdde = INVALID_TRIGGER_MODE;
        m_fpd_model_info_list.insert(FPD_MODEL_NONE, info);
    }

    /*iRay----------------------------------------*/
    static const char* TRIGGER_MODE_OUTER = "Outer";
    static const char* TRIGGER_MODE_INNER = "Inner";
    static const char* TRIGGER_MODE_SOFT = "Software";
    static const char* TRIGGER_MODE_PREP = "PREP";
    static const char* TRIGGER_MODE_SRV = "Service";
    static const char* TRIGGER_MODE_FREESYNC = "FreeSync";
    static const char* IRAY_API_LIB_PFN = "FpdSys";
    static const char* IRAY_CFG_FILE_PTH = "";
    static const char* IRAY_LOG_FILE_PTH = "";
    static const char* IRAY_IMG_FILE_PTH = "";
    info = new(fpd_model_info_t);
    if(info)
    {//1417V3
        info->sid = FPD_SID_IRAY_STATIC;
        info->mfg = FPD_MFG_IRAY;
        info->api_lib_pfn = IRAY_API_LIB_PFN;
        info->cfg_file_pth = IRAY_CFG_FILE_PTH;
        info->log_file_pth = IRAY_LOG_FILE_PTH;
        info->img_file_pth = IRAY_IMG_FILE_PTH;
        info->img_info = {IMG_PIX_NORM_14I_35CM_L1, IMG_PIX_NORM_17I_43CM_L1, IMG_BITS_DEP_NORM};
        info->trigger_mode_list.insert(TRIGGER_MODE_OUTER, Enm_TriggerMode_Outer);
        info->trigger_mode_list.insert(TRIGGER_MODE_INNER, Enm_TriggerMode_Inner);
        info->trigger_mode_list.insert(TRIGGER_MODE_SOFT, Enm_TriggerMode_Soft);
        info->trigger_mode_list.insert(TRIGGER_MODE_PREP, Enm_TriggerMode_Prep);
        info->trigger_mode_list.insert(TRIGGER_MODE_SRV, Enm_TriggerMode_Service);
        info->trigger_mode_list.insert(TRIGGER_MODE_FREESYNC, Enm_TriggerMode_FreeSync);
        info->def_trigger_mdde = Enm_TriggerMode_Inner;

        m_fpd_model_info_list.insert(IRAY_MODEL_MARS1417V3, info);
    }
    info = new(fpd_model_info_t);
    if(info)
    {//1717
        info->sid = FPD_SID_IRAY_STATIC;
        info->mfg = FPD_MFG_IRAY;
        info->api_lib_pfn = IRAY_API_LIB_PFN;
        info->cfg_file_pth = IRAY_CFG_FILE_PTH;
        info->log_file_pth = IRAY_LOG_FILE_PTH;
        info->img_file_pth = IRAY_IMG_FILE_PTH;
        info->img_info = {IMG_PIX_NORM_17I_43CM_L1, IMG_PIX_NORM_17I_43CM_L1, IMG_BITS_DEP_NORM};
        info->trigger_mode_list.insert(TRIGGER_MODE_OUTER, Enm_TriggerMode_Outer);
        info->trigger_mode_list.insert(TRIGGER_MODE_INNER, Enm_TriggerMode_Inner);
        info->trigger_mode_list.insert(TRIGGER_MODE_SOFT, Enm_TriggerMode_Soft);
        info->trigger_mode_list.insert(TRIGGER_MODE_PREP, Enm_TriggerMode_Prep);
        info->trigger_mode_list.insert(TRIGGER_MODE_SRV, Enm_TriggerMode_Service);
        info->trigger_mode_list.insert(TRIGGER_MODE_FREESYNC, Enm_TriggerMode_FreeSync);
        info->def_trigger_mdde = Enm_TriggerMode_Inner;

        m_fpd_model_info_list.insert(IRAY_MODEL_MARS1717, info);
    }

    /*PZM----------------------------------------*/
    static const char* TRIGGER_MODE_AED = "AED";
    static const char* TRIGGER_MODE_HST = "HST";
    static const char* PZM_API_LIB_PFN = "pzm/sdk_4.1.16/ComApi/x64/ComApi";
    static const char* PZM_CFG_FILE_PTH = "pzm/sdk_4.1.16";
    static const char* PZM_LOG_FILE_PTH = "pzm_sdk_log";
    static const char* PZM_IMG_FILE_PTH = "pzm_image";
    info = new(fpd_model_info_t);
    if(info)
    {//4343Z
        info->sid = FPD_SID_PZM_STATIC;
        info->mfg = FPD_MFG_PZM;
        info->api_lib_pfn = PZM_API_LIB_PFN;
        info->cfg_file_pth = PZM_CFG_FILE_PTH;
        info->log_file_pth = QString(log_dir_str) + "/" + PZM_LOG_FILE_PTH;
        info->img_file_pth = PZM_IMG_FILE_PTH;
        info->img_info = {IMG_PIX_NORM_17I_43CM_L2, IMG_PIX_NORM_17I_43CM_L2, IMG_BITS_DEP_NORM};
        info->trigger_mode_list.insert(TRIGGER_MODE_AED, PZM_TRIGGER_MODE_AED);
        info->trigger_mode_list.insert(TRIGGER_MODE_HST, PZM_TRIGGER_MODE_HST);
        info->def_trigger_mdde = PZM_TRIGGER_MODE_AED;

        m_fpd_model_info_list.insert(PZM_MODEL_4343Z, info);
    }
    info = new(fpd_model_info_t);
    if(info)
    {//3543Z
        info->sid = FPD_SID_PZM_STATIC;
        info->mfg = FPD_MFG_PZM;
        info->api_lib_pfn = PZM_API_LIB_PFN;
        info->cfg_file_pth = PZM_CFG_FILE_PTH;
        info->log_file_pth = QString(log_dir_str) + "/" + PZM_LOG_FILE_PTH;
        info->img_file_pth = PZM_IMG_FILE_PTH;
        info->img_info = {IMG_PIX_NORM_14I_35CM_L2, IMG_PIX_NORM_17I_43CM_L2, IMG_BITS_DEP_NORM};
        info->trigger_mode_list.insert(TRIGGER_MODE_AED, PZM_TRIGGER_MODE_AED);
        info->trigger_mode_list.insert(TRIGGER_MODE_HST, PZM_TRIGGER_MODE_HST);
        info->def_trigger_mdde = PZM_TRIGGER_MODE_AED;

        m_fpd_model_info_list.insert(PZM_MODEL_3543Z, info);
    }
}

fpd_model_info_t* CFpdModels:: get_fpd_minfo_from_name(QString &n)
{
    fpd_model_info_list_t::iterator it;
    for(it = m_fpd_model_info_list.begin(); it != m_fpd_model_info_list.end(); ++it)
    {
        if(n.contains(it.key()))
        {
            return it.value();
        }
    }
    return nullptr;
}

void CFpdModels::clear_fpd_model_list()
{
    fpd_model_info_list_t::iterator it = m_fpd_model_info_list.begin();

    while(it != m_fpd_model_info_list.end())
    {
        delete it.value();
        ++it;
    }
    m_fpd_model_info_list.clear();
}
