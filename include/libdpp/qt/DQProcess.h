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
        DQProcess();
        QProcess Process;
        void OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void OnProcessReadyReadStandardOutput();
        void OnProcessReadyReadStandardError();
};
}

#endif
#endif
