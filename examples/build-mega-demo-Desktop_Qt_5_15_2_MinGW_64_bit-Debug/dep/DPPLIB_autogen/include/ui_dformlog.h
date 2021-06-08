/********************************************************************************
** Form generated from reading UI file 'DFormLog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DFORMLOG_H
#define UI_DFORMLOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DFormLog
{
public:
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *PlainTextEditLog;
    QHBoxLayout *horizontalLayout;
    QPushButton *ButtonOpenFolder;
    QPushButton *ButtonSendLogs;
    QPushButton *pushButton;
    QPushButton *ButtonQuit;

    void setupUi(QDialog *DFormLog)
    {
        if (DFormLog->objectName().isEmpty())
            DFormLog->setObjectName(QString::fromUtf8("DFormLog"));
        DFormLog->resize(609, 336);
        verticalLayout = new QVBoxLayout(DFormLog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        PlainTextEditLog = new QPlainTextEdit(DFormLog);
        PlainTextEditLog->setObjectName(QString::fromUtf8("PlainTextEditLog"));
        PlainTextEditLog->setTextInteractionFlags(Qt::TextEditable|Qt::TextSelectableByMouse);

        verticalLayout->addWidget(PlainTextEditLog);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ButtonOpenFolder = new QPushButton(DFormLog);
        ButtonOpenFolder->setObjectName(QString::fromUtf8("ButtonOpenFolder"));

        horizontalLayout->addWidget(ButtonOpenFolder);

        ButtonSendLogs = new QPushButton(DFormLog);
        ButtonSendLogs->setObjectName(QString::fromUtf8("ButtonSendLogs"));

        horizontalLayout->addWidget(ButtonSendLogs);

        pushButton = new QPushButton(DFormLog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        ButtonQuit = new QPushButton(DFormLog);
        ButtonQuit->setObjectName(QString::fromUtf8("ButtonQuit"));

        horizontalLayout->addWidget(ButtonQuit);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(DFormLog);
        QObject::connect(ButtonQuit, SIGNAL(clicked()), DFormLog, SLOT(accept()));

        QMetaObject::connectSlotsByName(DFormLog);
    } // setupUi

    void retranslateUi(QDialog *DFormLog)
    {
        DFormLog->setWindowTitle(QCoreApplication::translate("DFormLog", "Dialog", nullptr));
        ButtonOpenFolder->setText(QCoreApplication::translate("DFormLog", "Open Log Folder", nullptr));
        ButtonSendLogs->setText(QCoreApplication::translate("DFormLog", "Send Logs", nullptr));
        pushButton->setText(QCoreApplication::translate("DFormLog", "Prova", nullptr));
        ButtonQuit->setText(QCoreApplication::translate("DFormLog", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DFormLog: public Ui_DFormLog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DFORMLOG_H
