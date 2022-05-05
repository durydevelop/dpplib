#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QtQuickWidgets/QQuickWidget>
#include <QTreeWidgetItem>
#include <QRegularExpression>
#include "libdpp/qt/DQt.h"
#include <sstream>

using namespace DTools;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dLog=new DLog();
    Log("Start");
    #ifdef __MINGW64__
        Log("MINGW64 VERSION " __MINGW64_VERSION_STR);
    #endif
    #ifdef __MINGW32__
        Log("MINGW32 VERSION "+std::to_string(__MINGW32_MAJOR_VERSION)+"."+std::to_string(__MINGW32_MINOR_VERSION));
    #endif
    Log("__cplusplus = "+std::to_string(__cplusplus));
    Log("__GNUC__ = "+std::to_string(__GNUC__)+"."+std::to_string(__GNUC_MINOR__)+"."+std::to_string(__GNUC_PATCHLEVEL__));

    //connect(this,&MainWindow::AddLog,this,&MainWindow::PrintLog); //,Qt::QueuedConnection);

    Watcher=nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Log(std::string LogStr) {
    ui->TextEditLog->append(LogStr.c_str());
    dLog->d(LogStr);
}

// *****************************************************************************************************************
// *********************************** DPathWatcher ****************************************************************
// *****************************************************************************************************************

void MainWindow::WatcherChangeCallback(DPathWatcher::DChangeStatus Status, fs::path File, std::string Msg) {
    std::string sStatus;
    switch (Status) {
        case DPathWatcher::CHANGE_STATUS_MODIFIED:
            sStatus="MODIFIED";
            break;
        case DPathWatcher::CHANGE_STATUS_CREATED:
            sStatus="CREATED";
            break;
        case DPathWatcher::CHANGE_STATUS_ERASED:
            sStatus="DELETED";
            break;
        case DPathWatcher::CHANGE_STATUS_NONE:
            sStatus="NO_CHANGE";
            break;
        case DPathWatcher::CHANGE_STATUS_LOG_STR:
            sStatus="";
            break;
    }

    //emit(AddLog(File.string()+" : "+sStatus));
    //PrintLog(File.string()+" : "+sStatus);
    ui->TextEditLog->append((File.string()+" : "+sStatus + Msg).c_str());
}

void MainWindow::Check(void) {
    Log("Check()");
    Watcher->Check();
}

void MainWindow::on_ButtonPathWatcher_clicked()
{
    //Watcher=new DPathWatcher("E:\\Fabio_Share\\appunti.txt");
    Watcher=new DPathWatcher();
    Watcher->AddPath(ui->EditWatcher->text().toStdString(),false);
    auto callback=std::bind(&MainWindow::WatcherChangeCallback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    Watcher->SetCallback(callback);
    Log("Creato");
}

void MainWindow::on_ButtonCheck_clicked()
{
    Watcher->Check();
}

void MainWindow::on_ButtonStart_clicked()
{
    Watcher->Start();
}

void MainWindow::on_ButtonStop_clicked()
{
    Watcher->SetStopAndWait(5);
}

void MainWindow::on_ButtonWatcherAlive_clicked()
{
    if (Watcher->IsWatching()) {
        Log("Watching");
    }
    else {
        Log("Not Watching");
    }
}

void MainWindow::on_ButtonWatcherBrowseFile_clicked()
{
    QString Filename=QFileDialog::getOpenFileName(this, tr("File to watch"),QDir(ui->EditWatcher->text()).path());
    if (!Filename.isEmpty() || Filename != ui->EditWatcher->text()) {
        ui->EditWatcher->setText(Filename);
    }
}

void MainWindow::on_ButtonWatcherBrowseDir_clicked()
{
    QString Dirname=QFileDialog::getExistingDirectory(this, tr("Folder to watch"),QDir(ui->EditWatcher->text()).path());
    if (!Dirname.isEmpty() || Dirname != ui->EditWatcher->text()) {
        ui->EditWatcher->setText(Dirname);
    }
}

void MainWindow::on_ButtonQMLShow_clicked()
{
    QQuickWidget *BusyWidget;
    BusyWidget=new QQuickWidget(ui->FrameSpinner);
    BusyWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    if (ui->CheckBoxQMLSplashScreen->isChecked()) {
        BusyWidget->setWindowFlags(Qt::SplashScreen);
    }
    BusyWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    BusyWidget->setAttribute(Qt::WA_TranslucentBackground);
    BusyWidget->setClearColor(Qt::transparent);
    QUrl f=QUrl::fromLocalFile(ui->EditQML->text());
    BusyWidget->setSource(f);
    BusyWidget->show();
}

void MainWindow::on_ButtonQMLBrowseFile_clicked()
{
    QString Filename=QFileDialog::getOpenFileName(this, tr("QML widget file"),QDir(ui->EditQML->text()).path());
    if (!Filename.isEmpty() || Filename != ui->EditQML->text()) {
        ui->EditQML->setText(Filename);
    }
}

void MainWindow::on_ButtonUdpServer_clicked()
{

}

void MainWindow::on_ButtonDPathBrowseDir_clicked()
{
    QString Dirname=QFileDialog::getExistingDirectory(this, tr("Folder to scan"),QDir(ui->EditDPath->text()).path());
    if (!Dirname.isEmpty() || Dirname != ui->EditDPath->text()) {
        ui->EditDPath->setText(Dirname);
    }
}

void MainWindow::on_ButtonDPathSearch_clicked()
{
    QString sContent=ui->TextEditExtList->toPlainText();
    //QStringList strList=sContent.split(QRegExp("[\n]"),QString::SkipEmptyParts);
    std::string extList=sContent.toStdString();
    sContent=ui->TextEditNameList->toPlainText();
    std::string nameList=sContent.toStdString();
    size_t n=0;
    bool StopRequest=false;
    if (ui->RadioButtonDPathFiles->isChecked()) {
        n=DPath::CountFiles(ui->EditDPath->text().toStdString(),ui->CheckBoxDPathRecoursive->isChecked(),nameList,false,extList,false,ui->CheckBoxDPathCase->isChecked(),StopRequest);
    }
    else {
        n=DPath::CountDirs(ui->EditDPath->text().toStdString(),ui->CheckBoxDPathRecoursive->isChecked(),nameList,false,extList,false,ui->CheckBoxDPathCase->isChecked(),StopRequest);
    }
    ui->TextEditLog->append(ui->EditDPath->text()+" : "+QString::number(n));
}

void MainWindow::on_ButtonDShellExecute_clicked()
{
    QString Filename=ui->EditDShell->text();
    QStringList ArgList=ui->TextEditDShellArgs->toPlainText().split(QRegularExpression("[\n]"),Qt::SkipEmptyParts);
    qint64 Pid;
    if (ui->CheckBoxExecuteDetached->isChecked()) {
        DShell::ExecuteDetached(Filename,ArgList,&Pid);
    }
    else {
        DShell::Execute(Filename,ArgList,ui->SpinBoxWaitMillis->value(),&Pid);
    }
}

void MainWindow::OnResponse(DNetwork::DRESTClient::DHttpResponse HttpResponse)
{
    std::stringstream ss;
    ss << "HttpResponse:" << std::endl;
    ss << HttpResponse << std::endl;
    Log(ss.str());
}

void MainWindow::OnLog(std::string LogMsg)
{
    Log("DEBUG : "+LogMsg);
}

void MainWindow::on_ButtonRestClientCreate_clicked()
{
    if (RESTClient) {
        ui->TextEditLog->append("DRESTClient already created");
        return;
    }

    RESTClient=std::make_shared<DNetwork::DRESTClient>();
    if (RESTClient) {
        ui->TextEditLog->append("DRESTClient created");
    }

    auto ResponseCallback=std::bind(&MainWindow::OnResponse,this,std::placeholders::_1);
    RESTClient->SetOnResponse(ResponseCallback);
    auto LogCallback=std::bind(&MainWindow::OnLog,this,std::placeholders::_1);
    RESTClient->SetOnLog(LogCallback);
}


void MainWindow::on_ButtonRestClientConnect_clicked()
{
    //RESTClient->SetUrl(ui->EditRestClientUrl->text().toStdString());
    if(RESTClient->Connect(ui->EditRestClientUrl->text().toStdString())) {
        ui->TextEditLog->append(("DRESTClient Connected to "+RESTClient->GetUri().UriString).c_str());
    }
    else {
        ui->TextEditLog->append(("Error connecting to "+RESTClient->GetUri().UriString+" : "+RESTClient->GetLastError()).c_str());
    }
}

void MainWindow::on_ButtonRestClientPost_clicked()
{
    //RESTClient->ClearBody();
    RESTClient->SetHttpReqContentType(DNetwork::DRESTClient::CONTENT_TYPE_URL_ENCODED);
    RESTClient->SetHttpReqKeepAlive(true);

    for (int ixI=0; ixI<ui->TreeWidgetRestClientBody->topLevelItemCount(); ixI++) {
        QTreeWidgetItem *Item=ui->TreeWidgetRestClientBody->topLevelItem(ixI);
        RESTClient->AddReqBodyParam(Item->data(0,Qt::EditRole).toString().toStdString(),Item->data(1,Qt::EditRole).toString().toStdString());
    }

    if (RESTClient->SendPOST()) {
        ui->TextEditLog->append("Sent and back");
    }
    else {
        ui->TextEditLog->append(("Error send : "+RESTClient->GetLastError()).c_str());
    }

}

void MainWindow::on_ButtonRestClientDIsconnect_clicked()
{
    if (RESTClient->Disconnect()) {
        ui->TextEditLog->append("Disconnected");
    }
    else {
        ui->TextEditLog->append(("Error disconnect : "+RESTClient->GetLastError()).c_str());
    }
}


void MainWindow::on_ButtonRestClientOneShot_clicked()
{
    on_ButtonRestClientConnect_clicked();
    on_ButtonRestClientPost_clicked();
    on_ButtonRestClientDIsconnect_clicked();
}


void MainWindow::on_ButtonRestClientDelete_clicked()
{
    if (RESTClient) {
        RESTClient.reset();
    }
}

// ***********************************************************************************
// ************************ DTree ****************************************************
// ***********************************************************************************

void MainWindow::on_ButtonDTreeLoad_clicked()
{

    std::stringstream json;
    json << ui->TextEditDTreeContent->toPlainText().toStdString();
    if (dTree.LoadJsonText(json)) {
        Log("Json Content Loaded");
    }
    else {
        Log("Json Content Load error: "+dTree.GetLastStatus());
    }
}

void MainWindow::on_ButtonDTreeReadString1_clicked()
{
    Log(dTree.ReadString(ui->EditDTreeSubTree1->text().toStdString(),ui->EditDTreeItem1->text().toStdString(),ui->EditDTreeDefault1->text().toStdString()));
}

void MainWindow::on_ButtonDTreeRootCount_clicked()
{
    Log(std::to_string(dTree.ChildrenCount(ui->EditDTreeSubTreeCount->text().toStdString())));
}

void MainWindow::on_ButtonDTreeReadString2_clicked()
{
    Log(dTree.ReadString(ui->EditDTreeSubTree2->text().toStdString(),ui->EditDTreeDefault1->text().toStdString()));
}

void MainWindow::on_ButtonDTreeReadTree_clicked()
{
    DTree Tree(dTree.ReadTree(ui->EditDTreeSubTree2->text().toStdString()));
    Log("HasChildern:  "+std::to_string(Tree.HasChildern()));
    Log("ChidrenCount: "+std::to_string(Tree.ChildrenCount()));
    Log("HasData:      "+std::to_string(Tree.HasData()));
}

void MainWindow::on_ButtonDTreeCounts_clicked()
{
    Log("HasChildern:  "+std::to_string(dTree.HasChildren(ui->EditDTreeSubTree2->text().toStdString())));
    Log("ChidrenCount: "+std::to_string(dTree.ChildrenCount(ui->EditDTreeSubTree2->text().toStdString())));
    Log("HasData:      "+std::to_string(dTree.HasData(ui->EditDTreeSubTree2->text().toStdString())));
}

