/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *TextEditLog;
    QFrame *frame_2;
    QCheckBox *CheckBoxQMLSplashScreen;
    QFrame *FrameSpinner;
    QLineEdit *EditQML;
    QPushButton *ButtonQMLShow;
    QLabel *label_3;
    QPushButton *ButtonQMLBrowseFile;
    QLabel *label_2;
    QPushButton *ButtonUdpServer;
    QTabWidget *tabWidget;
    QWidget *widget;
    QFrame *frame;
    QLineEdit *EditWatcher;
    QPushButton *ButtonWatcherAlive;
    QPushButton *ButtonCheck;
    QPushButton *ButtonStop;
    QPushButton *ButtonPathWatcher;
    QPushButton *ButtonStart;
    QPushButton *ButtonWatcherBrowseFile;
    QPushButton *ButtonWatcherBrowseDir;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label;
    QFrame *frame_4;
    QTextEdit *TextEditExtList;
    QTextEdit *TextEditNameList;
    QPushButton *ButtonDPathBrowseDir;
    QLineEdit *EditDPath;
    QPushButton *ButtonDPathSearch;
    QCheckBox *CheckBoxDPathRecoursive;
    QCheckBox *CheckBoxDPathCase;
    QRadioButton *RadioButtonDPathFiles;
    QRadioButton *RadioButtonDPathDirs;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_7;
    QWidget *widget_2;
    QFrame *frame_3;
    QLineEdit *EditDShell;
    QPushButton *ButtonDShellExecute;
    QPushButton *ButtonDShellBrowseFile;
    QTextEdit *TextEditDShellArgs;
    QLabel *label_12;
    QLabel *label_13;
    QCheckBox *CheckBoxExecuteDetached;
    QSpinBox *SpinBoxWaitMillis;
    QLabel *label_14;
    QLabel *LabelDShellExecute;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1448, 654);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        TextEditLog = new QTextEdit(centralwidget);
        TextEditLog->setObjectName(QString::fromUtf8("TextEditLog"));
        TextEditLog->setGeometry(QRect(10, 450, 761, 131));
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(810, 220, 561, 111));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Sunken);
        frame_2->setLineWidth(2);
        CheckBoxQMLSplashScreen = new QCheckBox(frame_2);
        CheckBoxQMLSplashScreen->setObjectName(QString::fromUtf8("CheckBoxQMLSplashScreen"));
        CheckBoxQMLSplashScreen->setGeometry(QRect(10, 80, 121, 20));
        FrameSpinner = new QFrame(frame_2);
        FrameSpinner->setObjectName(QString::fromUtf8("FrameSpinner"));
        FrameSpinner->setGeometry(QRect(440, 10, 111, 91));
        FrameSpinner->setFrameShape(QFrame::Box);
        FrameSpinner->setFrameShadow(QFrame::Raised);
        EditQML = new QLineEdit(frame_2);
        EditQML->setObjectName(QString::fromUtf8("EditQML"));
        EditQML->setGeometry(QRect(10, 50, 381, 22));
        ButtonQMLShow = new QPushButton(frame_2);
        ButtonQMLShow->setObjectName(QString::fromUtf8("ButtonQMLShow"));
        ButtonQMLShow->setGeometry(QRect(340, 10, 93, 28));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 30, 111, 16));
        label_3->setToolTipDuration(4);
        ButtonQMLBrowseFile = new QPushButton(frame_2);
        ButtonQMLBrowseFile->setObjectName(QString::fromUtf8("ButtonQMLBrowseFile"));
        ButtonQMLBrowseFile->setGeometry(QRect(400, 50, 31, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(820, 210, 141, 16));
        QFont font;
        font.setPointSize(8);
        font.setBold(true);
        font.setWeight(75);
        font.setKerning(true);
        label_2->setFont(font);
        label_2->setAutoFillBackground(true);
        ButtonUdpServer = new QPushButton(centralwidget);
        ButtonUdpServer->setObjectName(QString::fromUtf8("ButtonUdpServer"));
        ButtonUdpServer->setGeometry(QRect(810, 120, 93, 28));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 40, 761, 391));
        widget = new QWidget();
        widget->setObjectName(QString::fromUtf8("widget"));
        frame = new QFrame(widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 30, 531, 101));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLineWidth(2);
        frame->setMidLineWidth(0);
        EditWatcher = new QLineEdit(frame);
        EditWatcher->setObjectName(QString::fromUtf8("EditWatcher"));
        EditWatcher->setGeometry(QRect(10, 40, 411, 22));
        ButtonWatcherAlive = new QPushButton(frame);
        ButtonWatcherAlive->setObjectName(QString::fromUtf8("ButtonWatcherAlive"));
        ButtonWatcherAlive->setGeometry(QRect(410, 70, 93, 28));
        ButtonCheck = new QPushButton(frame);
        ButtonCheck->setObjectName(QString::fromUtf8("ButtonCheck"));
        ButtonCheck->setGeometry(QRect(110, 70, 93, 28));
        ButtonStop = new QPushButton(frame);
        ButtonStop->setObjectName(QString::fromUtf8("ButtonStop"));
        ButtonStop->setGeometry(QRect(310, 70, 93, 28));
        ButtonPathWatcher = new QPushButton(frame);
        ButtonPathWatcher->setObjectName(QString::fromUtf8("ButtonPathWatcher"));
        ButtonPathWatcher->setGeometry(QRect(10, 70, 93, 28));
        ButtonStart = new QPushButton(frame);
        ButtonStart->setObjectName(QString::fromUtf8("ButtonStart"));
        ButtonStart->setGeometry(QRect(210, 70, 93, 28));
        ButtonWatcherBrowseFile = new QPushButton(frame);
        ButtonWatcherBrowseFile->setObjectName(QString::fromUtf8("ButtonWatcherBrowseFile"));
        ButtonWatcherBrowseFile->setGeometry(QRect(420, 10, 31, 21));
        ButtonWatcherBrowseDir = new QPushButton(frame);
        ButtonWatcherBrowseDir->setObjectName(QString::fromUtf8("ButtonWatcherBrowseDir"));
        ButtonWatcherBrowseDir->setGeometry(QRect(420, 40, 31, 21));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 20, 131, 16));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(460, 10, 71, 16));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(460, 40, 71, 16));
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 101, 16));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setAutoFillBackground(true);
        frame_4 = new QFrame(widget);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setGeometry(QRect(10, 160, 541, 181));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        TextEditExtList = new QTextEdit(frame_4);
        TextEditExtList->setObjectName(QString::fromUtf8("TextEditExtList"));
        TextEditExtList->setGeometry(QRect(160, 80, 104, 87));
        TextEditNameList = new QTextEdit(frame_4);
        TextEditNameList->setObjectName(QString::fromUtf8("TextEditNameList"));
        TextEditNameList->setGeometry(QRect(10, 80, 131, 87));
        ButtonDPathBrowseDir = new QPushButton(frame_4);
        ButtonDPathBrowseDir->setObjectName(QString::fromUtf8("ButtonDPathBrowseDir"));
        ButtonDPathBrowseDir->setGeometry(QRect(430, 30, 31, 21));
        EditDPath = new QLineEdit(frame_4);
        EditDPath->setObjectName(QString::fromUtf8("EditDPath"));
        EditDPath->setGeometry(QRect(10, 30, 411, 22));
        ButtonDPathSearch = new QPushButton(frame_4);
        ButtonDPathSearch->setObjectName(QString::fromUtf8("ButtonDPathSearch"));
        ButtonDPathSearch->setGeometry(QRect(430, 70, 93, 28));
        CheckBoxDPathRecoursive = new QCheckBox(frame_4);
        CheckBoxDPathRecoursive->setObjectName(QString::fromUtf8("CheckBoxDPathRecoursive"));
        CheckBoxDPathRecoursive->setGeometry(QRect(280, 80, 91, 20));
        CheckBoxDPathCase = new QCheckBox(frame_4);
        CheckBoxDPathCase->setObjectName(QString::fromUtf8("CheckBoxDPathCase"));
        CheckBoxDPathCase->setGeometry(QRect(280, 100, 121, 20));
        RadioButtonDPathFiles = new QRadioButton(frame_4);
        RadioButtonDPathFiles->setObjectName(QString::fromUtf8("RadioButtonDPathFiles"));
        RadioButtonDPathFiles->setGeometry(QRect(280, 130, 95, 20));
        RadioButtonDPathFiles->setChecked(true);
        RadioButtonDPathDirs = new QRadioButton(frame_4);
        RadioButtonDPathDirs->setObjectName(QString::fromUtf8("RadioButtonDPathDirs"));
        RadioButtonDPathDirs->setGeometry(QRect(280, 150, 95, 20));
        label_8 = new QLabel(frame_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 10, 131, 16));
        label_9 = new QLabel(frame_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 60, 91, 16));
        label_10 = new QLabel(frame_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(160, 60, 81, 16));
        label_11 = new QLabel(frame_4);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(470, 30, 71, 16));
        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 150, 141, 16));
        label_7->setFont(font1);
        label_7->setAutoFillBackground(true);
        tabWidget->addTab(widget, QString());
        widget_2 = new QWidget();
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        frame_3 = new QFrame(widget_2);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(10, 20, 331, 171));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        EditDShell = new QLineEdit(frame_3);
        EditDShell->setObjectName(QString::fromUtf8("EditDShell"));
        EditDShell->setGeometry(QRect(10, 40, 271, 24));
        ButtonDShellExecute = new QPushButton(frame_3);
        ButtonDShellExecute->setObjectName(QString::fromUtf8("ButtonDShellExecute"));
        ButtonDShellExecute->setGeometry(QRect(240, 120, 81, 41));
        ButtonDShellBrowseFile = new QPushButton(frame_3);
        ButtonDShellBrowseFile->setObjectName(QString::fromUtf8("ButtonDShellBrowseFile"));
        ButtonDShellBrowseFile->setGeometry(QRect(290, 40, 31, 21));
        TextEditDShellArgs = new QTextEdit(frame_3);
        TextEditDShellArgs->setObjectName(QString::fromUtf8("TextEditDShellArgs"));
        TextEditDShellArgs->setGeometry(QRect(10, 90, 131, 71));
        label_12 = new QLabel(frame_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 20, 58, 16));
        label_13 = new QLabel(frame_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(10, 70, 58, 16));
        CheckBoxExecuteDetached = new QCheckBox(frame_3);
        CheckBoxExecuteDetached->setObjectName(QString::fromUtf8("CheckBoxExecuteDetached"));
        CheckBoxExecuteDetached->setGeometry(QRect(150, 90, 161, 22));
        SpinBoxWaitMillis = new QSpinBox(frame_3);
        SpinBoxWaitMillis->setObjectName(QString::fromUtf8("SpinBoxWaitMillis"));
        SpinBoxWaitMillis->setGeometry(QRect(150, 140, 81, 25));
        SpinBoxWaitMillis->setMaximum(10000);
        SpinBoxWaitMillis->setValue(5000);
        label_14 = new QLabel(frame_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(150, 120, 71, 16));
        LabelDShellExecute = new QLabel(widget_2);
        LabelDShellExecute->setObjectName(QString::fromUtf8("LabelDShellExecute"));
        LabelDShellExecute->setGeometry(QRect(20, 10, 111, 16));
        LabelDShellExecute->setFont(font1);
        LabelDShellExecute->setAutoFillBackground(true);
        tabWidget->addTab(widget_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1448, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        CheckBoxQMLSplashScreen->setText(QCoreApplication::translate("MainWindow", "Show as Splash", nullptr));
        EditQML->setInputMask(QString());
        EditQML->setText(QCoreApplication::translate("MainWindow", "spin.qml", nullptr));
        ButtonQMLShow->setText(QCoreApplication::translate("MainWindow", "Show", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "QtQuick widget file", nullptr));
#if QT_CONFIG(tooltip)
        ButtonQMLBrowseFile->setToolTip(QCoreApplication::translate("MainWindow", "Select File", nullptr));
#endif // QT_CONFIG(tooltip)
        ButtonQMLBrowseFile->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "QQuickWidget Tester", nullptr));
        ButtonUdpServer->setText(QCoreApplication::translate("MainWindow", "Udp Server", nullptr));
        ButtonWatcherAlive->setText(QCoreApplication::translate("MainWindow", "IsAlive", nullptr));
        ButtonCheck->setText(QCoreApplication::translate("MainWindow", "Check", nullptr));
        ButtonStop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        ButtonPathWatcher->setText(QCoreApplication::translate("MainWindow", "Create", nullptr));
        ButtonStart->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
#if QT_CONFIG(tooltip)
        ButtonWatcherBrowseFile->setToolTip(QCoreApplication::translate("MainWindow", "Select File", nullptr));
#endif // QT_CONFIG(tooltip)
        ButtonWatcherBrowseFile->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
#if QT_CONFIG(tooltip)
        ButtonWatcherBrowseDir->setToolTip(QCoreApplication::translate("MainWindow", "Select Dir", nullptr));
#endif // QT_CONFIG(tooltip)
        ButtonWatcherBrowseDir->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "File or folder to watch", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Browse File", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Browse Dir", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "DPathWatcher", nullptr));
#if QT_CONFIG(tooltip)
        ButtonDPathBrowseDir->setToolTip(QCoreApplication::translate("MainWindow", "Select Dir", nullptr));
#endif // QT_CONFIG(tooltip)
        ButtonDPathBrowseDir->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        ButtonDPathSearch->setText(QCoreApplication::translate("MainWindow", "Count", nullptr));
        CheckBoxDPathRecoursive->setText(QCoreApplication::translate("MainWindow", "Recoursive", nullptr));
        CheckBoxDPathCase->setText(QCoreApplication::translate("MainWindow", "Case Sensistive", nullptr));
        RadioButtonDPathFiles->setText(QCoreApplication::translate("MainWindow", "Find Files", nullptr));
        RadioButtonDPathDirs->setText(QCoreApplication::translate("MainWindow", "Find Dirs", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Source Folder", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Name patterns", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Ext patterns", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Browse Dir", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Find files and folders", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(widget), QCoreApplication::translate("MainWindow", "DPath", nullptr));
        ButtonDShellExecute->setText(QCoreApplication::translate("MainWindow", "Execute", nullptr));
#if QT_CONFIG(tooltip)
        ButtonDShellBrowseFile->setToolTip(QCoreApplication::translate("MainWindow", "Select File", nullptr));
#endif // QT_CONFIG(tooltip)
        ButtonDShellBrowseFile->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Filename", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Args List", nullptr));
        CheckBoxExecuteDetached->setText(QCoreApplication::translate("MainWindow", "Detached (Wait unused)", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Wait Millis", nullptr));
        LabelDShellExecute->setText(QCoreApplication::translate("MainWindow", "Execute process", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(widget_2), QCoreApplication::translate("MainWindow", "DShell", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
