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
        int area_w = this->width(), area_h = this->height();
        painter.setWindow(-1 * area_w/2, -1 * area_h/2, area_w, area_h);

        int img_w = Image.width(), img_h = Image.height();
        m_img_wnd_rect.setRect(-1 * img_w/2, -1 * img_h/2, img_w, img_h);
        m_img_wnd_ori_trans.reset();
        m_img_wnd_ori_trans.translate(-1 * m_img_wnd_rect.left(), -1 * m_img_wnd_rect.top());

        QTransform img_world_trans;
        img_world_trans.rotate(AngleValue);
        QRect img_fit_rotate_rect = img_world_trans.mapRect(m_img_wnd_rect);
        QSizeF img_fit_rotate_scale_size(img_fit_rotate_rect.width(),
                                         img_fit_rotate_rect.height());
        img_fit_rotate_scale_size.scale(QSizeF(area_w, area_h), Qt::KeepAspectRatio);
        float f_r_s_width = img_fit_rotate_scale_size.width(),
              f_r_s_height = img_fit_rotate_scale_size.height();

        float fit_area_scale_x = 1.0 * f_r_s_width / img_fit_rotate_rect.width();
        float fit_area_scale_y = 1.0 * f_r_s_height / img_fit_rotate_rect.height();

        img_world_trans.reset();
        img_world_trans.translate(XPtInterval, YPtInterval);
        img_world_trans.scale(ZoomValue * fit_area_scale_x, ZoomValue * fit_area_scale_y);
        img_world_trans.rotate(AngleValue);

        painter.setTransform(img_world_trans);

        // 绘制图像
        painter.drawImage(m_img_wnd_rect, Image);

        float dpr = this->devicePixelRatioF();
        QTransform dpr_invert_trans;
        dpr_invert_trans.scale(1/dpr, 1/dpr);
        m_combined_trans = painter.combinedTransform() * dpr_invert_trans;
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
    update_px_info_lbl(pos_x, pos_y);

    if (!Pressed || !m_img_loaded)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    int xPtInterval = pos.x() - OldPos.x();
    int yPtInterval = pos.y() - OldPos.y();

    if(operateType==Enm_OperateType::Enm_WWWL_Operat)
    {
        //WW += xPtInterval*qAbs(xPtInterval)*5;
        //WL += yPtInterval*qAbs(yPtInterval)*5;
        WW += xPtInterval*10;
        WL += yPtInterval*10;
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
        Image=PrimImage.copy();
        if(invertState)
        {
            Image.invertPixels();
        }
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
        QTransform inverted_trans = m_combined_trans.inverted(&inverted);
        if(!inverted)
        {
            DIY_LOG(LOG_ERROR, "Image combined transform is singular...");
            return;
        }
        /* This part of coordinates-transform MUST be coordinated with that in
         * paintEvent function.
         */
        int wnd_mouse_x, wnd_mouse_y, img_data_x, img_data_y;
        inverted_trans.map(mouse_x, mouse_y, &wnd_mouse_x, &wnd_mouse_y);
        m_img_wnd_ori_trans.map(wnd_mouse_x, wnd_mouse_y, &img_data_x, &img_data_y);
        if(m_img_wnd_rect.contains(wnd_mouse_x, wnd_mouse_y))
        {
            m_img_wnd_ori_trans.map(wnd_mouse_x, wnd_mouse_y, &img_data_x, &img_data_y);
            if(PrimImage.valid(img_data_x, img_data_y))
            {
                info_s = QString("x=%1,y=%2\n") .arg(img_data_x).arg(img_data_y);
                quint64 px_idx = quint64(img_data_y * PrimImage.width() + img_data_x);
                if(PrimImage.format() == QImage::Format_Grayscale16)
                {
                    const quint16* px_ptr = (const quint16*)PrimImage.constBits();
                    quint16 px_value = *(px_ptr + px_idx);
                    info_s += QString("value:%1").arg(px_value);
                }
                else
                {//assuming 8 bit.
                    const quint8* px_ptr = (const quint8*)PrimImage.constBits();
                    quint8 px_value = *(px_ptr + px_idx);
                    info_s += QString("value:%1").arg(px_value);
                }
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
        if(m_img_sn.isEmpty())
        {
            info_s = "";
        }
        else
        {
            quint32 min_val, max_val;
            getImagePixelsMaxMin(PrimImage, max_val, min_val);
            info_s = QString("w:%1,h:%2\nmin:%3,max:%4")
                    .arg(PrimImage.width()).arg(PrimImage.height())
                    .arg(min_val).arg(max_val);
        }
        emit imgInfoDisplay(info_s, m_img_sn);
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

    m_img_loaded = true;
    m_img_sn = QFileInfo(LocalFileName).baseName();
    setMouseTracking(true);
    display_img_info();

    /*
    Image=PrimImage.copy();
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    contrastRatio = 0;
    brightRatio = 0;
    set_def_WWWL();
    update();
    */
    resetImage();

    emit imageLoaded();

}

void ImageLabel::loadImage(QImage img, QString img_sn, bool clear_img, bool invert)
{
    PrimImage=img;

    // int max,min;
    //  getImagePixelsMaxMin(PrimImage,max,min);
    //  PrimImage=get8BitImage(PrimImage,max,min);

    /*
    Image=PrimImage.copy();
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    contrastRatio = 0;
    brightRatio = 0;
    set_def_WWWL();
    */
    m_img_loaded = !clear_img;
    if(clear_img)
    {
        setMouseTracking(false);
    }
    else
    {
        setMouseTracking(true);
    }
    m_img_sn = img_sn;
    display_img_info();

    resetImage(invert);

    //update();
    emit imageLoaded();
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
    /*
    PrimImage.invertPixels();
    Image=PrimImage.copy();
    */
    Image.invertPixels();
    set_def_WWWL();
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
void ImageLabel::resetImage(bool invert)
{
    Image=PrimImage.copy();
    invertState = invert;
    if(invertState)
    {
        Image.invertPixels();
    }
    /*
    WW=65447;
    WL=32810;
    */
    set_def_WWWL();
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
    if(Img.format()==QImage::Format_Grayscale16){
        max = 0; min = 65535;
        unsigned short int * data = (unsigned short int *)Img.bits();
        for(int i = 0; i < pixels; i++)
        {
            max = ((max<*(data+i)) && (*(data+i) <= max_thr)) ? * (data+i) : max;
            min = ((min>*(data+i)) && (*(data+i) >= min_thr)) ? * (data+i) : min;
        }
    }
    else
    {//assuming 8 bit
        max = 0; min = 255;
        uchar *data = Img.bits();
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

void ImageLabel::set_def_WWWL()
{
    DIY_LOG(LOG_INFO, QString("ori WW, WL: %1, %2").arg(WW).arg(WL));
    //calculate ww and wl.
    quint32 min_val, max_val;
    getImagePixelsMaxMin(Image, max_val, min_val);
    WW = max_val - min_val;
    WL = (max_val + min_val) / 2;

    DIY_LOG(LOG_INFO, QString("updated WW, WL: %1, %2").arg(WW).arg(WL));

    Image=getWWWLImage(Image,WW,WL);
    emit wwwlChanged(WW,WL);
}
