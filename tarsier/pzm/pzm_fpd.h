#ifndef PZM_FPD_H
#define PZM_FPD_H

#include "pzm/sdk_4.1.16/ComApi/NetCom.h"
#include "../fpdmodels.h"
#include <QObject>
#include <QLibrary>

class CPZM_Fpd: public QObject
{
    Q_OBJECT
public:
    /*model can't be nullptr when constructing.*/
    CPZM_Fpd(QObject *parent = nullptr, fpd_model_info_t* model = nullptr);
    ~CPZM_Fpd();

private:
    QLibrary *m_api_lib = nullptr;
    fpd_model_info_t* m_model_info = nullptr; //this var is set by parent, so do not delete it here.

private:
    BOOL init_lib();

signals:
    void fpdErrorOccurred(QString errorInfo);
};

#endif // PZM_FPD_H
