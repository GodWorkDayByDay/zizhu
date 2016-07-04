/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *exitButton;
    QPushButton *pushButton_btn_6;
    QPushButton *pushButton_btn_4;
    QPushButton *pushButton_btn_8;
    QPushButton *pushButton_btn_1;
    QPushButton *pushButton_btn_5;
    QPushButton *pushButton_btn_2;
    QPushButton *pushButton_btn_3;
    QPushButton *pushButton_btn_7;
    QLabel *titlelabel;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(1024, 768);
        exitButton = new QPushButton(Widget);
        exitButton->setObjectName(QStringLiteral("exitButton"));
        exitButton->setGeometry(QRect(910, 690, 111, 71));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(22);
        exitButton->setFont(font);
        pushButton_btn_6 = new QPushButton(Widget);
        pushButton_btn_6->setObjectName(QStringLiteral("pushButton_btn_6"));
        pushButton_btn_6->setGeometry(QRect(520, 320, 200, 51));
        pushButton_btn_6->setFont(font);
        pushButton_btn_4 = new QPushButton(Widget);
        pushButton_btn_4->setObjectName(QStringLiteral("pushButton_btn_4"));
        pushButton_btn_4->setGeometry(QRect(520, 230, 200, 51));
        pushButton_btn_4->setFont(font);
        pushButton_btn_8 = new QPushButton(Widget);
        pushButton_btn_8->setObjectName(QStringLiteral("pushButton_btn_8"));
        pushButton_btn_8->setGeometry(QRect(520, 400, 200, 51));
        pushButton_btn_8->setFont(font);
        pushButton_btn_1 = new QPushButton(Widget);
        pushButton_btn_1->setObjectName(QStringLiteral("pushButton_btn_1"));
        pushButton_btn_1->setGeometry(QRect(290, 150, 200, 51));
        pushButton_btn_1->setFont(font);
        pushButton_btn_5 = new QPushButton(Widget);
        pushButton_btn_5->setObjectName(QStringLiteral("pushButton_btn_5"));
        pushButton_btn_5->setGeometry(QRect(290, 320, 200, 51));
        pushButton_btn_5->setFont(font);
        pushButton_btn_2 = new QPushButton(Widget);
        pushButton_btn_2->setObjectName(QStringLiteral("pushButton_btn_2"));
        pushButton_btn_2->setGeometry(QRect(520, 150, 200, 51));
        pushButton_btn_2->setFont(font);
        pushButton_btn_3 = new QPushButton(Widget);
        pushButton_btn_3->setObjectName(QStringLiteral("pushButton_btn_3"));
        pushButton_btn_3->setGeometry(QRect(290, 230, 200, 51));
        pushButton_btn_3->setFont(font);
        pushButton_btn_7 = new QPushButton(Widget);
        pushButton_btn_7->setObjectName(QStringLiteral("pushButton_btn_7"));
        pushButton_btn_7->setGeometry(QRect(290, 400, 200, 51));
        pushButton_btn_7->setFont(font);
        titlelabel = new QLabel(Widget);
        titlelabel->setObjectName(QStringLiteral("titlelabel"));
        titlelabel->setGeometry(QRect(300, 50, 411, 61));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font1.setPointSize(48);
        titlelabel->setFont(font1);
        titlelabel->setAlignment(Qt::AlignCenter);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", 0));
        exitButton->setText(QApplication::translate("Widget", "\351\200\200\345\207\272", 0));
        pushButton_btn_6->setText(QString());
        pushButton_btn_4->setText(QString());
        pushButton_btn_8->setText(QString());
        pushButton_btn_1->setText(QString());
        pushButton_btn_5->setText(QString());
        pushButton_btn_2->setText(QString());
        pushButton_btn_3->setText(QString());
        pushButton_btn_7->setText(QString());
        titlelabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
