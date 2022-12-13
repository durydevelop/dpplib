#ifndef DShellH
#define DShellH

#include <string>

#ifdef QT_CORE_LIB
    #include <libdpp/qt/DQShell.h>
#endif

namespace DTools
{
namespace DShell
{
    #ifdef _WIN32
        bool Execute(std::string Filename, std::string Args, bool Visible);
    #endif
    bool Execute(std::string Filename, std::string Args);
    #if defined _WIN32 || defined _WIN64
        bool ExecuteDetached(std::string Filename, std::string Args);
    #endif
	bool ShutDown(void);
/*
		HANDLE ShellEsegui(std::string NomeFile, bool Visibile, AnsiString Parametri);
		HANDLE ShellEsegui(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa);
		HANDLE ShellEsegui(AnsiString NomeFile);
		DWORD ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa);
		DWORD ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri);
		bool ShellFind(AnsiString sCerca, AnsiString sPercorso);
*/
} // DShell
} // DTools


#endif
