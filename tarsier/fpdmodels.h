#ifndef FPDMODELS_H
#define FPDMODELS_H

#include <QString>
#include <QMap>

/*
 * All supported FPD models info.
*/

/*series id corresponds to dll to load, and then the api used.*/
typedef enum
{
   FPD_SID_NONE = -1,
   FPD_SID_IRAY_STATIC = 0,
   FPD_SID_PZM_STATIC
}fpd_series_id_type_t;
typedef struct
{
    int w, h;
    int bits;
}fpd_image_info_t;
#define IMG_PIX_NORM_14I_35CM_L1 2304
#define IMG_PIX_NORM_14I_35CM_L2 2560
#define IMG_PIX_NORM_17I_43CM_L1 2800
#define IMG_PIX_NORM_17I_43CM_L2 3072
#define IMG_BITS_DEP_NORM (2*8)

extern const char* FPD_NAME_NONE_INTERNAL_STR;
extern const char* FPD_NAME_NONE_UI_STR;

typedef struct
{
    QString mfg;
    fpd_series_id_type_t sid; //series id
    fpd_image_info_t  img_info;
    int def_trigger_mdde;
    QMap<QString, int> trigger_mode_list; //key: trigger mode name; value: enum or macro value.
    QString api_lib_pfn;
    QString cfg_file_pth, log_file_pth, img_file_pth;
}fpd_model_info_t;

/*PZM dose not have trigger mode definition. So we define it here by ourseleves.*/
typedef enum
{
    PZM_TRIGGER_MODE_AED = 11,
    PZM_TRIGGER_MODE_HST,
}PZM_trigger_mode_t;
#define INVALID_TRIGGER_MODE (-1)
/*key: series string; value: series info.*/
typedef QMap<QString, fpd_model_info_t*> fpd_model_info_list_t;


class CFpdModels
{
public:
    CFpdModels();
    ~CFpdModels();
    fpd_model_info_t* get_fpd_minfo_from_name(QString &n);

private:
    fpd_model_info_list_t m_fpd_model_info_list;
    void clear_fpd_model_list();
    void setup_fpd_model_list();
};

#endif // FPDMODELS_H
