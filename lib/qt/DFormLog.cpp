#include "libdpp/qt/DFormLog.h"

#ifdef QT_GUI_LIB

#include "libdpp/qt/ui_dformlog.h"
#include "libdpp/qt/DQt.h"
//#include <QtConcurrent/QtConcurrentRun>
#include <QFile>

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

DFormLog::DFormLog(QWidget *parent, std::shared_ptr<DTools::DLog> dLog) :
    QDialog(parent),
    ui(new Ui::DFormLog)
{
    ui->setupUi(this);
    ShowDateTime=true;
    ShowOutputLevel=true;
    if (dLog) {
        SetDLog(dLog);
    }
    setWindowTitle(dLog->GetFilename().c_str());
}

DFormLog::~DFormLog()
{
    DuryLog->SetCallback();
    delete ui;
}

/**
 * @brief Set Qt::WA_DeleteOnClose attribute.
 * @param Enabled   ->  if true the form instance wil be destroyed when closed.
 */
void DFormLog::SetDeleteOnClose(bool Enabled)
{
    setAttribute(Qt::WA_DeleteOnClose,Enabled);
}

/**
 * @brief Save window position into Prefs.
 * @param Prefs ->  a DTools::DPreferences in which save window position.
 */
void DFormLog::SaveWindowPosition(DTools::DPreferences &Prefs)
{
    if (!DTools::DWindow::SaveQWindowPosition(*this,&Prefs)) {
        DuryLog->e(tr("Failed to save log window position").toStdString());
    }
}

/**
 * @brief Restore window position into Prefs.
 * @param Prefs ->  a DTools::DPreferences from which read window position.
 */
void DFormLog::RestoreWindowPosition(DTools::DPreferences &Prefs)
{
    if (!DTools::DWindow::RestoreQWindowPosition(*this,&Prefs)) {
        DuryLog->e(tr("Failed to save log window position").toStdString());
    }
}

/**
 * @brief Set DTools::DLog class instance from which read log to show.
 * @param dLog  ->  std::shared_prt to an already create DTools::DLog class.
 */
void DFormLog::SetDLog(std::shared_ptr<DTools::DLog> dLog)
{
    DuryLog=dLog;
    if (DuryLog) {
        auto MemberDLogCallback=std::bind(&DFormLog::DLogCallback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        DuryLog->SetCallback(MemberDLogCallback);
        connect(this,&DFormLog::SignalAdd,this,&DFormLog::Add);
    }
}

/**
 * @brief Loads and show entire content of DTools::DLog file.
 * @return true on success, otherwise false.
 */
bool DFormLog::LoadFile(void) {
    std::string Filename=DuryLog->GetFilename();
    QFile LogFile(Filename.c_str());
    LogFile.open(QIODevice::Text | QIODevice::ReadOnly);
    if (!LogFile.isOpen()) {
        ui->PlainTextEditLog->appendPlainText(tr("ERROR loading ")+LogFile.fileName());
        return false;
    }

    ui->PlainTextEditLog->clear();
    while(!LogFile.atEnd()) {
        QString Line=LogFile.readLine();
        ui->PlainTextEditLog->appendPlainText(Line);
    }
    LogFile.close();

    return true;
}

/**
 * @brief Adds a message line.
 * @param Msg   ->  Log message string.
 * @param OutputLevel   ->  is used to
 * @param Header
 */
void DFormLog::Add(QString Msg, QString OutputLevel, QString Header)
{
    QString LogString;
    if (ShowDateTime)  {
        LogString.append(Header+" : ");
    }

    if (ShowOutputLevel) {
        LogString.append(OutputLevel);
    }

    LogString.append(Msg);

    ui->PlainTextEditLog->appendPlainText(Msg);
}

void DFormLog::DLogCallback(std::string Msg, std::string OutputLevel, std::string Header) {
    emit SignalAdd(Msg.c_str(),OutputLevel.c_str(),Header.c_str());
}

void DFormLog::on_ButtonOpenFolder_clicked()
{
    DTools::DShell::ShowInFolder(DuryLog->GetFilename().c_str());
}

void DFormLog::on_ButtonReload_clicked()
{
    LoadFile();
}

void DFormLog::on_ButtonSendLogs_clicked()
{

}
#endif

