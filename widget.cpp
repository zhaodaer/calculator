#include "widget.h"
#include "ui_widget.h"
#include<QMessageBox>
#include <map>
#include <stack>
#include <cmath>



Widget::Widget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Widget)
{
    initUi();

    //设置程序处于等待操作数输入的状态
    waitForOperand = true;

    connectSlots();
    setShortcutKeys();
}


Widget::~Widget()
{
    delete ui;
}




//初始化界面
void Widget::initUi()
{
    ui->setupUi(this);
    ui->lineEdit->setText("请输入");
    //禁用标题栏的最大化按钮
    //setWindowFlags(windowFlags() & ~(Qt::WindowMaximizeButtonHint));
}


//将中缀表达式改写成后缀表达式
QString Widget::inToPost(QString infix) throw(const char*)
{
    std::stack<char> stack;
    char current = 0;//读入的字符
    QString postfix;//写入后缀表达式的字符串

    std::map<char,int> priority;//运算符号优先级表
    priority['+'] = 0;
    priority['-'] = 0;
    priority['*'] = 1;
    priority['/'] = 1;
    priority['^'] = 2;

    for(int i = 0; i < infix.length() ;++i)//逐个读取中缀表达式字符串中的字符
    {
        current =infix[i].toLatin1();
        if(isdigit(current))//如果是数字直接输出
        {
            postfix.push_back(current);
            continue;
        }
        switch(current)
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
            //postfix.push_back('#');
            if(i > 0)    //如果运算符的前一项不是右括号则说明前一个数字输入完毕，用#标识前面几个字符组成一个数字
             {
                if (infix[i-1] != ')')
                    postfix.push_back('#');
                /*if(infix[i-1].isDigit())
                    postfix.push_back('#');
                else
                    throw "expression is illegality";*/
            }
            else
                postfix.push_back('#');
            if(!stack.empty())//比较目前符号与栈顶符号优先级，低于则出栈，并输出字符串
            {
                char tempTop=stack.top();
                while(tempTop != '(' && priority[current]<=priority[tempTop])
                {
                    stack.pop();
                    postfix.push_back(tempTop);
                    if(stack.empty())
                        break;
                    tempTop=stack.top();
                }
            }
            stack.push(current);//符号全部出栈或者遇到了'('或者大于栈顶符号的优先级，将新符号压入栈中
            break;
        case '.':
            postfix.push_back(current);
            break;
        case '%':
            postfix.push_back(current);
            break;
        case '(':
            stack.push(current);//左括号直接入栈
            break;
        case ')':
            postfix.push_back('#');//右括号说明前方数字输入完成，标识一下
            char tempTop;
            tempTop=stack.top();
            while(tempTop !='(')//直到栈顶元素是左括号才停止循环
            {
                stack.pop();
                postfix.push_back(tempTop);
                tempTop=stack.top();
            }
            stack.pop();
            break;
        default:
            throw "expression has illegality character";
            break;
        }
    }
    if(infix[infix.size()-1] != ')')
    {
        if(infix[infix.size()-1].isDigit())
            postfix.push_back('#');
        else
            throw "expression is illegality";
    }
    while(!stack.empty())
    {
        char tempOut=stack.top();
        stack.pop();
        postfix.push_back(tempOut);
    }
    return postfix;
}

//后缀表达式的计算
double Widget::compute(QString s) throw(const char*)
{
    std::stack<double> stack;
    QString str;
    double curr;

    double  temNum1;
    double  temNum2;
    for(auto i = s.begin();i != s.end(); i++)
    {
        if((*i).isDigit())
        {
            str.push_back(*i);
            continue;
        }
        switch((*i).toLatin1())
        {
        case '+':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2 + temNum1);
            break;
        case '-':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2 - temNum1);
            break;
        case '*':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2 * temNum1);
            break;
        case '/':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(temNum2 / temNum1);
            break;
        case '^':
            temNum1=stack.top();
            stack.pop();
            temNum2=stack.top();
            stack.pop();
            stack.push(pow(temNum2, temNum1));
            break;
        case '.':
            str.push_back(*i);
            break;
        case '#':
            curr =str.toDouble();//字符串转换为浮点型
            str.clear();
            stack.push(curr);
            break;
        case '%':
            curr = stack.top();
            stack.pop();
            curr*=0.01;
            stack.push(curr);
            break;
        default:
            throw "expression has illegality character";
            break;
        }
    }
    curr=stack.top();
    return curr;
}

/*
//
bool Widget::calculate(double operand, QString pendingOperator)
{
    if(pendingOperator == "+")
    {
        result += operand;
    }
    else if(pendingOperator == "-")
    {
        result -= operand;
    }
    else if(pendingOperator == "*")
    {
        result *= operand;
    }
    else if(pendingOperator == "/")
    {
        if(operand == 0.0)
            return false;
        result /= operand;
    }
    return true;
}
*/

//处理运算错误
void Widget::abortOperation()
{
    ui->lineEdit->setText("0");
    waitForOperand = true;
    QMessageBox::warning(this, "运算错误", error);
}


void Widget::connectSlots()
{
    QPushButton *digitBtns[10] = {
        ui->p0,      ui->p1,          ui->p2,          ui->p3,
        ui->p4,      ui->p5,          ui->p6,          ui->p7,
        ui->p8,      ui->p9
    };
    for (auto btn : digitBtns)
        connect(btn, &QPushButton::clicked, this, &Widget::digitClicked);
    QPushButton *operatorBtns[7] = {
        ui->p_plus,         ui->p_minus,     ui->p_divide,  ui->p_multiply,
        ui->p_leftBracket,  ui->p_rightBracket,    ui->p_pow,
    };
    for (auto btn : operatorBtns)
        connect(btn, &QPushButton::clicked, this, &Widget::operatorClicked);

}


void Widget::operatorClicked()
{

    QPushButton *clickedBtn = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedBtn->text();
    ui->lineEdit->setText(ui->lineEdit->text() + clickedOperator);


}




// 按钮点击事件更新显示在界面上的文本
void Widget::digitClicked()
{
    QPushButton *digitBtn = static_cast<QPushButton*>(sender());
    QString value = digitBtn->text();
    if(ui->lineEdit->text() == "0" && value == "0")
        return;
    if(waitForOperand)
    {
        ui->lineEdit->setText(value);
        waitForOperand = false;
    }
    else
    {
        ui->lineEdit->setText(ui->lineEdit->text() + value);
    }
}




//清除按钮
void Widget::on_clearBtn_clicked()
{
    /*
    //将当前显示的数归零
    ui->lineEdit->setText("0");
    waitForOperand = true;
    */
    //去掉末尾字符
    QString str = ui->lineEdit->text();
    if (str != "0")
    {
        ui->lineEdit->setText(str.left(str.count() - 1));
    }
}






//清除所有按钮
void Widget::on_clearAllBtn_clicked()
{
    //将当前显示的数据归零，并将之前保存的数据运算清除
    ui->lineEdit->setText("0");
    waitForOperand = true;
    result = 0.0;
    pendingOperator.clear();
}


//等号处理按钮
void Widget::on_p_equal_clicked()
{
    double result = 0.0;
        try
        {
            //将文本框显示的数据用后缀表达式计算，并存入result
            result = compute(inToPost(ui->lineEdit->text()));
        }
        //如果有错误则抓取
        catch(const char *er)
        {
            error = er;
            abortOperation();
            return;
        }
        //输出结果从 1+2*6 ->  1+2*6=13
        ui->lineEdit->setText(ui->lineEdit->text() + '=' + QString::number(result));
        waitForOperand = true;

}

//符号切换按钮
void Widget::on_signBtn_clicked()
{
    QString text = ui->lineEdit->text();
    double value = text.toDouble();
    if(value > 0)
    {
        text.prepend('-');
    }
    else if(value < 0)
    {
        text.remove(0, 1);
    }
    ui->lineEdit->setText(text);
}


//小数点按钮
void Widget::on_p_point_clicked()
{
    if (waitForOperand)
            ui->lineEdit->setText("0");
        ui->lineEdit->setText(ui->lineEdit->text() + ".");
        waitForOperand = false;

}




//键盘点击函数
void Widget::setShortcutKeys()
{

    Qt::Key key[21] = {
        Qt::Key_0,          Qt::Key_1,          Qt::Key_2,          Qt::Key_3,
        Qt::Key_4,          Qt::Key_5,          Qt::Key_6,          Qt::Key_7,
        Qt::Key_8,          Qt::Key_9,
        Qt::Key_Plus,       Qt::Key_Minus,      Qt::Key_Asterisk,   Qt::Key_Slash,
        Qt::Key_Enter,      Qt::Key_Period,     Qt::Key_Backspace,  Qt::Key_M,
        Qt::Key_ParenLeft,  Qt::Key_ParenRight, Qt::Key_AsciiCircum
    };


    QPushButton *btn[21] = {
           ui->p0,      ui->p1,          ui->p2,          ui->p3,
           ui->p4,      ui->p5,          ui->p6,          ui->p7,
           ui->p8,      ui->p9,
           ui->p_plus,         ui->p_minus,     ui->p_divide,  ui->p_multiply,
           ui->p_equal,       ui->p_point,           ui->clearBtn,           ui->signBtn,
           ui->p_leftBracket, ui->p_rightBracket,    ui->p_pow
       };
       for (int i = 0; i < 21; i++)
           btn[i]->setShortcut(QKeySequence(key[i]));
       ui->clearAllBtn->setShortcut(QKeySequence("Ctrl+Backspace"));
   }


