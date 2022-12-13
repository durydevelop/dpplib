#include <libdpp/qt/DQProcess.h>

#ifdef QT_GUI_LIB

#include <QTextStream>

namespace DTools
{
DQProcess::DQProcess()
{
    connect(&Process,(void (QProcess::*)(int,QProcess::ExitStatus))&QProcess::finished,this,&DQProcess::OnProcessFinished);
    connect(&Process,&QProcess::readyReadStandardOutput,this,&DQProcess::OnProcessReadyReadStandardOutput);
    connect(&Process,&QProcess::readyReadStandardOutput,this,&DQProcess::OnProcessReadyReadStandardError);
}

void DQProcess::OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    //QProcess::NormalExit
    //QProcess::CrashExit
}

void DQProcess::OnProcessReadyReadStandardOutput()
{
    Process.setReadChannel(QProcess::StandardOutput);
    QTextStream stream(&Process);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        // extract progress info from line and etc.
    }
}

void DQProcess::OnProcessReadyReadStandardError()
{
    QString perr = Process.readAllStandardError();
    if (perr.length()) {
        // Error perr);
    }
}
}

#endif
