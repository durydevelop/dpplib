#include <dpplib/qt/DQProcess.h>
#include <dpplib/DFilesystem.h>

#ifdef QT_GUI_LIB

#include <QTextStream>

namespace DTools
{
    DQProcess::DQProcess(std::string Filename, std::string WorkingDir) {
        Process.setProgram(Filename.c_str());
        if (WorkingDir.empty()) {
            Process.setWorkingDirectory(fs::path(Filename).parent_path().string().c_str());
        }
        else {
            Process.setWorkingDirectory(QString::fromStdString(WorkingDir));
        }
        AllowMultipleInstances=false;
    }

    void DQProcess::SetOnStdOutErrCallback(DOnStdOutErrCallback Callback) {
        connect(&Process,&QProcess::readyReadStandardOutput,this,&DQProcess::OnProcessReadyReadStandardOutput);
        connect(&Process,&QProcess::readyReadStandardError,this,&DQProcess::OnProcessReadyReadStandardError);
        OnStdOutErrCallback=Callback;
    }

    void DQProcess::SetOnFinishedCallback(DOnFinishedCallback Callback) {
        connect(&Process,(void (QProcess::*)(int,QProcess::ExitStatus))&QProcess::finished,this,&DQProcess::OnProcessFinished);
        OnFinishedCallback=Callback;
    }

    DQProcess::DOnStdOutErrCallback DQProcess::GetOnStdOutErrCallback(void) {
        return OnStdOutErrCallback;
    }

    DQProcess::DOnFinishedCallback DQProcess::GetOnFinishedCallback(void) {
        return OnFinishedCallback;
    }


    void DQProcess::OnProcessFinished(int ExitCode, QProcess::ExitStatus ExitStatus) {
        if (OnFinishedCallback) {
            OnFinishedCallback(ExitCode,ExitStatus == QProcess::NormalExit ? DEXIT_NORMAL : DEXIT_CRASH);
        }
    }

    void DQProcess::OnProcessReadyReadStandardOutput() {
        Process.setReadChannel(QProcess::StandardOutput);
        QTextStream stream(&Process);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (OnStdOutErrCallback) {
                OnStdOutErrCallback(DSTD_OUT,line.toStdString());
            }
        }
    }

    void DQProcess::OnProcessReadyReadStandardError() {
        Process.setReadChannel(QProcess::StandardError);
        QTextStream stream(&Process);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (OnStdOutErrCallback) {
                OnStdOutErrCallback(DSTD_ERR,line.toStdString());
            }
        }
    /*
        QString perr = Process.readAllStandardError();
        if (perr.length()) {
            // Error perr);
        }
    */
    }

    std::string DQProcess::GetFilename(void) {
        return(Process.program().toStdString());
    }

    QProcess::ProcessState DQProcess::GetState(void) {
        return (Process.state());
    }

    bool DQProcess::Exec(size_t TimeOutMs) {
        if (AllowMultipleInstances && Process.state() != QProcess::NotRunning) {
                OnStdOutErrCallback(DSTD_ERR,"Process is running cannot start");
                return false;
        }

        OnStdOutErrCallback(DSTD_OUT,"Starting...");
        Process.start();
        if (TimeOutMs >= 0) {
            if (Process.waitForStarted(5000)) {
                OnStdOutErrCallback(DSTD_OUT,"Started");
                return true;
            }
            else {
                OnStdOutErrCallback(DSTD_ERR,"Not started: "+Process.errorString().toStdString());
                return false;
            }
        }
        else {
            OnStdOutErrCallback(DSTD_OUT,"Executed (no wait for started)");
            return true;
        }
    }
}

#endif
