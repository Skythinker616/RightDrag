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
    QColor lineColor;
    Qt::PenStyle lineSytle;
    int lineWidth;

private:
    Ui::FloatDialog *ui;
};

#endif // FLOATDIALOG_H
