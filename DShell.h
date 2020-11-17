#ifndef DShellH
#define DShellH

#include <string>

#ifdef _WIN32
	#include <windows.h>
#endif

namespace DShell {
	bool Execute(std::string Filename, std::string Args, bool Visible);
/*
		HANDLE ShellEsegui(std::string NomeFile, bool Visibile, AnsiString Parametri);
		HANDLE ShellEsegui(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa);
		HANDLE ShellEsegui(AnsiString NomeFile);
		DWORD ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa);
		DWORD ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri);
		bool ShellFind(AnsiString sCerca, AnsiString sPercorso);
*/
};


#endif
