/*选框窗口*/
#ifndef FLOATDIALOG_H
#define FLOATDIALOG_H

#include <QDialog>

namespace Ui {
class FloatDialog;
}

class FloatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FloatDialog(QWidget *parent = nullptr);
    ~FloatDialog();
    void changeShape(int x,int y,int w,int h);
    void paintEvent(QPaintEvent*);
    QColor lineColor;//选框线颜色
    Qt::PenStyle lineSytle;//选框线样式
    int lineWidth;//选框线粗细

private:
    Ui::FloatDialog *ui;
};

#endif // FLOATDIALOG_H
