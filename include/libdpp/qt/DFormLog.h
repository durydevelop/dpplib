#ifndef DFORMLOG_H
#define DFORMLOG_H

#ifdef QT_GUI_LIB

#include <QDialog>
#include <QPlainTextEdit>
#include "libdpp/DLog.h"
#include "libdpp/DPreferences.h"

class DStreamBuff : public std::basic_streambuf<char> {
    public:
        DStreamBuff(QPlainTextEdit* TextEdit) {
            LogBox=TextEdit;
        }

        ~DStreamBuff() {

        }

    private:

    protected:
        //This is called when a std::endl has been inserted into the stream
        virtual int_type overflow(int_type v) {
            if (v == '\n') { // || v == '\r') {
                LogBox->appendPlainText(QString());
            }
            return v;
        }


        virtual std::streamsize xsputn(const char *p, std::streamsize n) {
            QString str(p);
            if(str.contains("\n")){
                str.remove("\r");
                QStringList strSplitted = str.split("\n");

                LogBox->moveCursor (QTextCursor::End);
                LogBox->insertPlainText (strSplitted.at(0)); //Index 0 is still on the same old line

                for(int i = 1; i < strSplitted.size(); i++){
                    LogBox->appendPlainText(strSplitted.at(i));
                }
            }else{
                LogBox->moveCursor(QTextCursor::End);
                LogBox->insertPlainText(str);
            }
            return n;
        }

    private:
        QPlainTextEdit* LogBox;
};

namespace Ui {
    class DFormLog;
}

class DFormLog : public QDialog
{
    Q_OBJECT

    public:
        static void Create(void);
        explicit DFormLog(QSharedPointer<DTools::DLog> dLog = nullptr, QSharedPointer<DTools::DPreferences> dPreferences = nullptr,QWidget *parent = nullptr);
        ~DFormLog();

        void Debug(QString LogMsg);

        // Pointer to be feed
        std::ostream *LogStream;

    private slots:
        void on_pushButton_clicked();
        void on_ButtonOpenFolder_clicked();
        void on_ButtonSendLogs_clicked();

    private:
        Ui::DFormLog *ui;
        QSharedPointer<DTools::DLog> DuryLog;
        QSharedPointer<DTools::DPreferences> Prefs;

        // Pointer to be feed
        DStreamBuff *dStreamBuff;
};

#endif
#endif // DFORMLOG_H
