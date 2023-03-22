#include "imageoperation.h"
#include <QDebug>
#include <QMetaType>

ImageOperation::ImageOperation(QObject *parent) : QObject(parent){
    //dllLoadStatus=false;
    qLib=new QLibrary();

    qRegisterMetaType<img_op_type_t>();
}

ImageOperation::~ImageOperation(){
    FreeTiffLibrary();
    //dllLoadStatus=NULL;
    delete qLib;
    qLib=NULL;
}

/**
 * @brief ImageOperation::LoadTiffLibrary 载入dll
 * @return true：成功 false：失败
 */
bool ImageOperation::LoadTiffLibrary(){
    //qLib=new QLibrary("TiffParse");
    qLib->setFileName("TiffParse");
    if(qLib->load()){
        qDebug()<<tr("----Load TiffParse DLL Success----");
        fCreateImageMng=(CreateImageMng)qLib->resolve("CreateImageMng");
        fReleaseImageMng=(ReleaseImageMng)qLib->resolve("ReleaseImageMng");
        fTiffClose=(TiffClose)qLib->resolve("TiffClose");
        fTiffGetImageCount=(TiffGetImageCount)qLib->resolve("TiffGetImageCount");
        fTiffOpen = (TiffOpen)qLib->resolve("TiffOpen");
        fTiffReadImage = (TiffReadImage)qLib->resolve("TiffReadImage");
        fTiffWriteImage = (TiffWriteImage)qLib->resolve("TiffWriteImage");
        if(fCreateImageMng==NULL || fReleaseImageMng==NULL || fTiffClose==NULL || fTiffGetImageCount==NULL
                || fTiffOpen==NULL || fTiffReadImage==NULL || fTiffWriteImage==NULL){
            emit imageOperationErrorOccurred("获得TiffParse.dll方法失败！");
            return false;
        }
        //dllLoadStatus=true;
        return true;
    }else{
        qDebug()<<tr("----Load TiffParse DLL Success----");
        emit imageOperationErrorOccurred("载入TiffParse.dll失败！");
        return false;
    }
}


/**
 * @brief ImageOperation::FreeLibrary 卸载dll
 */
void ImageOperation::FreeTiffLibrary(){
    if(qLib->isLoaded()){
        qLib->unload();
        //dllLoadStatus=false;
    }
}


/**
 * @brief ImageOperation::SaveTiffFile 保存tiff图片到指定位置
 * @param path 保存路径
 * @param iri IRayImage结构体数据指针
 * @return true：成功 false：失败
 */
bool ImageOperation::SaveTiffFile(QString path, IRayImage *iri){
    if(!(qLib->isLoaded())){
        LoadTiffLibrary();
    }
    int * pMng=fCreateImageMng();
    const char * pt=path.toStdString().c_str();
    if(fTiffOpen(pMng,pt,1)){
        fTiffWriteImage(pMng, iri);
        fTiffClose(pMng);
    }else{
        emit imageOperationErrorOccurred("保存文件失败");
        return false;
    }
    fReleaseImageMng(pMng);
    return true;
}

