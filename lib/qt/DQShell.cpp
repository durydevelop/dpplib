#include <libdpp/qt/DQShell.h>

#ifdef QT_GUI_LIB

#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

namespace DTools
{
namespace DShell
{
    void ShowInFolder(const QString& path) {
        QFileInfo info(path);
        if (!info.exists()) {
            return;
        }
        #if defined _WIN32 || defined _WIN64
            QStringList args;
            if (!info.isDir())
                args << "/select,";
            args << QDir::toNativeSeparators(path);
            if (QProcess::startDetached("explorer", args))
                return;
        #elif defined(Q_OS_MAC)
            QStringList args;
            args << "-e";
            args << "tell application \"Finder\"";
            args << "-e";
            args << "activate";
            args << "-e";
            args << "select POSIX file \"" + path + "\"";
            args << "-e";
            args << "end tell";
            args << "-e";
            args << "return";
            if (!QProcess::startDetached("/usr/bin/osascript", args))
                return;
        #else
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
        #endif
    }

    bool Execute(const QString& Filename, const QStringList& Args, int WaitMSecs, qint64 *Pid) {
        QFileInfo info(Filename);
        if (info.isDir()) return false;
        QProcess Process;

        Process.start(Filename,Args);
        if (WaitMSecs != 0) {
            if (!Process.waitForStarted(WaitMSecs)) {
                return false;
            }
        }
        if (Pid) *Pid=Process.processId();

        if (WaitMSecs != 0) {
            return(Process.waitForFinished(WaitMSecs));
        }

        return true;
    }

    /**
     * @brief ExecuteDetached
     * @param Filename
     * @param Args
     * @param Pid
     * @return
     */
    bool ExecuteDetached(const QString& Filename, const QStringList& Args, qint64 *Pid) {
        QFileInfo info(Filename);
        if (info.isDir()) return false;
        return(QProcess::startDetached(Filename,Args,info.dir().absolutePath(),Pid));
    }
}
}

#endif
