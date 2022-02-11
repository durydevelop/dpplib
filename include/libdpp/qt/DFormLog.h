#ifndef DFORMLOG_H
#define DFORMLOG_H

#ifdef QT_GUI_LIB

#include <QDialog>
#include <QPlainTextEdit>
#include "libdpp/DLog.h"
#include "libdpp/DPreferences.h"

namespace Ui {
    class DFormLog;
}

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

    signals:
        void SignalAdd(QString Msg, QString OutputLevel, QString Header);

    private slots:
        void on_ButtonReload_clicked();
        void on_ButtonOpenFolder_clicked();
        void on_ButtonSendLogs_clicked();

    public slots:
        void Add(QString Msg, QString OutputLevel, QString Header);

    private:
        void DLogCallback(std::string Msg, std::string OutputLevel, std::string Header);

        Ui::DFormLog *ui;
        QString Separator;
};

#endif
#endif // DFORMLOG_H
