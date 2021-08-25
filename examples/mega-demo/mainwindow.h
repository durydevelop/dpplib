#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "libdpp/network/DRESTClient.h"
#include "libdpp/DFilesystem.h"
#include "libdpp/DPathWatcher.h"
#include "libdpp/DLog.h"
#include <QMainWindow>

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

        void on_pushButton_2_clicked();

        void on_ButtonDShellExecute_clicked();

        void on_ButtonRestClientCreate_clicked();

        void on_ButtonRestClientConnect_clicked();

        void on_ButtonRestClientPost_clicked();


        void on_ButtonRestClientDIsconnect_clicked();

        void on_ButtonRestClientOneShot_clicked();

        void on_ButtonRestClientDelete_clicked();

    private:
        Ui::MainWindow *ui;
        DTools::DPathWatcher *Watcher;
        DTools::DLog *Log;
        QTimer *timer;
        std::shared_ptr<DTools::DNetwork::DRESTClient> RESTClient;

        static void ChangeCallback(void *ClassObj, DTools::DPathWatcher::DChangeStatus Status, DTools::fs::path File, std::string Msg);
        void ChangeCallbackReceiver(DTools::DPathWatcher::DChangeStatus Status, DTools::fs::path File, std::string Msg);

        // DRESTClient Callbacks
        void OnLog(std::string LogMsg);
        void OnResponse(DTools::DNetwork::DRESTClient::DHttpResponse HttpResponse);
};
#endif // MAINWINDOW_H
