#ifndef DFORMLOG_H
#define DFORMLOG_H
#ifdef QT_GUI_LIB

#include <QDialog>
#include <QPlainTextEdit>
#include <QStringListModel>
#include <QThread>
#include <libdpp/DLog.h>
#include <libdpp/DPreferences.h>
#include <libdpp/qt/DSpinnerWidget.h>

namespace Ui
{
    class DFormLog;
}

struct DEmitStringList : QObject
{
   Q_SIGNAL void signal(const QStringList &);
   void operator()(const QStringList &data) {
       emit signal(data);
   }
   Q_OBJECT
};

namespace DTools
{
class DFormLog : public QDialog
{
    Q_OBJECT

    public:
        static void Create(void);
        explicit DFormLog(QWidget *parent = nullptr, std::shared_ptr<DTools::DLog> dLog = nullptr);
        ~DFormLog();

        void SetDeleteOnClose(bool Enabled);
        void SaveWindowPosition(DTools::DPreferences &Prefs);
        void RestoreWindowPosition(DTools::DPreferences &Prefs);
        void SetDLog(std::shared_ptr<DTools::DLog> dLog);
        bool LoadFile(void);

        std::shared_ptr<DTools::DLog> DuryLog;

        bool ShowDateTime;
        bool ShowOutputLevel;

        QStringListModel *ModelStringList;
        DEmitStringList EmitStringList;

    signals:
        void SignalAdd(QString Msg, QString OutputLevel = "", QString Header = "");
        void SignalWaitingSpinner(bool Enable);
        void SignalScrollToBottom(void);

    private slots:
        void OnWaitingSpinner(bool Enable);
        void OnScrollToBottom(void);
        void on_ButtonReload_clicked();
        void on_ButtonOpenFolder_clicked();
        void on_ButtonSendLogs_clicked();

    public slots:
        void Add(QString Msg, QString OutputLevel = "", QString Header = "");
        void rowsInserted();

    private:
        void DLogCallback(std::string Msg, std::string OutputLevel, std::string Header);

        Ui::DFormLog *ui;
        QString Separator;
        DSpinnerWidget *WaitingSpinner;
        QThread *Thread;
        QTimer *Timer;
};
}
#endif
#endif // DFORMLOG_H
