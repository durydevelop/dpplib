#ifndef DQPROCESS_H
#define DQPROCESS_H
#ifdef QT_GUI_LIB

#include <QProcess>

namespace DTools
{
class DQProcess : public QObject
{
    Q_OBJECT
    public:
        enum DOutputChannel { DSTD_OUT, DSTD_ERR };
        enum DExitStatus    { DEXIT_NORMAL, DEXIT_CRASH };
        typedef std::function<void (DOutputChannel OutputChannel, std::string Line)> DOnStdOutErrCallback;
        typedef std::function<void (int ExitCode, DExitStatus ExitStatus)> DOnFinishedCallback;

        DQProcess(std::string Filename, std::string WorkingDir);

        void SetOnStdOutErrCallback(DOnStdOutErrCallback Callback);
        void SetOnFinishedCallback(DOnFinishedCallback Callback);

        DOnFinishedCallback GetOnFinishedCallback(void);
        DOnStdOutErrCallback GetOnStdOutErrCallback(void);
        std::string GetFilename(void);
        QProcess::ProcessState GetState(void);

        bool Exec(size_t TimeoutMs);

        bool AllowMultipleInstances;

    private:
        void OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void OnProcessReadyReadStandardOutput();
        void OnProcessReadyReadStandardError();

        QProcess Process;
        DOnStdOutErrCallback OnStdOutErrCallback;
        DOnFinishedCallback OnFinishedCallback;
};
}

#endif
#endif
