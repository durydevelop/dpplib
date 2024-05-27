#ifndef DQSHELL_H
#define DQSHELL_H
#ifdef QT_GUI_LIB

#include <QStringList>

namespace DTools
{
namespace DShell
{
    void ShowInFolder(const QString& path);
    bool Execute(const QString& Filename, const QStringList& Args = QStringList(), int WaitMSecs = 30000, qint64 *Pid = nullptr);
    bool ExecuteDetached(const QString& Filename, const QStringList& Args = QStringList(),qint64 *Pid = nullptr);
}
}
#endif
#endif
