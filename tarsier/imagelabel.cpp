#include "imagelabel.h"
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QMenu>
#include <QFileDialog>
#include <QGestureEvent>
#include <QList>
#include <QtMath>
#include "logger.h"

ImageLabel::ImageLabel(QWidget *parent):QLabel(parent)
{
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);

    /*
    m_px_info_lbl.setParent(this);
    m_px_info_lbl.setFrameShape(QFrame::NoFrame);
    */
}

ImageLabel::~ImageLabel()
{
    //m_px_info_lbl.hide();
}

/**
 * @brief ImageLabel::event
 * @param event
 * @return
 */
bool ImageLabel::event(QEvent *event){
    //    if(event->type()==QEvent::TouchBegin){
    //        QTouchEvent* touch = static_cast<QTouchEvent*>(event);
    //        QList<QTouchEvent::TouchPoint> touch_list = touch->touchPoints();
    //        qDebug()<<"---------"<<touch_list.size();
    //        QListIterator<QTouchEvent::TouchPoint> i (touch_list);
    //        while (i.hasNext())
    //            qDebug () <<"----**-----"<< i.next ();
    //    }
    if(event->type()==QEvent::Gesture){
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QWidget::event(event);
}


/**
 * @brief ImageLabel::gestureEvent 处理手势
 * @param event
 * @return
 */
bool ImageLabel::gestureEvent(QGestureEvent *event){
    //双指捏操作
    if(QGesture * pinch=event->gesture(Qt::PinchGesture)){
        QPinchGesture *gesture=static_cast<QPinchGesture*>(pinch);
        QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
        qreal scale=1.0;
        if (changeFlags & QPinchGesture::ScaleFactorChanged)
        {
            scale = gesture->scaleFactor();
        }
        if(scale >= 1.0)
        {
            OnZoomInImage();
        }else if(scale < 1.0)
        {
            OnZoomOutImage();
        }
    }
    return true;
}

//QPainter画图
void ImageLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(m_img_loaded)
    {
        QPainter painter(this);

        /* This part of coordinates-transform MUST be coordinated with that in
         * update_px_info_lbl function.
         */
        int img_w = Image.width(), img_h = Image.height();
        int width = qMin(img_w, this->width());
        int height = width * 1.0 / (img_w * 1.0 / img_h);
        height = qMin(height, this->height());
        width = height * 1.0 * (img_w * 1.0 / img_h);
        m_display_scale_x = 1.0 * width / img_w;
        m_display_scale_y = 1.0 * height / img_h ;


        m_img_trans.reset();
        m_img_trans.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);
        m_img_trans.scale(ZoomValue, ZoomValue);
        m_img_trans.rotate(AngleValue);
        painter.setTransform(m_img_trans);

        // 根据窗口计算应该显示的图片的大小
        /*
        int width = qMin(Image.width(), this->width());
        int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
        height = qMin(height, this->height());
        width = height * 1.0 * (Image.width() * 1.0 / Image.height());

        // 平移
        painter.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);

        // 缩放
        painter.scale(ZoomValue, ZoomValue);

        // 旋转
        painter.rotate(AngleValue);
        */

        // 绘制图像
        QRect picRect(-width / 2, -height / 2, width, height);
        painter.drawImage(picRect, Image);

        int area_w = this->width(), area_h = this->height();
        //m_img_rect.setRect(area_w/2 + picRect.x(), area_h/2 + picRect.y(), width, height);
        m_img_ori_rect = picRect;
        m_img_rect = m_img_trans.mapRect(picRect);
    }
}
//鼠标滚轮滚动
void ImageLabel::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)  //放大
        OnZoomInImage();
    else            //缩小
        OnZoomOutImage();

    update();
}
//鼠标摁下
void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    OldPos = event->pos();
    Pressed = true;
}
//鼠标移动
void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    int pos_x = pos.x(), pos_y = pos.y();
    QString px_info_str = QString("%1,%2").arg(pos_x).arg(pos_y);
    update_px_info_lbl(pos_x, pos_y);

    if (!Pressed)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    int xPtInterval = pos.x() - OldPos.x();
    int yPtInterval = pos.y() - OldPos.y();

    //    qDebug()<<"xPtInterval"<<xPtInterval;
    //    qDebug()<<"yPtInterval"<<yPtInterval;
    if(operateType==Enm_OperateType::Enm_WWWL_Operat){
//        if(!((xPtInterval%10) && (yPtInterval%10)))
//            return;
        WW += xPtInterval*qAbs(xPtInterval)*5;
        WL += yPtInterval*qAbs(yPtInterval)*5;
        if(WL>65535){
            WL=65535;
        }
        else if(WL<1){
            WL=1;
        }

        if(WW>65535){
            WW=65535;
        }
        else if(WW<1){
            WW=1;
        }
        //        if(xPtInterval>0){
        //            WL=WL+2;
        //            int m = WL - WW/2.0;
        //            int n = WL + WW/2.0;
        //            if(WL>255){
        //                WL=255;
        //            }
        //            if(n>255){
        //                WW=(255-WL)*2;
        //            }
        //        }else if(xPtInterval<0){
        //            WL=WL-2;
        //            int m = WL - WW/2.0;
        //            int n = WL + WW/2.0;
        //            if(WL<0){
        //                WL=0;
        //            }
        //            if(m<0){
        //                WW=WL*2;
        //            }
        //        }else if(yPtInterval>0){
        //            WW=WW+2;
        //            int m = WL - WW/2.0;
        //            int n = WL + WW/2.0;
        //            if(WW>255){
        //                WW=255;
        //            }
        //            if(n>255){
        //                WL=255-WW/2.0;
        //            }
        //            if(m<0){
        //                WL=WW/2.0;
        //            }
        //        }else if(yPtInterval<0){
        //            WW=WW-2;
        //            int m = WL - WW/2.0;
        //            int n = WL + WW/2.0;
        //            if(WW<0){
        //                WW=0;
        //            }
        //            if(n>255){
        //                WL=255-WW/2.0;
        //            }
        //            if(m<0){
        //                WL=WW/2.0;
        //            }
        //        }
        Image=PrimImage.copy();
        //Image=getWWWLImage(Image,WW,WL).convertToFormat(QImage::Format_Grayscale8);
        Image=getWWWLImage(Image,WW,WL);
        emit wwwlChanged(WW,WL);
    }else if(operateType==Enm_OperateType::Enm_Zoom_Operat){
        if(yPtInterval>0){
            OnZoomInImage();
        }else if(yPtInterval<0){
            OnZoomOutImage();
        }
    }else if(operateType==Enm_OperateType::Enm_Translation_Operate){
        XPtInterval += xPtInterval;
        YPtInterval += yPtInterval;
    }
    OldPos = pos;
    update();
}
//鼠标松开事件
void ImageLabel::mouseReleaseEvent(QMouseEvent */*event*/)
{
    Pressed = false;
    setCursor(Qt::ArrowCursor);
}


/**
 * @brief ImageLabel::mouseDoubleClickEvent 鼠标双击事件
 * @param event
 */
void ImageLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    OnPresetImage();
}

void ImageLabel::update_px_info_lbl(int mouse_x, int mouse_y)
{
    if(m_img_loaded)
    {
        QString info_s;
        bool inverted;
        QTransform inverted_trans = m_img_trans.inverted(&inverted);
        if(!inverted)
        {
            DIY_LOG(LOG_ERROR, "Image transform is singular...");
            return;
        }
        /* This part of coordinates-transform MUST be coordinated with that in
         * paintEvent function.
         */
        int ori_x, ori_y;
        inverted_trans.map(mouse_x, mouse_y, &ori_x, &ori_y);
        int img_x, img_y;
        img_x = ori_x + m_img_ori_rect.width()/2;
        img_y = ori_y + m_img_ori_rect.height()/2;
        int img_data_x, img_data_y;
        img_data_x = (int)(1.0 * img_x / m_display_scale_x);
        img_data_y = (int)(1.0 * img_y / m_display_scale_y);

        if(m_img_rect.contains(mouse_x, mouse_y))
        {
            if(PrimImage.valid(img_data_x, img_data_y))
            {
                info_s = QString("x=%1,y=%2\n") .arg(img_data_x).arg(img_data_y);
                const quint16* px_ptr = (const quint16*)PrimImage.constBits();
                quint64 px_idx = quint64(img_data_y * PrimImage.width() + img_data_x);
                quint16 px_value = *(px_ptr + px_idx);
                info_s += QString("value:%1").arg(px_value);
            }
            else
            {
                info_s = "";
                DIY_LOG(LOG_ERROR, QString("Image size: %1, %2; pixel (%3, %4) is invalid "
                                           "in this image").arg(PrimImage.width())
                        .arg(PrimImage.height()).arg(img_data_x).arg(img_data_y));
            }
        }
        else
        {
            info_s = "";
        }
        emit pxInfoUpdate(info_s);
    }
    /*
    const int offset_x = 10, offset_y = 0;
    m_px_info_lbl.setText(txt);
    m_px_info_lbl.adjustSize();
    m_px_info_lbl.move(x + offset_x, y + offset_y);
    m_px_info_lbl.show();
    */
}

void ImageLabel::display_img_info()
{
    QString info_s;
    if(m_img_loaded)
    {
        quint32 min_val, max_val;
        getImagePixelsMaxMin(PrimImage, max_val, min_val);
        info_s = QString("w:%1,h:%2\nmin:%3,max:%4")
                .arg(PrimImage.width()).arg(PrimImage.height())
                .arg(min_val).arg(max_val);
        emit imgInfoDisplay(info_s, m_selected_img_fn);
    }
}

//选择打开图片
void ImageLabel::OnSelectImage()
{
    QString LocalFileName = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*.tif)"));
    QFile file(LocalFileName);
    if (!file.exists())
        return;
    PrimImage.load(LocalFileName);

    // int max,min;
    // getImagePixelsMaxMin(PrimImage,max,min);
    // PrimImage=get8BitImage(PrimImage,max,min);

    Image=PrimImage.copy();
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    contrastRatio = 0;
    brightRatio = 0;

    m_selected_img_fn = QFileInfo(LocalFileName).baseName();

    update();
    emit imageLoaded();

    m_img_loaded = true;
    setMouseTracking(true);
    display_img_info();
}

void ImageLabel::loadImage(QImage img, bool clear_img)
{
    PrimImage=img;

    // int max,min;
    //  getImagePixelsMaxMin(PrimImage,max,min);
    //  PrimImage=get8BitImage(PrimImage,max,min);

    Image=PrimImage.copy();
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    contrastRatio = 0;
    brightRatio = 0;

    update();
    emit imageLoaded();
    m_img_loaded = !clear_img;
    if(clear_img)
    {
        setMouseTracking(false);
        m_selected_img_fn = "";
    }
    else
    {
        setMouseTracking(true);
        display_img_info();
    }
}
//图片放大
void ImageLabel::OnZoomInImage()
{
    ZoomValue += 0.025;
    if (ZoomValue >= 5.0)
    {
        ZoomValue = 5.0;
        return;
    }
    update();
}
//图片缩小
void ImageLabel::OnZoomOutImage()
{
    ZoomValue -= 0.025;
    if (ZoomValue <= 0.5)
    {
        ZoomValue = 0.5;
        return;
    }
    update();
}
//图片还原
void ImageLabel::OnPresetImage()
{
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    update();
}

void ImageLabel::contrastAddImage()
{
    contrastRatio=contrastRatio+0.05;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

void ImageLabel::contrastSubImage()
{
    contrastRatio=contrastRatio-0.05;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

void ImageLabel::brightAddImage()
{
    brightRatio=brightRatio+0.1;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

void ImageLabel::brightSubImage()
{
    brightRatio=brightRatio-0.1;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

void ImageLabel::setContrastImage(float contrast)
{
    contrastRatio=contrast;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

void ImageLabel::setBrightImage(float bright)
{
    brightRatio=bright;
    Image=AdjustContrastAndBrightness(PrimImage,contrastRatio,brightRatio);
    update();
}

/**
 * @brief ImageLabel::saveImage 保存图片
 * @param path 图片路径
 */
bool ImageLabel::saveImage(QString path)
{
    return Image.save(path);
}

/**
 * @brief ImageLabel::rotateImage 旋转图片
 */
void ImageLabel::rotateImage()
{
    //方法1  必须配合painter.rotate(AngleValue);
    numberOfTimes++;
    if(numberOfTimes==4){
        numberOfTimes=0;
    }
    AngleValue=90*numberOfTimes;
    update();
    //方法2
    //    QMatrix matrix;
    //    matrix.rotate(90);
    //    Image = Image.transformed(matrix);
    //    update();
}

/**
 * @brief ImageLabel::antiColorImage 反色图片
 * @param state 反色状态 true 反色  false正常
 */
void ImageLabel::antiColorImage(bool state)
{
    //Image.invertPixels();
    invertState=state;
    PrimImage.invertPixels();
    Image=PrimImage.copy();
    update();
}

/**
 * @brief ImageLabel::operateImage 操作图片
 * @param operate 操作的类型
 */
void ImageLabel::operateImage(Enm_OperateType operate)
{
    operateType=operate;
    if(operate==Enm_OperateType::Enm_WWWL_Operat){

    }else if(operate==Enm_OperateType::Enm_Zoom_Operat){

    }else if(operate==Enm_OperateType::Enm_Translation_Operate){

    }
}

/**
 * @brief ImageLabel::resetImage 重置图片
 */
void ImageLabel::resetImage()
{
    if(invertState){
        invertState=false;
        PrimImage.invertPixels();
    }
    Image=PrimImage.copy();
    WW=65447;
    WL=32810;
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    AngleValue=0;
    numberOfTimes=0;
    operateType=Enm_OperateType::Enm_No_Operat;
    update();
}

/**
 * @brief ImageLabel::getImagePixelsMaxMin 获得图片最大值和最小值
 * @param Img 图片
 * @param max 最大值
 * @param min 最小值
 */
void ImageLabel::getImagePixelsMaxMin(QImage Img,quint32 &max, quint32 &min,
                                      quint32 max_thr, quint32 min_thr)
{
    int pixels = Img.width() * Img.height();
    if(Img.format()==QImage::Format_Grayscale8)
    {
        max = 0; min = 255;
        uchar *data = Img.bits();
        for(int i = 0; i < pixels; i++)
        {
            max = ((max<*(data+i)) && (*(data+i) <= max_thr)) ? * (data+i) : max;
            min = ((min>*(data+i)) && (*(data+i) >= min_thr)) ? * (data+i) : min;
        }
    }else if(Img.format()==QImage::Format_Grayscale16){
        max = 0; min = 65535;
        unsigned short int * data = (unsigned short int *)Img.bits();
        for(int i = 0; i < pixels; i++)
        {
            max = ((max<*(data+i)) && (*(data+i) <= max_thr)) ? * (data+i) : max;
            min = ((min>*(data+i)) && (*(data+i) >= min_thr)) ? * (data+i) : min;
        }
    }
}

/**
 * @brief ImageLabel::get8BitImage 通过最大最小值转换图片为8位图
 * @param Img 图片
 * @param max 最大值
 * @param min 最小值
 * @return
 */
QImage ImageLabel::get8BitImage(QImage Img,int max, int min)
{
    int pixels = Img.width() * Img.height();
    if(Img.format()==QImage::Format_Grayscale8){
        uchar *data = Img.bits();
        for(int i = 0; i < pixels; i++){
            *(data+i)=(*(data+i)-min)*255.0/(double)(max - min);
        }
    }else if(Img.format()==QImage::Format_Grayscale16){
        QImage grayImg(Img.width(),Img.height(),QImage::Format_Grayscale8);
        uchar *grayImgDataPtr = grayImg.bits();
        unsigned short int * data = (unsigned short int *)Img.bits();
        for(int i = 0; i < pixels; i++){
            *(grayImgDataPtr+i)=(*(data+i)-min)*255.0/(double)(max - min);
        }
        Img=grayImg;
    }
    return Img;
}

/**
 * @brief ImageLabel::getWWWLImage 获得窗宽窗位图
 * @param Img 图片
 * @param ww 窗宽值
 * @param wl 窗位值
 * @return
 */
QImage ImageLabel::getWWWLImage(QImage Img,int ww, int wl)
{
    //    int min = wl - ww/2.0;
    //    int max = wl + ww/2.0;

//    float a = 256.0 / wl;
//    float b = 128.0 - a * ww;

    //方式一 计算方法不对
    //    int pixels = Img.width() * Img.height();
    //    if(Img.format()==QImage::Format_Grayscale8){
    //        uchar *data = Img.bits();
    //        for(int i = 0; i < pixels; i++){
    //            if (*(data+i) < min){
    //                *(data+i) = 0;
    //                continue;
    //            }
    //            if (*(data+i) > max){
    //                *(data+i) = 255;
    //                continue;
    //            }
    //            *(data+i)=(*(data+i)-min)*255.0/(double)(max - min);
    //        }
    //    }
    //****方式2，获取到的像素不对
    //    for(int i = 0; i < Img.height(); i++){
    //        for(int j = 0; j < Img.width(); j++){
    //            QRgb aa = Img.pixel(j,i);
    //            Img.setPixel(j, i, Img.pixel(j,i) * a + b);
    //        }
    //    }
    //***方式3
//    int pixels = Img.width() * Img.height();
//    unsigned   short *data = (short*)Img.bits();
//    for(int i = 0; i < pixels; i++){
//        *(data+i)=*(data+i) * a + b;
//    }
    // 方式4
    float min = (2 * wl - ww) / 2 + 0.5;
    float max = (2 * wl + ww) / 2 + 0.5;
    float gamma = 1;
    double dFactor = 65535.0 / (double)(max - min);
    int pixels = Img.width() * Img.height();
    unsigned short *data = (unsigned short*)Img.bits();
    for (int i=0; i < pixels; i++) {
        if(*(data+i) < min){
            *(data+i) = 0;
            continue;
        }
        if(*(data+i) > max){
            *(data+i) = 65535;
            continue;
        }

      //  *(data+i)=255.0 * qPow((float)(*(data+i) / (max - min)), 1.0 / gamma);
         *(data+i)=(int)((*(data+i) - min)*dFactor);

        if(*(data+i) < 0){
            *(data+i) = 0;
            continue;
        }
        else if(*(data+i) > 65535){
            *(data+i) = 65535;
            continue;
        }
    }

    return Img;
}

/**
 * @brief ImageLabel::AdjustContrastAndBrightness 调节对比度和亮度
 * @param Img
 * @param iContrastCoefficient 对比度系数 取值范围0到1
 * @param iBrightCoefficient 亮度系数 取值范围-1到1
 * @return
 */
QImage ImageLabel::AdjustContrastAndBrightness(QImage Img, float iContrastCoefficient, float iBrightCoefficient)
{
    if(iContrastCoefficient>1.0){
        iContrastCoefficient=1.0;
    }
    if(iContrastCoefficient<0){
        iContrastCoefficient=0;
    }
    if(iBrightCoefficient>1.0){
        iBrightCoefficient=1.0;
    }
    if(iBrightCoefficient<-1.0){
        iBrightCoefficient=-1.0;
    }
    int pixels = Img.width() * Img.height();
    int gray;
    if(Img.format()==QImage::Format_Grayscale8){
        int iBrightValue=0xff * iBrightCoefficient;
        uchar *data = Img.bits();
        for (int i = 0; i < pixels; ++i)
        {
            gray = *(data+i) + (*(data+i) - 127.5) * iContrastCoefficient * 10 + iBrightValue;
            *(data+i) = (gray < 0x00) ? 0x00 : (gray > 0xff) ? 0xff : gray;
        }
    }else if(Img.format()==QImage::Format_Grayscale16){
        int iBrightValue=0xffff * iBrightCoefficient;
        unsigned short int * data = (unsigned short int *)Img.bits();
        for (int i = 0; i < pixels; ++i)
        {
            gray = *(data+i) + (*(data+i) - 32767.5) * iContrastCoefficient * 10 + iBrightValue;
            *(data+i) = (gray < 0x00) ? 0x00 : (gray > 0xffff) ? 0xffff : gray;
        }
    }
    return Img;
}


//右键菜单
void ImageLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    QAction *loadImage = new QAction(this);
    loadImage->setText("加载测试图片");
    connect(loadImage, &QAction::triggered, this, &ImageLabel::OnSelectImage);
    menu->addAction(loadImage);
    menu->exec(pos);
}
