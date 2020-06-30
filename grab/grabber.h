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
    }combineKey;
    explicit Grabber(QObject *parent = nullptr);
    ~Grabber();
    static void sigMouseMoveEmit();
    static void sigMouseDownEmit();
    static void sigMouseUpEmit();
    void setRectColor(QColor &color);
    void setRectStyle(Qt::PenStyle style);
    void setRectWidth(int width);

private:
    QPixmap pixmap;
    struct{
        bool isPressing;
        bool isDragging;
        QPoint start,end;
    }dragInfo;
    FloatDialog *floatDialog;
    void screenshot();
    QRect* pointToRect(QPoint &p1,QPoint &p2);
    bool combineKeyCorrect();

signals:
    void sigMouseMove();
    void sigMouseDown();
    void sigMouseUp();
    void sigGrabFinished(QPixmap &pixmap);

private slots:
    void slotMouseMove();
    void slotMouseDown();
    void slotMouseUp();
};

#endif // GRABBER_H
