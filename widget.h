#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();



private:
    Ui::Widget *ui;
    void setShortcutKeys();

    //初始化界面
    void initUi();


    //将表达式转化为后缀表达式
    QString inToPost(QString infix) throw(const char*);
    //计算后缀表达式的结果
    double compute(QString s) throw(const char*);




    QString error;



    bool calculate(double operand, QString pendingOperator);

    //终止运算，清除数据，报错
    void abortOperation();

    //连接信号和槽
    void connectSlots();

    //储存运算符
    QString pendingOperator;

    //储存运算结果
    double result;

    //标记是否等待一个操作数
    bool waitForOperand;

private slots:
    void on_clearBtn_clicked();
    void on_clearAllBtn_clicked();
    void on_p_equal_clicked();
    void digitClicked();
    void on_signBtn_clicked();
    void operatorClicked();
    void on_p_point_clicked();


};
#endif // WIDGET_H
