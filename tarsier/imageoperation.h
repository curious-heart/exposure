#ifndef IMAGEOPERATION_H
#define IMAGEOPERATION_H

#include <QObject>
#include <QLibrary>
#include "include/TiffParser.h"

class ImageOperation : public QObject
{
    Q_OBJECT
public:
    explicit ImageOperation(QObject *parent = nullptr);
    ~ImageOperation();
    bool LoadTiffLibrary();
    void FreeTiffLibrary();
    bool SaveTiffFile(QString path,IRayImage * iri);

private:
    QLibrary *qLib;
    bool dllLoadStatus;
    CreateImageMng fCreateImageMng;
    ReleaseImageMng fReleaseImageMng;
    TiffClose fTiffClose;
    TiffGetImageCount fTiffGetImageCount;
    TiffOpen fTiffOpen;
    TiffReadImage fTiffReadImage;
    TiffWriteImage fTiffWriteImage;


signals:
    /**
     * @brief imageOperationErrorOccurred 图片操作中出现错误
     * @param errorInfo 错误信息
     */
    void imageOperationErrorOccurred(QString errorInfo);
    /**
     * @brief imageSaveFinshed 图片保存完毕
     * @param path 图片路径
     */
    //void imageSaveFinshed(QString path);
    /**
     * @brief imageCreateFinshed 创建图片完成
     * @param img 图片
     * @param imageNum 图片编号
     */
    void imageCreateFinshed(QImage *img,QString imageNum);

};

#endif // IMAGEOPERATION_H
