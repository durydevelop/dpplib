#include "libdpp/qt/DFormLog.h"

#ifdef QT_GUI_LIB
#include <QtConcurrent/QtConcurrentRun>
#include "libdpp/qt/ui_dformlog.h"
#include "libdpp/qt/DQShell.h"
#include "libdpp/qt/DQWindow.h"
#include <QBuffer>
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

#define DEFAULT_SEPARATOR " : "

namespace DTools
{
DFormLog::DFormLog(QWidget *parent, std::shared_ptr<DLog> dLog) :
    QDialog(parent),
    ui(new Ui::DFormLog)
{
    ui->setupUi(this);
    ShowDateTime=true;
    ShowOutputLevel=true;
    Separator=DEFAULT_SEPARATOR;
    if (dLog) {
        SetDLog(dLog);
    }
    setWindowTitle(dLog->GetFilename().c_str());
    connect(this,&DFormLog::SignalWaitingSpinner,this,&DFormLog::OnWaitingSpinner);
    connect(this,&DFormLog::SignalScrollToBottom,this,&DFormLog::OnScrollToBottom);

    ModelStringList = new QStringListModel();
    connect(&EmitStringList, &DEmitStringList::signal, ModelStringList, &QStringListModel::setStringList);
    ui->ListViewLog->setModel(ModelStringList);
    ui->ListViewLog->setUniformItemSizes(true);
    //connect(ui->ListViewLog->model(),&QStringListModel::rowsInserted,ui->ListViewLog,&QListView::scrollToBottom);
    connect(ui->ListViewLog->model(),&QStringListModel::rowsInserted,this,&DFormLog::rowsInserted);

    WaitingSpinner=new DQSpinnerWidget(ui->ListViewLog,true,false);
}

void DFormLog::rowsInserted() {
    qDebug("fired");
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
 * @param Prefs ->  a DPreferences in which save window position.
 */
void DFormLog::SaveWindowPosition(DPreferences &Prefs)
{
    if (!DWindow::SaveQWindowPosition(*this,&Prefs)) {
        DuryLog->e(tr("Failed to save log window position").toStdString());
    }
}

/**
 * @brief Restore window position into Prefs.
 * @param Prefs ->  a DPreferences from which read window position.
 */
void DFormLog::RestoreWindowPosition(DPreferences &Prefs)
{
    if (!DWindow::RestoreQWindowPosition(*this,&Prefs)) {
        DuryLog->e(tr("Failed to save log window position").toStdString());
    }
}

/**
 * @brief Set DLog class instance from which read log to show.
 * @param dLog  ->  std::shared_prt to an already create DLog class.
 */
void DFormLog::SetDLog(std::shared_ptr<DLog> dLog)
{
    DuryLog=dLog;
    if (DuryLog) {
        auto MemberDLogCallback=std::bind(&DFormLog::DLogCallback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        DuryLog->SetCallback(MemberDLogCallback);
        connect(this,&DFormLog::SignalAdd,this,&DFormLog::Add);
    }
}

/**
 * @brief Loads and show entire content of DLog file.
 * @return true on success, otherwise false.
 */
bool DFormLog::LoadFile(void) {

    emit SignalWaitingSpinner(true);
    QCoreApplication::processEvents();
    ModelStringList->setStringList(QStringList{});

    QFuture<bool> LoadRunner=QtConcurrent::run([this]{
        QFile File(DuryLog->GetFilename().c_str());
        QStringList lines;
        if (!File.open(QIODevice::ReadOnly)) {
            return false;
        }

        QTextStream in(&File);
        while (!in.atEnd()) {
            QString line = in.readLine();
            lines.append(line);
        }
        File.close();
        EmitStringList(lines);

        return true;
    });

    QTimer::singleShot(0,this,[this,LoadRunner](){
        LoadRunner.result();
        emit SignalWaitingSpinner(false);
        emit SignalScrollToBottom();
    });

    return true;
}

/**
 * @brief Adds a message line.
 * @param Msg   ->  Log message string.
 * @param OutputLevel   ->  is used to
 * @param Header
 */
void DFormLog::Add(QString Msg, QString OutputLevel, QString TimeStamp)
{
    QString LogString;

    if (ShowDateTime && !TimeStamp.isEmpty())  {
            LogString.append(TimeStamp+Separator);
    }

    if (ShowOutputLevel && !OutputLevel.isEmpty()) {
        LogString.append(OutputLevel+Separator);
    }

    LogString.append(Msg);

    if (ModelStringList->insertRow(ModelStringList->rowCount())) {
        QModelIndex index = ModelStringList->index(ModelStringList->rowCount() - 1, 0);
        ModelStringList->setData(index, LogString);
        ui->ListViewLog->scrollToBottom();
    }
}

void DFormLog::OnWaitingSpinner(bool Enable)
{
    if (Enable) {
        WaitingSpinner->start();
    }
    else {
        WaitingSpinner->stop();
    }
}

void DFormLog::OnScrollToBottom(void) {
    ui->ListViewLog->scrollToBottom();
}

void DFormLog::DLogCallback(std::string Msg, std::string OutputLevel, std::string Header) {
    emit SignalAdd(Msg.c_str(),OutputLevel.c_str(),Header.c_str());
}

void DFormLog::on_ButtonOpenFolder_clicked()
{
    DShell::ShowInFolder(DuryLog->GetFilename().c_str());
}

void DFormLog::on_ButtonReload_clicked()
{
    LoadFile();
}

void DFormLog::on_ButtonSendLogs_clicked()
{

}
}

/* TODO:
file = new QFile("file.txt");

if (!file->open(QIODevice::ReadOnly)) {
    qInfo() << "Error" << file->errorString();
    return;
}

QTextDocument *doc = this->document();
m_cursor = new QTextCursor(doc);

this->moveCursor(QTextCursor::Start);

auto *timer = new QTimer(this);
timer->setInterval(300);
timer->start();

connect(timer, &QTimer::timeout, [this, timer]() {
    if (is_adding) {
        qInfo() << "Already adding" << c1;
        return;
    }
    this->is_adding = true;
    startAppend();
    for (int i = 0; i < 1000; ++i) {
        if (file->atEnd()) {
            timer->stop();
            file->close();
            file->deleteLater();
            qInfo() << "done all";
            break;
        } else {
            c1++;
            append(QString::number(c1) + " - " + file->readLine());
        }
    }
    stopAppend();
    c2++;
    this->is_adding = false;
});
*/
#endif
