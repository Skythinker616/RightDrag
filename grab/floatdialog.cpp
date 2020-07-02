#include "floatdialog.h"
#include "ui_floatdialog.h"
#include "qpainter.h"

FloatDialog::FloatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloatDialog)
{
    ui->setupUi(this);
    setWindowFlags( Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint|Qt::Tool);//显示最前&无边框&无任务栏图标
    setAttribute(Qt::WA_TranslucentBackground, true);//透明
}

FloatDialog::~FloatDialog()
{
    delete ui;
}

//更改选框大小和位置
void FloatDialog::changeShape(int x, int y, int w, int h)
{
    move(x,y);
    setFixedSize(w,h);
    update();//触发paintEvent
}

void FloatDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(lineColor,lineWidth,lineSytle));//设置笔触样式
    painter.drawRect(this->rect());//画选框占满整个窗口
}
