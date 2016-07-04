/********************************************************************************
** Form generated from reading UI file 'singlecoldialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SINGLECOLDIALOG_H
#define UI_SINGLECOLDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_singleColDialog
{
public:
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_4;
    QPushButton *pushButton;

    void setupUi(QDialog *singleColDialog)
    {
        if (singleColDialog->objectName().isEmpty())
            singleColDialog->setObjectName(QStringLiteral("singleColDialog"));
        singleColDialog->setEnabled(true);
        singleColDialog->resize(1024, 768);
        pushButton_1 = new QPushButton(singleColDialog);
        pushButton_1->setObjectName(QStringLiteral("pushButton_1"));
        pushButton_1->setGeometry(QRect(240, 5, 461, 61));
        QFont font;
        font.setPointSize(24);
        pushButton_1->setFont(font);
        pushButton_2 = new QPushButton(singleColDialog);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(240, 70, 461, 61));
        pushButton_2->setFont(font);
        pushButton_3 = new QPushButton(singleColDialog);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(243, 142, 461, 61));
        pushButton_3->setFont(font);
        pushButton_5 = new QPushButton(singleColDialog);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setGeometry(QRect(240, 290, 461, 61));
        pushButton_5->setFont(font);
        pushButton_6 = new QPushButton(singleColDialog);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setGeometry(QRect(240, 370, 461, 61));
        pushButton_6->setFont(font);
        pushButton_7 = new QPushButton(singleColDialog);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        pushButton_7->setGeometry(QRect(240, 450, 461, 61));
        pushButton_7->setFont(font);
        pushButton_8 = new QPushButton(singleColDialog);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setGeometry(QRect(240, 530, 461, 61));
        pushButton_8->setFont(font);
        pushButton_9 = new QPushButton(singleColDialog);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setGeometry(QRect(240, 620, 461, 61));
        pushButton_9->setFont(font);
        pushButton_10 = new QPushButton(singleColDialog);
        pushButton_10->setObjectName(QStringLiteral("pushButton_10"));
        pushButton_10->setGeometry(QRect(240, 700, 461, 61));
        pushButton_10->setFont(font);
        pushButton_4 = new QPushButton(singleColDialog);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(240, 215, 461, 61));
        pushButton_4->setFont(font);
        pushButton = new QPushButton(singleColDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(880, 680, 121, 81));
        QFont font1;
        font1.setPointSize(38);
        pushButton->setFont(font1);

        retranslateUi(singleColDialog);

        QMetaObject::connectSlotsByName(singleColDialog);
    } // setupUi

    void retranslateUi(QDialog *singleColDialog)
    {
        singleColDialog->setWindowTitle(QApplication::translate("singleColDialog", "Dialog", 0));
        pushButton_1->setText(QString());
        pushButton_2->setText(QString());
        pushButton_3->setText(QString());
        pushButton_5->setText(QString());
        pushButton_6->setText(QString());
        pushButton_7->setText(QString());
        pushButton_8->setText(QString());
        pushButton_9->setText(QString());
        pushButton_10->setText(QString());
        pushButton_4->setText(QString());
        pushButton->setText(QApplication::translate("singleColDialog", "\350\277\224\345\233\236", 0));
    } // retranslateUi

};

namespace Ui {
    class singleColDialog: public Ui_singleColDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SINGLECOLDIALOG_H
