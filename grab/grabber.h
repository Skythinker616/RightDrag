/*屏幕抓取*/
#ifndef GRABBER_H
#define GRABBER_H

#include <QObject>
#include "qpixmap.h"
#include "floatdialog.h"

class Grabber : public QObject
{
    Q_OBJECT
public:
    struct{
        bool ctrl,shift,alt;
    }combineKey;//组合键使能
    bool blockMouseEvent;//是否拦截鼠标事件
    static bool mouseBlocking;//若为true则右键按下和抬起事件都会拦截，由于需要在钩子函数使用故为static
    explicit Grabber(QObject *parent = nullptr);
    ~Grabber();
    static void sigMouseMoveEmit();//三个静态函数用于在钩子中释放信号
    static void sigMouseDownEmit();
    static void sigMouseUpEmit();
    void setRectColor(QColor &color);//设置选框样式
    void setRectStyle(Qt::PenStyle style);
    void setRectWidth(int width);

private:
    QPixmap pixmap;//暂存截到的图片
    struct{
        bool isPressing;//右键是否有效按下(组合键也应符合)
        bool isDragging;//是否正在有效拖拽
        QPoint start,end;//开始和结束拖拽的坐标
    }dragInfo;//拖拽信息
    FloatDialog *floatDialog;//选框窗口

    void screenshot();//进行截图
    QRect* pointToRect(QPoint &p1,QPoint &p2);//由两个点生成矩形
    bool combineKeyCorrect();//检查组合键是否正确

signals:
    void sigMouseMove();
    void sigMouseDown();
    void sigMouseUp();
    void sigGrabFinished(QPixmap &pixmap);//截图完成信号

private slots:
    void slotMouseMove();
    void slotMouseDown();
    void slotMouseUp();
};

#endif // GRABBER_H
