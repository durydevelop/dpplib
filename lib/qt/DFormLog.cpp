#include "libdpp/qt/DFormLog.h"

#ifdef QT_GUI_LIB

//#include "libdpp/qt/ui_dformlog.h"
#include "libdpp/qt/DQt.h"
#include <QtConcurrent/QtConcurrentRun>

/**
 * Create autodelete form:
 * @code
 QPointer<DFormLog> FormLog;
 FormLog=new DFormLog(Log,Params->Prefs,this);
 FormLog->setAttribute(Qt::WA_DeleteOnClose,true);
 FormLog->show();

 MainWindow::~MainWindow()
 {
    if (FormLog) {
        delete FormLog;
    }
 }
 * @endcode
 *
 */

DFormLog::DFormLog(DTools::DLog *dLog, DTools::DPreferences *dPreferences, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DFormLog)
{
    ui->setupUi(this);
    dStreamBuff=new DStreamBuff(ui->PlainTextEditLog);
    LogStream=new std::ostream(dStreamBuff);
    DuryLog=dLog;
    if (DuryLog) {
        DuryLog->SetOutputStream(LogStream);
    }
    Prefs=dPreferences;
    if (!DTools::DWindow::RestoreQWindowPosition(*this,Prefs)) {
        DuryLog->e(tr("Failed to restore log window position").toStdString());
    }

    QFile LogFile(dLog->GetFilename().c_str());
    LogFile.open(QIODevice::Text | QIODevice::ReadOnly);
    while(!LogFile.atEnd()) {
        ui->PlainTextEditLog->appendPlainText(LogFile.readLine());
    }
    LogFile.close();
}

DFormLog::~DFormLog()
{
    if (!DTools::DWindow::SaveQWindowPosition(*this,Prefs)) {
        DuryLog->e(tr("Failed to save log window position").toStdString());
    }

    if (DuryLog) DuryLog->SetOutputStream();
    delete LogStream;
    delete dStreamBuff;
    delete ui;
}

void DFormLog::Debug(QString LogMsg)
{
    *LogStream << LogMsg.toStdString() << std::endl;
}

void DFormLog::on_pushButton_clicked()
{
    //Debug("Prova 1");
    //Debug("Prova 2");

    //ui->PlainTextEditLog->setPlainText(QString::fromStdString(DuryLog->Read()));
    //ui->PlainTextEditLog->moveCursor(QTextCursor::End);
}

void DFormLog::on_ButtonOpenFolder_clicked()
{
    DTools::DShell::ShowInFolder(DuryLog->GetFilename().c_str());
}

void DFormLog::on_ButtonSendLogs_clicked()
{

}
#endif
