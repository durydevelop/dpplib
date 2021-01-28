#include "libdpp/DShell.h"

#ifdef _WIN32
    bool DShell::Execute(std::string Filename, std::string Args) {
        return(Execute(Filename,Args,true));
    }

    bool DShell::Execute(std::string Filename, std::string Args, bool Visible) {
        SHELLEXECUTEINFO exec_info;

        if (Visible) {
            exec_info.nShow = SW_SHOW;
        }
        else {
            exec_info.nShow = SW_HIDE;
        }

        exec_info.cbSize = sizeof(SHELLEXECUTEINFO);
        exec_info.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
        exec_info.hwnd = NULL;
        exec_info.lpVerb = NULL;
        exec_info.lpFile = Filename.c_str();
        exec_info.lpParameters = Args.c_str();
        exec_info.lpDirectory = NULL;

        return(ShellExecuteEx(&exec_info));
    }
#elif __linux__
    bool DShell::Execute(std::string Filename, std::string Args) {
        char *my_args[4];

          my_args[0] = "child.exe";
          my_args[1] = "arg1";
          my_args[2] = "arg2";
          my_args[3] = NULL;

          puts("Spawning child with spawnv");

          spawnv( P_WAIT, "child.exe", my_args);
    }
#endif

//! Start a shutdown procedure
bool DShell::ShutDown(void)
{
    #if defined _WIN32 || defined _WIN64
        // Windows API shutdown code
        HANDLE hToken;
        // Get a token for this process.
        if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken)) {
            return(FALSE);
        }

        TOKEN_PRIVILEGES tkp;
        // Get the LUID for the shutdown privilege.
        LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);

        tkp.PrivilegeCount=1;  // one privilege to set
        tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

        // Get the shutdown privilege for this process.
        AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);

        if (GetLastError() != ERROR_SUCCESS) {
            return FALSE;
        }

        // Shut down the system and force all applications to close.
        if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED)) {
            return FALSE;
        }

        return TRUE;
    #elif defined __unix__
        #if defined QT_CORE_LIB
            return(QProcess::startDetached("shutdown -P now"));
        #else
            return(system("shutdown -P now"));
        #endif
    #endif
}


/*
    HANDLE DShell::ShellEsegui(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa)
    {
     HANDLE hProcesso;

      hProcesso = ShellEsegui(NomeFile, Visibile, Parametri);
      if (hProcesso == NULL) {
        return(NULL);
      }
      else{
        DWORD dwRet = WaitForSingleObject(hProcesso, mSecAttesa);
        if ( dwRet == WAIT_TIMEOUT){
            return NULL;
        }
        else if (dwRet == WAIT_FAILED){
            return NULL;
        }
        else if (dwRet == WAIT_OBJECT_0){
            return hProcesso;
        }
      }
      return NULL;
    }

    HANDLE DShell::ShellEsegui(AnsiString NomeFile)
    {
        return(ShellEsegui(NomeFile,true,""));
    }

    DWORD DShell::ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri, unsigned long mSecAttesa)
    {
     DWORD exit_code;
     HANDLE hProcesso;

      hProcesso=ShellEsegui(NomeFile, Visibile, Parametri, mSecAttesa);
      if (hProcesso == NULL) {
        return (-1);
      }

      if (GetExitCodeProcess(hProcesso, &exit_code)){
        return exit_code;
      }
      else{
        return (-2);
      }
    }
    DWORD DShell::ShellEseguiEx(AnsiString NomeFile, bool Visibile, AnsiString Parametri)
    {
        return(ShellEseguiEx(NomeFile,Visibile,Parametri, INFINITE));
    }
    //------------------------------------------------------------------------------

    // ***********************************************************************
    // ** Apre una finestra di explorer con selezionato                     **
    // ** il primo file trovato che contiene la stringa <AnsiString sCerca> **
    // ** AnsiString sCerca    -> Stringa da cercare                        **
    // ** AnsiString sPercorso -> Percorso in cui cercare                   **
    // ** Se trova un file o una directory in cui è presente la stringa     **
    // ** <sCerca> apre explorer con l'oggetto selezionato                  **
    // ** atrimenti apre explorer nella cartella <sPercorso>                **
    // ***********************************************************************
    bool DShell::ShellFind(AnsiString sCerca, AnsiString sPercorso)
    {
     SHELLEXECUTEINFO exec_info;
     TSearchRec sr;
     TCursor cCursor;
     char sMess[255+40];
     AnsiString sExplorer;

        cCursor=Screen->Cursor;
        Screen->Cursor=crHourGlass;
        if (!sPercorso.IsPathDelimiter(sPercorso.Length())) {
            sPercorso+="\\";
        }
        exec_info.lpParameters=sPercorso.c_str();
        if (sCerca != "") {
            if (FindFirst(sPercorso+"*.*",faAnyFile,sr) == 0) {
                do {
                    if (sr.Name.Pos(sCerca) > 0) {
                        sExplorer="/select,"+sPercorso+sr.Name;
                        exec_info.lpParameters=sExplorer.c_str();
                        break;
                    }
                }while(FindNext(sr) == 0);
                FindClose(sr);
            }

            if (exec_info.lpParameters[0] != '/') {
                wsprintf(sMess,"<%s> non è stato trovato",sCerca.c_str());
                MessageBox(NULL,sMess,"Attenzione",MB_OK);
            }
        }

      exec_info.nShow=SW_SHOW;
      exec_info.cbSize = sizeof(SHELLEXECUTEINFO);
      exec_info.fMask =SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI; //SEE_MASK_NOCLOSEPROCESS |
      exec_info.hwnd = NULL;
      exec_info.lpVerb = NULL;
      exec_info.lpFile="Explorer";
      exec_info.lpDirectory = NULL;

      ShellExecuteEx(&exec_info);
      if (exec_info.hInstApp <= (HINSTANCE)32)
        {
         MessageBox(NULL, "Errore esecuzione Comando Shell", "ATTENZIONE", MB_OK );
         //char serr[255];
         //StoreErrorText(GetLastError(),serr,255);
         return false;
        }

      Screen->Cursor=cCursor;
    return true;
    }
#endif
*/
