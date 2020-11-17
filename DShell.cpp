#include "DShell.h"

#ifdef _WIN32
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
