#include "libdpp/DShell.h"

#include "libdpp/DCsv.h"
#include "libdpp/DString.h"

#ifdef __linux__
    #include <spawn.h>
    #include <string.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

#if defined QT_CORE_LIB
    #include <QProcess>
#endif

namespace DTools
{
namespace DShell
{
    #ifdef _WIN32
        bool Execute(std::string Filename, std::string Args) {
            return(Execute(Filename,Args,true));
        }

        bool Execute(std::string Filename, std::string Args, bool Visible) {
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
            #ifdef UNICODE
                exec_info.lpFile = DString::ToWideStr(Filename).c_str();
                exec_info.lpParameters = DString::ToWideStr(Args).c_str();
            #else
                exec_info.lpFile = Filename.c_str();
                exec_info.lpParameters = Args.c_str();
            #endif
            exec_info.lpDirectory = NULL;

            return(ShellExecuteEx(&exec_info));
        }
    #elif __linux__
        bool Execute(std::string Filename, std::string Args) {
            // String to std::vector
            std::vector<std::string> vArgs;
            DCsv::CsvToVector(vArgs,Args,' ');
            // Insert filename as firts arg
            vArgs.insert(vArgs.begin(),Filename);
            // std::vector to array of char*
            char *pArgs[vArgs.size()];
            for(size_t ixA=1;ixA<vArgs.size();ixA++) {
                size_t len=vArgs[ixA].size();
                pArgs[ixA]=new char[len+1];
                strncpy(pArgs[ixA],vArgs[ixA].c_str(),len);
                *pArgs[len]='\0';
            }
            // Spawn process
            pid_t pid=0;
            if (posix_spawn(&pid,Filename.c_str(),NULL,NULL,pArgs,NULL) != 0) {
                return false;
            }
            // TODO wait
            return true;
        }
    #endif

#if defined _WIN32 || defined _WIN64
    //! For now only for compatibily with DQt.h
    bool ExecuteDetached(std::string Filename, std::string Args) {
        // TODO: start in new process
        return(Execute(Filename,Args));
    }
#endif

    //! Start a shutdown procedure
    bool ShutDown(void)
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
                return(QProcess::startDetached("shutdown",QStringList({"-P","now"})));
            #else
                return(system("shutdown -P now"));
            #endif
        #endif
    }
} // DShell
} // DTools

/* TODO:
#include <spawn.h>
      #include <stdint.h>
      #include <stdio.h>
      #include <unistd.h>
      #include <stdlib.h>
      #include <string.h>
      #include <wait.h>
      #include <errno.h>

      #define errExit(msg)    do { perror(msg); \
                                   exit(EXIT_FAILURE); } while (0)

      #define errExitEN(en, msg) \
                              do { errno = en; perror(msg); \
                                   exit(EXIT_FAILURE); } while (0)

      char **environ;

      int
      main(int argc, char *argv[])
      {
          pid_t child_pid;
          int s, opt, status;
          sigset_t mask;
          posix_spawnattr_t attr;
          posix_spawnattr_t *attrp;
          posix_spawn_file_actions_t file_actions;
          posix_spawn_file_actions_t *file_actionsp;

          // Parse command-line options, which can be used to specify an
          //   attributes object and file actions object for the child.

          attrp = NULL;
          file_actionsp = NULL;

          while ((opt = getopt(argc, argv, "sc")) != -1) {
              switch (opt) {
              case 'c':       // -c: close standard output in child

                  // Create a file actions object and add a "close"
                  //   action to it.

                  s = posix_spawn_file_actions_init(&file_actions);
                  if (s != 0)
                      errExitEN(s, "posix_spawn_file_actions_init");

                  s = posix_spawn_file_actions_addclose(&file_actions,
                                                        STDOUT_FILENO);
                  if (s != 0)
                      errExitEN(s, "posix_spawn_file_actions_addclose");

                  file_actionsp = &file_actions;
                  break;

              case 's':       // -s: block all signals in child

                  // Create an attributes object and add a "set signal mask"
                  //   action to it.

                  s = posix_spawnattr_init(&attr);
                  if (s != 0)
                      errExitEN(s, "posix_spawnattr_init");
                  s = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);
                  if (s != 0)
                      errExitEN(s, "posix_spawnattr_setflags");

                  sigfillset(&mask);
                  s = posix_spawnattr_setsigmask(&attr, &mask);
                  if (s != 0)
                      errExitEN(s, "posix_spawnattr_setsigmask");

                  attrp = &attr;
                  break;
              }
          }

          // Spawn the child. The name of the program to execute and the
          //   command-line arguments are taken from the command-line arguments
          //   of this program. The environment of the program execed in the
          //   child is made the same as the parent's environment.

          s = posix_spawnp(&child_pid, argv[optind], file_actionsp, attrp,
                           &argv[optind], environ);
          if (s != 0)
              errExitEN(s, "posix_spawn");

          // Destroy any objects that we created earlier

          if (attrp != NULL) {
              s = posix_spawnattr_destroy(attrp);
              if (s != 0)
                  errExitEN(s, "posix_spawnattr_destroy");
          }

          if (file_actionsp != NULL) {
              s = posix_spawn_file_actions_destroy(file_actionsp);
              if (s != 0)
                  errExitEN(s, "posix_spawn_file_actions_destroy");
          }

          printf("PID of child: %jd\n", (intmax_t) child_pid);

          // Monitor status of the child until it terminates.

          do {
              s = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
              if (s == -1)
                  errExit("waitpid");

              printf("Child status: ");
              if (WIFEXITED(status)) {
                  printf("exited, status=%d\n", WEXITSTATUS(status));
              } else if (WIFSIGNALED(status)) {
                  printf("killed by signal %d\n", WTERMSIG(status));
              } else if (WIFSTOPPED(status)) {
                  printf("stopped by signal %d\n", WSTOPSIG(status));
              } else if (WIFCONTINUED(status)) {
                  printf("continued\n");
              }
          } while (!WIFEXITED(status) && !WIFSIGNALED(status));

          exit(EXIT_SUCCESS);
      }
*/
