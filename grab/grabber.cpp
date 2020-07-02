#include "grabber.h"
#include <Windows.h>
#include "qcursor.h"
#include "qguiapplication.h"
#include "qscreen.h"

Grabber *pGrabberForSig;//指向grabber对象，用来在静态函数中emit
HHOOK mouseHook=NULL;//鼠标钩子

bool Grabber::mouseBlocking=false;//初始化静态变量

//鼠标钩子回调
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    Q_UNUSED(nCode);
    Q_UNUSED(lParam);
    switch(wParam)
    {
        case WM_MOUSEMOVE:
        Grabber::sigMouseMoveEmit();//发送对应的信号
        break;
        case WM_RBUTTONDOWN:
        Grabber::sigMouseDownEmit();
        if(Grabber::mouseBlocking)//若当前处于拦截状态则返回true
            return true;
        break;
        case WM_RBUTTONUP:
        Grabber::sigMouseUpEmit();
        if(Grabber::mouseBlocking)
            return true;
        break;
    }
    return false;
}

Grabber::Grabber(QObject *parent) : QObject(parent)
{
    pGrabberForSig=this;//全局指针指向自身
    mouseHook=SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);//注册鼠标钩子
    if(mouseHook==NULL) qDebug("hook null");
    floatDialog=new FloatDialog();//创建选框窗口
    connect(this,SIGNAL(sigMouseMove()),this,SLOT(slotMouseMove()));//进行函数和槽的连接
    connect(this,SIGNAL(sigMouseDown()),this,SLOT(slotMouseDown()));
    connect(this,SIGNAL(sigMouseUp()),this,SLOT(slotMouseUp()));
}

Grabber::~Grabber()
{
    UnhookWindowsHookEx(mouseHook);//卸载钩子
    delete floatDialog;//销毁选框
    pGrabberForSig=NULL;
}

//三个静态函数用于发出信号
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

//三个函数用于设置选框样式
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

//进行截图，结果存入pixmap成员
void Grabber::screenshot()
{
    QRect *rect=pointToRect(dragInfo.start,dragInfo.end);
    pixmap=QGuiApplication::primaryScreen()->grabWindow(0,rect->x(),rect->y(),rect->width(),rect->height());
    delete rect;
}

//两个点转换为矩形
QRect* Grabber::pointToRect(QPoint &p1, QPoint &p2)
{
    int x1=p1.x(),y1=p1.y();
    int x2=p2.x(),y2=p2.y();
    if(x1>x2) qSwap(x1,x2);
    if(y1>y2) qSwap(y1,y2);
    QRect *rect=new QRect(x1,y1,x2-x1,y2-y1);
    return rect;
}

//检查组合键是否正确按下
bool Grabber::combineKeyCorrect()
{
    bool res=true;
    if(((GetKeyState(VK_CONTROL)&0x8000)==0x8000)!=combineKey.ctrl) res=false;
    if(((GetKeyState(VK_SHIFT)&0x8000)==0x8000)!=combineKey.shift) res=false;
    if(((GetKeyState(VK_MENU)&0x8000)==0x8000)!=combineKey.alt) res=false;
    return res;
}

//鼠标移动槽函数
void Grabber::slotMouseMove()
{
    if(!dragInfo.isPressing) return;//若未有效按下则直接退出
    if(!combineKeyCorrect())
    {
        if(dragInfo.isDragging)//表示正在有效拖拽但组合键变为无效，取消一次拖拽
        {
            dragInfo.isDragging=false;//取消拖拽标识
            dragInfo.end=QCursor::pos();//记录结束拖拽位置
            floatDialog->hide();//隐藏选框
        }
        return;
    }
    if(!dragInfo.isDragging)//正在有效拖拽但标识为false，表示刚开始一次拖拽
    {
        dragInfo.isDragging=true;//标识置true
        dragInfo.start=QCursor::pos();//记录开始拖拽位置
        floatDialog->changeShape(dragInfo.start.x(),dragInfo.start.y(),1,1);//移动选框窗口到鼠标位置
        floatDialog->show();//显示选框
    }
    //正在拖拽
    QPoint nowPos=QCursor::pos();
    QRect *rect=pointToRect(dragInfo.start,nowPos);
    floatDialog->changeShape(rect->x(),rect->y(),rect->width(),rect->height());//更新选框形状和位置
    delete rect;
}

//鼠标按下槽函数
void Grabber::slotMouseDown()
{
    if(combineKeyCorrect())//组合键正确则算作有效按下
        dragInfo.isPressing=true;

    if(combineKeyCorrect()&&blockMouseEvent)//判定是否拦截本次鼠标事件
        mouseBlocking=true;
    else
        mouseBlocking=false;
}

void Grabber::slotMouseUp()
{
    if(dragInfo.isDragging)//若正在有效拖拽，则拖拽正常结束
    {
        dragInfo.end=QCursor::pos();//记录结束位置
        floatDialog->hide();//隐藏选框
        screenshot();//截图
        emit sigGrabFinished(pixmap);//发出消息，传递截图
    }
    dragInfo.isPressing=false;//按下和拖拽标识置false
    dragInfo.isDragging=false;

    if(combineKeyCorrect()&&mouseBlocking)//判定是否拦截本次鼠标事件
        mouseBlocking=true;
    else
        mouseBlocking=false;
}
