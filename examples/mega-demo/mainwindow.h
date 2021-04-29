#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libdpp/DFilesystem.h"
#include "libdpp/DPathWatcher.h"
#include "libdpp/DLog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void PrintLog(std::string LogStr);

    signals:
        //void AddLog(std::string LogStr);

    private slots:
        void on_ButtonPathWatcher_clicked();
        void on_ButtonCheck_clicked();
        void on_ButtonStart_clicked();
        void Check(void);
        void on_ButtonStop_clicked();
        void on_ButtonWatcherAlive_clicked();
        void on_ButtonWatcherBrowseFile_clicked();
        void on_ButtonWatcherBrowseDir_clicked();
        void on_ButtonQMLShow_clicked();

        void on_ButtonQMLBrowseFile_clicked();

        void on_ButtonUdpServer_clicked();

        void on_ButtonDPathBrowseDir_clicked();

        void on_ButtonDPathSearch_clicked();

    private:
        Ui::MainWindow *ui;
        DTools::DPathWatcher *Watcher;
        DTools::DLog *Log;
        QTimer *timer;

        static void ChangeCallback(void *ClassObj, DTools::DPathWatcher::DChangeStatus Status, DTools::fs::path File, std::string Msg);
        void ChangeCallbackReceiver(DTools::DPathWatcher::DChangeStatus Status, DTools::fs::path File, std::string Msg);
};
#endif // MAINWINDOW_H
