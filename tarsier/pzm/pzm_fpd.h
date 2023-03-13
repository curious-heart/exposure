#ifndef PZM_FPD_H
#define PZM_FPD_H

#include "pzm/sdk_4.1.16/ComApi/NetCom.h"
#include "../fpdmodels.h"
#include <QObject>
#include <QLibrary>

#define PZM_SN_LEN (32)
/*handler function type*/
typedef BOOL (*Fnt_COM_Init)();
typedef BOOL (*Fnt_COM_Uninit)();
typedef BOOL (*Fnt_COM_SetCfgFilePath)(CHAR* path);
typedef BOOL (*Fnt_COM_List)(TComFpList* ptComFpList);
typedef BOOL (*Fnt_COM_ListAdd)(CHAR* psn);
typedef BOOL (*Fnt_COM_ListDel)(CHAR* psn);
typedef BOOL (*Fnt_COM_ListClr)();
typedef BOOL (*Fnt_COM_Open)(CHAR *psn);
typedef BOOL (*Fnt_COM_Close)();
typedef BOOL (*Fnt_COM_StopNet)();
typedef BOOL (*Fnt_COM_StartNet)();
typedef BOOL (*Fnt_COM_RegisterEvCallBack)(CHAR nEvent,FP_EVENT_CALLBACK funcallback);
typedef BOOL (*Fnt_COM_SetPreCalibMode)(CHAR nCalMode);
typedef CHAR (*Fnt_COM_GetPreCalibMode)();
typedef BOOL (*Fnt_COM_SetCalibMode)(CHAR nCalMode);
typedef CHAR (*Fnt_COM_GetCalibMode)();
typedef BOOL (*Fnt_COM_AedAcq)();
typedef BOOL (*Fnt_COM_Trigger)();
typedef BOOL (*Fnt_COM_Prep)();
typedef BOOL (*Fnt_COM_Acq)();
typedef BOOL (*Fnt_COM_PrepAcq)();
typedef BOOL (*Fnt_COM_SetAcq)();
typedef BOOL (*Fnt_COM_ComAcq)();
typedef BOOL (*Fnt_COM_ExposeReq)();
typedef BOOL (*Fnt_COM_AedTrigger)();
typedef BOOL (*Fnt_COM_Stop)();
typedef BOOL (*Fnt_COM_Dst)();
typedef BOOL (*Fnt_COM_Dacq)();
typedef BOOL (*Fnt_COM_Cbct)();
typedef BOOL (*Fnt_COM_Dexit)();
typedef UINT32 (*Fnt_COM_GetNumOffLineImg)();
typedef BOOL (*Fnt_COM_GetImageMode)(TImageMode* ptImageMode);
typedef BOOL (*Fnt_COM_GetImageShiftMode)(TImageShiftMode* ptImageShiftMode);
typedef BOOL (*Fnt_COM_GetImageName)(CHAR* name);
typedef BOOL (*Fnt_COM_GetImageID)(UINT32 *pimgID);
typedef BOOL (*Fnt_COM_GetImage)(CHAR* pImageBuff);
typedef BOOL (*Fnt_COM_GetFPsn)(CHAR* psn);
typedef BOOL (*Fnt_COM_GetFPsnEx)(CHAR index, CHAR* psn);
typedef CHAR (*Fnt_COM_GetFPCurStatus)();
typedef CHAR (*Fnt_COM_GetFPCurStatusEx)(CHAR* psn);

class CPZM_Fpd: public QObject
{
    Q_OBJECT
public:
    /*model can't be nullptr when constructing.*/
    CPZM_Fpd(QObject *parent = nullptr);
    ~CPZM_Fpd();
    bool connect_to_fpd(fpd_model_info_t* model);
    bool disconnect_from_fpd(fpd_model_info_t* model);

    /**/
    static BOOL WINAPI FuncLinkexCallBack(char npara); /*EVENT_LINKUPEX*/
    static BOOL WINAPI FuncLinkCallBack(char nEvent); /*EVENT_LINKUP*/
    static BOOL WINAPI FuncBreakexCallBack(char npara); /*EVENT_LINKDOWNEX*/

private:
    QLibrary *m_api_lib = nullptr;
    fpd_model_info_t* m_model_info = nullptr; //this var is set by parent, so do not delete it here.

private:
    void unload_library();
    bool resolve_lib_functions();
    bool reg_pzm_callbacks();

signals:
    void fpdErrorOccurred(QString errorInfo);
    void pzm_fpd_comm_sig(int evt, int sn_id = -1, QString sn_str = "");
    void pzm_fpd_img_received_sig(CHAR* img, int width, int height, int bit_dep);

private:
    /*api function name str*/
    static constexpr const char* m_hstr_COM_Init = "COM_Init";
    static constexpr const char* m_hstr_COM_Uninit = "COM_Uninit";
    static constexpr const char* m_hstr_COM_SetCfgFilePath= "COM_SetCfgFilePath";
    static constexpr const char* m_hstr_COM_List = "COM_List";
    static constexpr const char* m_hstr_COM_Open = "COM_Open";
    static constexpr const char* m_hstr_COM_Close = "COM_Close";
    static constexpr const char* m_hstr_COM_StartNet = "COM_StartNet";
    static constexpr const char* m_hstr_COM_StopNet = "COM_StopNet";
    static constexpr const char* m_hstr_COM_RegisterEvCallBack = "COM_RegisterEvCallBack";
    static constexpr const char* m_hstr_COM_SetCalibMode = "COM_SetCalibMode";
    static constexpr const char* m_hstr_COM_GetFPsn = "COM_GetFPsn";
    static constexpr const char* m_hstr_COM_GetFPsnEx = "COM_GetFPsnEx";
    static constexpr const char* m_hstr_COM_GetFPCurStatus = "COM_GetFPCurStatus";
    static constexpr const char* m_hstr_COM_GetFPCurStatusEx = "COM_GetFPCurStatusEx";
    static constexpr const char* m_hstr_COM_GetImageMode = "COM_GetImageMode";
    static constexpr const char* m_hstr_COM_GetImage = "COM_GetImage";
    static constexpr const char* m_hstr_COM_AedAcq = "COM_AedAcq";
    static constexpr const char* m_hstr_COM_AedTrigger = "COM_AedTrigger";
    static constexpr const char* m_hstr_COM_Stop = "COM_Stop";
    static constexpr const char* m_hstr_COM_ExposeReq = "COM_ExposeReq";

    /*api function poiter*/
    Fnt_COM_Init m_hptr_COM_Init;
    Fnt_COM_Uninit m_hptr_COM_Uninit;
    Fnt_COM_SetCfgFilePath m_hptr_COM_SetCfgFilePath;
    Fnt_COM_List m_hptr_COM_List;
    Fnt_COM_Open m_hptr_COM_Open;
    Fnt_COM_Close m_hptr_COM_Close;
    Fnt_COM_StartNet m_hptr_COM_StartNet;
    Fnt_COM_StopNet m_hptr_COM_StopNet;
    Fnt_COM_RegisterEvCallBack m_hptr_COM_RegisterEvCallBack;
    Fnt_COM_SetCalibMode m_hptr_COM_SetCalibMode;
    Fnt_COM_GetFPsn m_hptr_COM_GetFPsn;
    Fnt_COM_GetFPsnEx m_hptr_COM_GetFPsnEx;
    Fnt_COM_GetFPCurStatus m_hptr_COM_GetFPCurStatus;
    Fnt_COM_GetFPCurStatusEx m_hptr_COM_GetFPCurStatusEx;
    Fnt_COM_GetImageMode m_hptr_COM_GetImageMode;
    Fnt_COM_GetImage m_hptr_COM_GetImage;
    Fnt_COM_AedAcq m_hptr_COM_AedAcq;
    Fnt_COM_AedTrigger m_hptr_COM_AedTrigger;
    Fnt_COM_Stop m_hptr_COM_Stop;
    Fnt_COM_ExposeReq m_hptr_COM_ExposeReq;
};
#endif // PZM_FPD_H
