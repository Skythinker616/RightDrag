#include "grabber.h"
#include <Windows.h>
#include "qcursor.h"
#include "qguiapplication.h"
#include "qscreen.h"

Grabber *pGrabberForSig;
HHOOK mouseHook=NULL;

LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    Q_UNUSED(nCode);
    Q_UNUSED(lParam);
    switch(wParam)
    {
        case WM_MOUSEMOVE:
        Grabber::sigMouseMoveEmit();
        break;
        case WM_RBUTTONDOWN:
        Grabber::sigMouseDownEmit();
        break;
        case WM_RBUTTONUP:
        Grabber::sigMouseUpEmit();
        break;
    }
    return false;
}

Grabber::Grabber(QObject *parent) : QObject(parent)
{
    pGrabberForSig=this;
    mouseHook=SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
    if(mouseHook==NULL) qDebug("hook null");
    floatDialog=new FloatDialog();
    connect(this,SIGNAL(sigMouseMove()),this,SLOT(slotMouseMove()));
    connect(this,SIGNAL(sigMouseDown()),this,SLOT(slotMouseDown()));
    connect(this,SIGNAL(sigMouseUp()),this,SLOT(slotMouseUp()));
}

Grabber::~Grabber()
{
    UnhookWindowsHookEx(mouseHook);
}

void Grabber::sigMouseMoveEmit()
{
    emit pGrabberForSig->sigMouseMove();
}

void Grabber::sigMouseDownEmit()
{
    emit pGrabberForSig->sigMouseDown();
}

void Grabber::sigMouseUpEmit()
{
    emit pGrabberForSig->sigMouseUp();
}

void Grabber::setRectColor(QColor &color)
{
    floatDialog->lineColor=color;
}

void Grabber::setRectStyle(Qt::PenStyle style)
{
    floatDialog->lineSytle=style;
}

void Grabber::setRectWidth(int width)
{
    floatDialog->lineWidth=width;
}

void Grabber::screenshot()
{
    QRect *rect=pointToRect(dragInfo.start,dragInfo.end);
    pixmap=QGuiApplication::primaryScreen()->grabWindow(0,rect->x(),rect->y(),rect->width(),rect->height());
    delete rect;
}

QRect* Grabber::pointToRect(QPoint &p1, QPoint &p2)
{
    int x1=p1.x(),y1=p1.y();
    int x2=p2.x(),y2=p2.y();
    if(x1>x2) qSwap(x1,x2);
    if(y1>y2) qSwap(y1,y2);
    QRect *rect=new QRect(x1,y1,x2-x1,y2-y1);
    return rect;
}

bool Grabber::combineKeyCorrect()
{
    bool res=true;
    if(((GetKeyState(VK_CONTROL)&0x8000)==0x8000)!=combineKey.ctrl) res=false;
    if(((GetKeyState(VK_SHIFT)&0x8000)==0x8000)!=combineKey.shift) res=false;
    if(((GetKeyState(VK_MENU)&0x8000)==0x8000)!=combineKey.alt) res=false;
    return res;
}

void Grabber::slotMouseMove()
{
    if(!dragInfo.isPressing) return;
    if(!combineKeyCorrect())
    {
        if(dragInfo.isDragging)
        {
            dragInfo.isDragging=false;
            dragInfo.end=QCursor::pos();
            floatDialog->hide();
        }
        return;
    }
    if(!dragInfo.isDragging)
    {
        dragInfo.isDragging=true;
        dragInfo.start=QCursor::pos();
        floatDialog->changeShape(dragInfo.start.x(),dragInfo.start.y(),1,1);
        floatDialog->show();
    }
    QPoint nowPos=QCursor::pos();
    QRect *rect=pointToRect(dragInfo.start,nowPos);
    floatDialog->changeShape(rect->x(),rect->y(),rect->width(),rect->height());
    delete rect;
}

void Grabber::slotMouseDown()
{
    dragInfo.isPressing=true;
    //qDebug("start:%d,%d",dragInfo.start.x(),dragInfo.start.y());
}

void Grabber::slotMouseUp()
{
    if(dragInfo.isDragging)
    {
        dragInfo.end=QCursor::pos();
        floatDialog->hide();
        screenshot();
        emit sigGrabFinished(pixmap);
    }
    dragInfo.isPressing=false;
    dragInfo.isDragging=false;
    //qDebug("end:%d,%d",dragInfo.end.x(),dragInfo.end.y());
}
