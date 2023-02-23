#ifndef IMAGELABEL_H
#define IMAGELABEL_H


#include <QObject>
#include <QLabel>
#include <QGestureEvent>

enum Enm_OperateType
{
    Enm_No_Operat = 0,
    Enm_WWWL_Operat = 1,
    Enm_Zoom_Operat = 2,
    Enm_Translation_Operate = 3
};

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);

protected:
    bool event(QEvent *e) override;                             //事件
    bool gestureEvent(QGestureEvent *event);                    //处理手势事件
    void contextMenuEvent(QContextMenuEvent *event) override;   //右键菜单
    void paintEvent(QPaintEvent *event) override;               //QPaint画图
    void wheelEvent(QWheelEvent *event) override;               //鼠标滚轮滚动
    void mousePressEvent(QMouseEvent *event) override;          //鼠标摁下
    void mouseMoveEvent(QMouseEvent *event) override;           //鼠标松开
    void mouseReleaseEvent(QMouseEvent *event) override;        //鼠标发射事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;    //鼠标双击事件

public slots:
    void OnSelectImage();                   //选择打开图片
    void loadImage(QImage img);             //载入图片
    void OnZoomInImage();                   //图片放大
    void OnZoomOutImage();                  //图片缩小
    void OnPresetImage();                   //图片还原
    void contrastAddImage();                //对比度+
    void contrastSubImage();                //对比度-
    void brightAddImage();                  //亮度+
    void brightSubImage();                  //亮度-
    void setContrastImage(float contrast);  //设置对比度
    void setBrightImage(float bright);      //设置亮度
    bool saveImage(QString path);           //保存图片
    void rotateImage();                     //旋转图片
    void antiColorImage(bool state);        //反色图片
    void operateImage(Enm_OperateType operate);//操作图片
    void resetImage();                      //重置图片

private:
    void getImagePixelsMaxMin(QImage Img,int &max,int &min);//获得图片最大值和最小值
    QImage get8BitImage(QImage Img,int max,int min);//通过最大最小值转换图片为8位图
    QImage getWWWLImage(QImage Img,int ww,int wl);//获得窗宽窗位图
    QImage AdjustContrastAndBrightness(QImage Img, float iContrastCoefficient, float iBrightCoefficient);//调整亮度和对比度

signals:
    void imageLoaded();     //图片载入完成
    void wwwlChanged(int ww,int wl);     //WWWL改变

private:
    QImage PrimImage;       //未修改的图片
    //QImage InvertImage;     //其他未修改仅仅反色的图片
    QImage Image;           //显示的图片
    qreal ZoomValue = 1.0;  //鼠标缩放值
    int XPtInterval = 0;    //平移X轴的值
    int YPtInterval = 0;    //平移Y轴的值
    QPoint OldPos;          //旧的鼠标位置
    bool Pressed = false;   //鼠标是否被摁压
    QString LocalFileName;  //图片名
    float contrastRatio = 0;//对比度系数
    float brightRatio = 0;  //亮度系数
    qreal AngleValue=0;     //旋转角度
    int numberOfTimes=0;    //旋转次数
    bool invertState=false; //反色状态
    int WW=65447;               //窗宽
    int WL=32810;               //窗位
    Enm_OperateType operateType;//操作类型
};

#endif // IMAGELABEL_H
