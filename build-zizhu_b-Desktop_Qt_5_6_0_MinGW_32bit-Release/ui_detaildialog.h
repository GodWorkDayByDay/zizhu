/********************************************************************************
** Form generated from reading UI file 'detaildialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETAILDIALOG_H
#define UI_DETAILDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DetailDialog
{
public:
    QLabel *label_1;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *password_lineEdit;
    QPushButton *pushButton;

    void setupUi(QDialog *DetailDialog)
    {
        if (DetailDialog->objectName().isEmpty())
            DetailDialog->setObjectName(QStringLiteral("DetailDialog"));
        DetailDialog->resize(1024, 768);
        label_1 = new QLabel(DetailDialog);
        label_1->setObjectName(QStringLiteral("label_1"));
        label_1->setGeometry(QRect(0, 0, 1024, 76));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(36);
        label_1->setFont(font);
        label_1->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(DetailDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(0, 76, 1024, 76));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(DetailDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(0, 152, 1024, 76));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(DetailDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(0, 228, 1024, 76));
        label_4->setFont(font);
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(DetailDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(0, 304, 1024, 76));
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);
        label_6 = new QLabel(DetailDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(0, 385, 1024, 76));
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignCenter);
        label_7 = new QLabel(DetailDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(0, 456, 1024, 76));
        label_7->setFont(font);
        label_7->setAlignment(Qt::AlignCenter);
        label_8 = new QLabel(DetailDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(0, 532, 1024, 76));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignCenter);
        label_9 = new QLabel(DetailDialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(0, 608, 1024, 76));
        label_9->setFont(font);
        label_9->setAlignment(Qt::AlignCenter);
        label_10 = new QLabel(DetailDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(0, 684, 1024, 76));
        label_10->setFont(font);
        label_10->setAlignment(Qt::AlignCenter);
        password_lineEdit = new QLineEdit(DetailDialog);
        password_lineEdit->setObjectName(QStringLiteral("password_lineEdit"));
        password_lineEdit->setEnabled(true);
        password_lineEdit->setGeometry(QRect(50, 304, 927, 76));
        password_lineEdit->setFont(font);
        password_lineEdit->setFocusPolicy(Qt::ClickFocus);
        password_lineEdit->setAcceptDrops(false);
        password_lineEdit->setEchoMode(QLineEdit::Password);
        pushButton = new QPushButton(DetailDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(870, 660, 141, 91));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font1.setPointSize(22);
        pushButton->setFont(font1);

        retranslateUi(DetailDialog);

        QMetaObject::connectSlotsByName(DetailDialog);
    } // setupUi

    void retranslateUi(QDialog *DetailDialog)
    {
        DetailDialog->setWindowTitle(QApplication::translate("DetailDialog", "Dialog", 0));
        label_1->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_2->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_3->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_4->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_5->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_6->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_7->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_8->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_9->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        label_10->setText(QApplication::translate("DetailDialog", "TextLabel", 0));
        pushButton->setText(QApplication::translate("DetailDialog", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class DetailDialog: public Ui_DetailDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETAILDIALOG_H
