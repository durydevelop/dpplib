#include "libdpp/DPath.h"
#include "libdpp/DStringGrid.h"
#include "libdpp/DString.h"
#include "libdpp/DCsv.h"
#include "libdpp/DChrono.h"
#include <iostream>

#include <fcntl.h>
#ifndef O_BINARY
    #define O_BINARY 0
#endif

// Debug macro: if DEBUG is defined, DEBUG_PRINT(Msg) macro prints Msg on stdout, otherwise do nothing (I use it to debug issues).
#define DEBUG
#ifdef DEBUG
	#include<iostream>
	#define DEBUG_PRINT(Msg) std::cout << Msg << std::endl;
#else
	#define DEBUG_PRINT(Msg)
#endif

// Macro ridefined from basestd.h
// #define DIntToPtr(i) (void *)(uintptr_t)(i)

namespace DTools
{
namespace DPath
{

    //! @return filename of current executable.
    fs::path GetExeFilename() {
        #ifdef _WIN32
            wchar_t path[MAX_PATH] = { 0 };
            GetModuleFileNameW(nullptr, path, MAX_PATH);
            return path;
        #else
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            return std::string(result, (count > 0) ? count : 0);
        #endif
    }

    std::string GetExt(fs::path Path) {
        std::string Ext=Path.extension().string();
        return(DTools::DString::RemoveChars(Ext,"."));
    }

    /**
     * @brief Change the extension of a file/dir
     * @param Path      ->  Filename or directory.
     * @param NewExt    ->  New extension (with or without dot).
     * @param Execute   ->  If false only string new name is returned, if true also file/dir on disk is renamed.
     * @return new file/dir name or empty path if rename on disk fails.
     */
    fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute) {
        fs::path NewPath=Path.parent_path() / (Path.stem().string() + "." + DString::LTrim(NewExt,"."));

		if (Execute) {
			try {
				fs::rename(Path,NewPath);
			} catch (std::exception &e) {
				return (fs::path());
			}
		}
		return (NewPath);
	}

    /**
     * @brief Change the extension of a file/dir (error code version)
     * @param Path      ->  Filename or directory.
     * @param NewExt    ->  New extension (with or without dot).
     * @param Execute   ->  If false only string new name is returned, if true also file/dir on disk is renamed.
     * @param ec        ->  a pointer to std::error_code variable to fill with result (can be null).
     * @return new file/dir name or empty path if rename on disk fails.
     */
    fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code &ec) {
        fs::path NewPath=Path.parent_path() / (Path.stem().string() + "." + DString::LTrim(NewExt,"."));

		if (Execute) {
			fs::rename(Path,NewPath,ec);
			if (ec) {
				return (fs::path());
			}
		}
		return (NewPath);
	}

	/**
	 * @brief Read all extensions of a filename.
	 * @param Path	->	File/dir name to scan.
	 * @return a vector with all found extensions.
	 */
	std::vector<std::string> ReadAllExts(fs::path Path) {
		std::vector<std::string> Exts;
		size_t pos=Path.string().find('.');
		if (pos == std::string::npos) {
			// Non ci sono estensioni
			return(Exts);
		}
		std::string Ext=Path.string().substr(pos+1,Path.string().size()-pos-1);
		DCsv::ReadCSVRow(Exts,Ext,'.');
		return(Exts);
	}

	/**
	 * @brief Check if a file/dir is older than a number of hours.
	 * @param Path	->	File/dir to check.
	 * @param Hrs	->	Number of hours.
	 * @return true if file/dir has been modified last time before Hrs hours, otherwise false.
	 */
	bool IsOlderThanHrs(const fs::path& Path, const int Hrs) {
		// get now as time_point
		std::chrono::system_clock::time_point now=std::chrono::system_clock::now();
		// get file_time of file
		fs::file_time_type fttime=fs::last_write_time(Path);
		// convert to time_t
		std::time_t ftimet=DChrono::to_time_t(fttime);
		// then in time_point
		std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);
		// and make the difference as hours
		std::chrono::hours diff=std::chrono::duration_cast<std::chrono::hours>(now - tptime);

		return(diff.count() >= Hrs);
	}

	/**
     * @brief Retrive the last modification time of file/dir.
	 * @param Path	->	file/dir to check.
	 * @return a time_point format time.
	 */
	std::chrono::system_clock::time_point LastWriteTime(const fs::path Path) {
		// get file_time of file
		err::error_code ec;
		fs::file_time_type fttime=fs::last_write_time(Path,ec);
		if (ec.value() > 0) {
			std::string err=ec.message();
			return(std::chrono::system_clock::time_point());
		}

		// convert to time_t C++20
		//auto cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(ftime));

		// convert to time_t gcc 7 e 8
		//std::time_t ftimet=std::chrono::system_clock::to_time_t(fttime);

		std::time_t ftimet=DTools::DChrono::to_time_t(fttime);

		// then in time_point
		std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);

		return(tptime);
	}

    std::string GetPermissionsString(fs::path Path) {
        fs::perms p=fs::status(Path).permissions();
        return(GetPermissionsString(p));
    }

    std::string GetPermissionsString(fs::perms p) {
        std::stringstream ss;
        ss << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
           << ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
           << ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
           << ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
           << ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
           << ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
           << ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
           << ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
           << ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-");

        return(ss.str());
    }

#ifdef _WIN32
    /**
    * @param genericAccessRights	->	GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL
    **/
    bool CanAccess(fs::path Path, DWORD AccessRights) {
        DWORD genericAccessRights=AccessRights;
        bool bRet = false;
        DWORD length = 0;
         if (!GetFileSecurityA(Path.string().c_str(), OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL, 0, &length ) && ERROR_INSUFFICIENT_BUFFER == ::GetLastError()) {
            PSECURITY_DESCRIPTOR security = static_cast< PSECURITY_DESCRIPTOR >( ::malloc( length ) );
            if (security && GetFileSecurityA(Path.string().c_str(),OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, security, length, &length )) {
                HANDLE hToken = NULL;
                if (OpenProcessToken(GetCurrentProcess(),TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken)) {
                    HANDLE hImpersonatedToken = NULL;
                    if (DuplicateToken( hToken, SecurityImpersonation, &hImpersonatedToken)) {
                        GENERIC_MAPPING mapping = { 0xFFFFFFFF ,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF };
                        PRIVILEGE_SET privileges = {};
                        DWORD grantedAccess = 0, privilegesLength = sizeof( privileges );
                        BOOL result = FALSE;

                        mapping.GenericRead = FILE_GENERIC_READ;
                        mapping.GenericWrite = FILE_GENERIC_WRITE;
                        mapping.GenericExecute = FILE_GENERIC_EXECUTE;
                        mapping.GenericAll = FILE_ALL_ACCESS;

                        MapGenericMask( &genericAccessRights, &mapping );
                        if (AccessCheck(security,hImpersonatedToken,genericAccessRights,&mapping,&privileges,&privilegesLength,&grantedAccess,&result)) {
                            bRet = (result == TRUE);
                        }
                        CloseHandle( hImpersonatedToken );
                    }
                    CloseHandle( hToken );
                }
                free( security );
            }
        }

        return bRet;
    }
#elif __linux__
    bool CanAccess(fs::path Path, fs::perms AccessRights) {
        fs::perms p=fs::status(Path).permissions();
        bool Ret=(p & AccessRights) == AccessRights ? true : false;
        return Ret;
    }
#endif

	//! TODO provare
	std::uintmax_t space_to_be_freed(const fs::path& dir, unsigned int percent_free_required) {
		const fs::space_info space_info = fs::space(dir) ;
		std::uintmax_t required = space_info.capacity * percent_free_required / 100.0 ;
		return space_info.available < required ? required - space_info.available : 0 ;
	}

	//! Copy a file.
	/**
	* @param From	->	Source File.
	* @param To		->	Destination file.
	*
	* @return 0 on success or an error_code if any arrors occours.
	**/
	bool Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode) {
		if (From == To) {
			return false;
		}

		#if __cplusplus > 201402L // C++17
			fs::copy_options options=OverwriteExisting ? fs::copy_options::update_existing : fs::copy_options::none;
		#else
			fs::copy_option options=OverwriteExisting ? fs::copy_option::overwrite_if_exists : fs::copy_option::none;
		#endif

        // Store file permission
        fs::perms FilePerms=fs::status(From).permissions();

		err::error_code ec;
		if (SafeMode) {
			//std::cout << "Safemode copy " << From.string() << " -> " << To.string() << std::endl;
			if (OverwriteExisting) {
				if (Exists(To)) {
					// Safe mode work-around:
					// delete dest before copy (some version of filesystem lib does not copy if dest existing)
					fs::remove(To,ec);
					if (ec) {
						return false;
					}
				}
			}
			fs::copy_file(From,To,ec);
		}
		else {
			fs::copy_file(From,To,options,ec);
		}

        // Set permission
        if (!ec) {
            //std::cout << "Restore permissions " << GetPermissionsString(FilePerms) << std::endl;
            fs::permissions(To,FilePerms,ec);
        }

        if (ec) {
            //std::cout << "Copy " << From.string() << " -> " << To.string() << " ERROR: " << ec.message() << std::endl;
            DEBUG_PRINT("DString::Copy_File() " << From.string() << " -> " << To.string() << " ERROR: " << ec.message());
            return false;
        }

        return true;
	}

	/**
	 * @brief Copy a file using posix api.
	 * @param SourceFile			->	Source filename.
	 * @param DestFile				->	Dest filename.
	 * @param OverwriteExisting		->	TODO
	 * @param MemberCallback		->	Class member function to callback each buffered copy loop.
	 * @param MemberCallbackClass	->	Pointer to Class containing callback funcion.
	 * @param BufferSize			->	Size of buffer used to copy, if value is 0, file lenght is used (only one callback will be performed).
	 * @return true on succes, otherwise false.
     * TODO: Copy permissions
	 */
	#include <sys/stat.h>
	bool Copy_File_Posix(const char* SourceFile, const char* DestFile, bool OverwriteExisting, DCallback Callback, size_t BufferSize) {
		if (!OverwriteExisting) {
			struct stat buffer;
			if (stat(SourceFile, &buffer) == 0) {
				// exists
				return false;
			}
		}

		int in=open(SourceFile, O_RDONLY | O_BINARY);
		if (in < 0) {
			//std::cout << "Can't open input file: " << inFile << std::endl;
			return false;
		}

		int out=open(DestFile, O_CREAT | O_WRONLY | O_BINARY, 0666);
		if (out < 0) {
			//std::cout << "Can't open output file: " << outFile << std::endl;
			return false;
		}

		size_t inFileSize=lseek(in, 0, SEEK_END);
		lseek(in, 0, SEEK_SET);

		if (BufferSize == 0) {
			// Use file size
			BufferSize=inFileSize;
		}
		else if (BufferSize > inFileSize) {
			BufferSize=inFileSize;
		}

		std::vector<char> inBuffer(BufferSize);

		for (size_t bytesLeft=inFileSize,chunk=inBuffer.size(); bytesLeft>0; bytesLeft-=chunk)	{
			if (bytesLeft < chunk) {
				// Last shot
				chunk=bytesLeft;
			}

			read(in,&inBuffer[0],chunk);
			write(out,&inBuffer[0],chunk);
			if (Callback) Callback(CALLBACK_DEC_BYTES,bytesLeft);
		}

		close(out);
		close(in);

		return true;
	}

	//! Copy a directory recoursively.
	/**
	* @param SourceDir		->	Directory to copy.
	* @param DestDir		->	Destination directory.
	* @param FailifExists	->	when false, destination is overwitten.
	* @param Callback		->	Callback function to call on copy operation starts. Callback pass COUNT_DIR before directory copy and COUNT_FILE before file copy.
	*
	* @return true on success or false if any arrors occours.
	* TODO: DErrorCode
	**/
	bool CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback) {
		// Verifica esistenza
		if (!Exists(SourceDir)) {
			return false;
		}

		if (!IsDirectory(SourceDir)) {
			return false;
		}

		if (FailIfExists) {
			if (Exists(DestDir)) {
				return false;
			}
		}

		// N.B. !!!! Crea solo la DestDir copiando gli attributi dalla SourceDir !!!!
		err::error_code ec;
		#if __cplusplus > 201402L // C++17
			fs::create_directory(DestDir,SourceDir,ec);
		#else
			fs::copy_directory(SourceDir,DestDir,ec);
		#endif
		if (ec.value() != 0) {
			std::string s=ec.message();
			Callback(CALLBACK_SET_FILES,1);
			return false;
		}

		if (Callback) {
			int nFiles=CountFiles(SourceDir,false,nullptr);
			int nDirs=CountDirs(SourceDir,false,nullptr);

			Callback(CALLBACK_SET_FILES,nFiles);
			Callback(CALLBACK_SET_DIRS,nDirs);
			Callback(CALLBACK_SET_OBJS,nFiles+nDirs);
		}
		// Itera attraverso SourceDir
		for (fs::directory_iterator iterator(SourceDir); iterator != fs::directory_iterator(); ++iterator) {
			if (fs::is_directory(iterator->status())) {
				if (Callback) Callback(CALLBACK_INC_DIR,1);
				// In ricorsione
				// nessuna callback perché viene eseguita solo per gli oggetti presenti nella root
				if (!CopyDir(iterator->path(),DestDir / iterator->path().filename(),FailIfExists,nullptr)) {
					return false;
				}
			}
			else if (is_regular_file(iterator->status())) {
				if (Callback) Callback(CALLBACK_INC_FILE,1);
				#if __cplusplus > 201402L // C++17
					fs::copy_options options=fs::copy_options::overwrite_existing;
				#else
					fs::copy_option options=fs::copy_option::overwrite_if_exists;
				#endif

				if (!fs::copy_file(iterator->path(),DestDir / iterator->path().filename(),options,ec)) {
					return false;
				}
			}
		}

		return true;
	}

	//! Move a directory recoursively (doing copy and delete)
	/**
	* @param SourceDir		->	Directory to move.
	* @param DestDir		->	Destination directory.
	* @param FailifExists	->	when false, destination is overwitten.
	* @param Callback		->	Callback function to call on copy operation starts. Callback pass COUNT_DIR before directory copy and COUNT_FILE before file copy.
	*
	* @return true on success or false if any arrors occours.
	**/
	bool MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback) {
		if (Callback) {
			// If You want callback, need to copy and delete...
			if (!CopyDir(SourceDir,DestDir,FailIfExists,Callback)) {
				return false;
			}

			err::error_code ec;
			fs::remove_all(SourceDir,ec);
			if (ec) {
				return false;
			}
		}
		else {
			#if __cplusplus > 201402L // C++17
				// C++17 fast move
				err::error_code ec;
				fs::rename(SourceDir,DestDir,ec);
				if (ec) {
					return false;
				}
			#else
				#ifdef WIN32
					// windows fast move
					if (SourceDir.string().substr(0,2) == DestDir.string().substr(0,2) && DestDir.string().substr(0,2) != "\\\\") {
						// use windows api for c++ < 17
						if (!MoveFileEx(SourceDir,DestDir,MOVEFILE_REPLACE_EXISTING)) {
							return false;
						}
					}
					else {
						// use c++ copy and delete
						if (!CopyDir(SourceDir,DestDir,FailIfExists,Callback,MemberCallbackClassObj)) {
							return false;
						}

						if (!fs::remove_all(SourceDir,ec))
						return false;
					}
				#else
					if (!CopyDir(SourceDir,DestDir,FailIfExists,Callback)) {
						return false;
					}

					err::error_code ec;
					fs::remove_all(SourceDir,ec);
					if (ec) {
						return false;
					}
				#endif
			#endif
		}
		return true;
	}

	//! Delete a directory recoursively
	/**
	* @param Dir	->	Directory to delete.
	*
	* @return true on success or false if any arrors occours.
	**/
	bool DeleteDir(fs::path Dir) {
		if (!Exists(Dir)) {
			return false;
		}
		err::error_code ec;
		fs::remove_all(Dir,ec);
		if (ec) {
			return false;
		}
		return true;
	}

	//! Cerca tutte le cartelle all'interno di una cartella.
	/**
	* Ricerca tramite una lista per nome e una lista per l'estensione.
	*
	* @param Result				->	Puntatore ad una array di path da popolare con i risultati, se nullptr ritorna solo il conteggio.
	* @param PathToScan			->	Directory di partenza.
	* @param Recoursive			->	Se true va in ricorsione.
	* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nel nome.
	* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa).
	* @param ExtContentList			->	Puntatore ad un array di estensioni da cercare, nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nell'estensione.
	* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa).
	* @param CaseSensitive		->	Se true tiene conto di maiuscole e minuscole.
	* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione.
	*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	* N.B. @ref Result non viene pulita quindi se non è vuota  risultati vengono aggiunti.
	**/
	int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool *StopRequest) {
		err::error_code ec;
		unsigned int Tot=0;
		//DEBUG_PRINT("ListDirs in "+PathToScan.string()+" Recoursive="+std::to_string(Recoursive)+" CaseSensitive="+std::to_string(CaseSensitive)+" NameContentList="+std::to_string(NameContentList->size())+" ExtContentList="+std::to_string(ExtContentList->size()));

		// Verifica esistenza
		bool Ret=Exists(PathToScan);
		if (!Ret) {
			return -1;
		}

		for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
			if (StopRequest) {
				if (*StopRequest) {
					//Log("Stop requested");
					break;
				}
			}

			if (fs::is_directory(iterator->status())) {
				if (Recoursive) {
					// In ricorsione
					//DEBUG_PRINT("Ricorsione su "+iterator->path().string());
					Tot+=ListDirs(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll,StopRequest);
				}

				fs::path CurrPath=iterator->path();
				bool Found=false;

				if (NameContentList != nullptr) {
					if (NameContentList->size() > 0) {
						// Ricerca patterns all'interno del nome
						std::string CurrName=CurrPath.stem().string();
						if (FindAll) {
							// Cerca tutte le ricorrenze
							// Found rimane true solo se vengono trovati tutti i pattern
							Found=true;
							for (std::string NameContent : *NameContentList) {
								DTools::DString::RemoveChars(NameContent,"*");
								DTools::DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrName != NameContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrName,NameContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (CurrName.find(NameContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrName).find(DTools::DString::ToUpperCopy(NameContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							// Cerca almeno una ricorrenza
							// Found diventa true al primo match
							Found=false;
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrName == NameContent) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::CmpNoCase(CurrName,NameContent)) {
											Found=true;
											break;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (NameContent.find(CurrName) != std::string::npos) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrName).find(DTools::DString::ToUpperCopy(NameContent)) != std::string::npos) {
											Found=true;
											break;
										}
									}
								}
							}
						}
					}
					else {
						// Lista vuota
						Found=true;
					}
				}
				else {
					// Lista null
					Found=true;
				}

				// Se la ricerca nel nome non ha dato risultati non è neccessario proseguire
				if (!Found) {
					continue;
				}

				if (ExtContentList != nullptr) {
					if (ExtContentList->size() > 0) {
						// Cerca il pattern nell'estensione
						std::string CurrExt=CurrPath.extension().string();
						if (FindAll) {
							// Cerca tutte le ricorrenze
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrExt != ExtContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrExt,ExtContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (CurrExt.find(ExtContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrExt).find(DTools::DString::ToUpperCopy(ExtContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							// Cerca almeno una ricorrenza
							// Found diventa true al primo match
							Found=false;
							DTools::DString::RemoveChars(CurrExt,".");
							for (std::string ExtContent : *ExtContentList) {
								DTools::DString::RemoveChars(ExtContent,"*");
								DTools::DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrExt == ExtContent) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::CmpNoCase(CurrExt,ExtContent)) {
											Found=true;
											break;
										}
									}
								}
								else {
									// Ricerca all'interno dell'estensione
									if (CaseSensitive) {
										if (CurrExt.find(ExtContent) != std::string::npos) {
											//DEBUG_PRINT("Found Case: "+CurrPath.string()+" ext="+CurrExt+" in "+ExtContent);
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrExt).find(DTools::DString::ToUpperCopy(ExtContent)) != std::string::npos) {
											//DEBUG_PRINT("Found No Case: "+CurrPath.string()+" ext="+CurrExt+" in "+ExtContent);
											Found=true;
											break;
										}
									}
								}
							}
						}
					}
					else {
						// Lista vuota
						Found=true;
					}
				}
				else {
					// Lista null
					Found=true;
				}

				if (!Found) {
					continue;
				}

				// Trovato
				Tot++;
				if (Result != nullptr) {
					Result->push_back(CurrPath);
				}
			}
		}
		//DEBUG_PRINT("Ritorno da "+PathToScan.string()+" : "+std::to_string(Tot));
		return(Tot);
	}

	//! Cerca tutte le cartelle all'interno di una cartella.
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione.
	*
	* @param Result			->	Puntatore ad una array di path da popolare con i risultati, se nullptr ritorna solo il conteggio.
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto.
	* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome.
	* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto.
	* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione.
	* @param CaseSensitive	->	Se true tiene conto di maiuscole e minuscole.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti.
	**/
	int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool *StopRequest) {
		std::vector<std::string> NameContentList;
		std::vector<std::string> ExtContentList;
		if (!NameContent.empty()) {
			NameContentList.push_back(NameContent);
		}
		if (!ExtContent.empty()) {
			ExtContentList.push_back(ExtContent);
		}
		return(ListDirs(Result,PathToScan,Recoursive,&NameContentList,NameWholeWord,&ExtContentList,ExtWholeWord,CaseSensitive,false,StopRequest));
	}

	//! Conta le cartelle all'interno di una cartella
	/**
	* @param PathToScan			->	Directory di partenza.
	* @param Recoursive			->	Se true va in ricorsione.
	* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), nullptr o vuoto equivale a tutto.
	* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome.
	* @param ExtContentList		->	Puntatore ad un array di estensioni da cercare, nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nell'estensione.
	* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione.
	* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione.
	*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool *StopRequest) {
		return(ListDirs(nullptr,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll,StopRequest));
	}

	//! Conta le cartelle all'interno di una cartella.
	/**
	* @param PathToScan		->	Directory di partenza.
	* @param NameConent		->	Stringa di ricerca per il nome, vuota equilave a tutto.
	* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param StopRequest	->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool *StopRequest) {
		return(ListDirs(nullptr,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensitive,StopRequest));
	}

	//! Conta tutte le cartelle all'interno di una cartella.
	/**
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param StopRequest	->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive, bool *StopRequest) {
		return(ListDirs(nullptr,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false,StopRequest));
	}

	//! Cerca tutti i files all'interno di una cartella.
	/**
	* Ricerca tramite una lista per nome e una lista per l'estensione.
	*
	* @param Result				->	Puntatore ad una array di path da popolare con i risultati, se nullptr ritorna solo il conteggio.
	* @param PathToScan			->	Directory di partenza.
	* @param Recoursive			->	Se true va in ricorsione.
	* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nel nome.
	* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa).
	* @param ExtList			->	Puntatore ad un array di estensioni da cercare, nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nell'estensione.
	* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa).
	* @param CaseSensitive		->	Se true tiene conto di maiuscole e minuscole.
	* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione.
	*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files trovati, -1 in caso di errore.
	* NameContentList e ExtConentList sono sempre legati da una corrispondenza AND quindi: se entrambi hanno contenuto, entrambi devono essere trovati
	* N.B. @ref Result non viene pulita quindi se non è vuota i risultati vengono aggiunti.
	**/
	int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool *StopRequest) {
		err::error_code ec;
		unsigned int Tot=0;
		//DEBUG_PRINT("ListFiles in "+PathToScan.string()+" Recoursive="+std::to_string(Recoursive)+" CaseSensitive="+std::to_string(CaseSensitive));

		// Verifica esistenza

		if (!Exists(PathToScan)) {
			return -1;
		}

		for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
			if (StopRequest) {
				if (*StopRequest) {
					//Log("Stop requested");
					break;
				}
			}
			if (fs::is_directory(iterator->status())) {
				if (Recoursive) {
					// In ricorsione
					//DEBUG_PRINT("Ricorsione su "+iterator->path().string());
					Tot+=ListFiles(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll,StopRequest);
				}
			}
			else {
				fs::path CurrPath=iterator->path();
				bool Found=false;

				if (NameContentList != nullptr) {
					if (NameContentList->size() > 0) {
						// Ricerca patterns all'interno del nome
						std::string CurrName=CurrPath.stem().string();
						if (FindAll) {
							// Cerca tutte le ricorrenze
							// Found rimane true solo se vengono trovati tutti i pattern
							Found=true;
							for (std::string NameContent : *NameContentList) {
								DTools::DString::RemoveChars(NameContent,"*");
								DTools::DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrName != NameContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrName,NameContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (CurrName.find(NameContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrName).find(DTools::DString::ToUpperCopy(NameContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							// Cerca almeno una ricorrenza
							// Found diventa true al primo match
							Found=false;
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrName == NameContent) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::CmpNoCase(CurrName,NameContent)) {
											Found=true;
											break;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (NameContent.find(CurrName) != std::string::npos) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(NameContent).find(DTools::DString::ToUpperCopy(CurrName)) != std::string::npos) {
											Found=true;
											break;
										}
									}
								}
							}
						}
					}
					else {
						// Lista vuota
						Found=true;
					}
				}
				else {
					// Lista null
					Found=true;
				}

				// Se la ricerca nel nome non ha dato risultati non è neccessario proseguire
				if (!Found) {
					continue;
				}

				//DEBUG_PRINT(CurrPath.stem().string());

				if (ExtContentList != nullptr) {
					if (ExtContentList->size() > 0) {
						// Cerca il pattern nell'estensione
						std::string CurrExt=GetExt(CurrPath);
						if (FindAll) {
							// Cerca tutte le ricorrenze
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrExt != ExtContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrExt,ExtContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (CurrExt.find(ExtContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrExt).find(DTools::DString::ToUpperCopy(ExtContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							// Cerca almeno una ricorrenza
							// Found diventa true al primo match
							Found=false;
							for (std::string ExtContent : *ExtContentList) {
								DTools::DString::RemoveChars(ExtContent,"*");
								DTools::DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrExt == ExtContent) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::CmpNoCase(CurrExt,ExtContent)) {
											Found=true;
											break;
										}
									}
								}
								else {
									// Ricerca contenuto
									if (CaseSensitive) {
										if (ExtContent.find(CurrExt) != std::string::npos) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrExt).find(DTools::DString::ToUpperCopy(ExtContent)) != std::string::npos) {
											Found=true;
											break;
										}
									}
								}
							}
						}
					}
					else {
						// Lista vuota
						Found=true;
					}
				}
				else {
					// Lista null
					Found=true;
				}

				if (!Found) {
					continue;
				}

				// Trovato
				Tot++;
				if (Result != nullptr) {
					Result->push_back(CurrPath);
				}
			}
		}
		//DEBUG_PRINT("Ritorno da "+PathToScan.string()+" : "+std::to_string(Tot));
		return(Tot);
	}

	//! Cerca tutti i files all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione.
	*
	* @param Result			->	Puntatore ad una array di path da popolare con i risultati, se nullptr ritorna solo il conteggio.
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto.
	* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome.
	* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto.
	* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files trovati, -1 in caso di errore.
	* N.B. @ref Result non viene pulita quindi se non è vuota i risultati vengono aggiunti.
	**/
	int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool *StopRequest) {
		std::vector<std::string> NameContentList;
		std::vector<std::string> ExtContentList;
		if (!NameContent.empty()) {
			NameContentList.push_back(NameContent);
		}
		if (!ExtContent.empty()) {
			ExtContentList.push_back(ExtContent);
		}
		return(ListFiles(Result,PathToScan,Recoursive,&NameContentList,NameWholeWord,&ExtContentList,ExtWholeWord,CaseSensitive,false,StopRequest));
	}

	//! Conta i files all'interno di una cartella
	/**
	* Ricerca tramite una lista per nome e una lista per l'estensione.
	*
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive			->	Se true va in ricorsione.
	* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nel nome.
	* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa).
	* @param ExtList			->	Puntatore ad un array di estensioni da cercare, nullptr o vuoto equivale a tutto.
	*								N.B. Tutte le stringhe devo essere contenute nell'estensione.
	* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa).
	* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione.
	*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione.
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files trovati, -1 in caso di errore.
	**/
	int CountFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool *StopRequest) {
		return(ListFiles(nullptr,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll,StopRequest));
	}

	//! Conta i files all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione
	*
	* @param PathToScan	->	Directory di partenza
	* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig)
	* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto
	* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome
	* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto
	* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione
	* @param CaseSensitive	->	Se true tiene conto di maiuscole e minuscole
	*
	* @return il numero di files trovati, -1 in caso di errore
	**/
	int CountFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensistive, bool *StopRequest) {
		return(ListFiles(nullptr,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensistive,StopRequest));
	}

	//! Conta tutti i files all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione.
	*
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param StopRequest	->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files trovati, -1 in caso di errore.
	**/
	int CountFiles(fs::path PathToScan, bool Recoursive, bool *StopRequest) {
		return(ListFiles(nullptr,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false,StopRequest));
	}

	//! Elimina files all'interno di una cartella
	/**
	* Ricerca tramite una lista per nome e una lista per l'estensione
	*
	* @param PathToScan			->	Directory di partenza
	* @param Recoursive			->	Se true va in ricorsione (solo se non è un ordine Prodig)
	* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), nullptr o vuoto equivale a tutto
	*								N.B. Tutte le stringhe devo essere contenute nel nome
	* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa)
	* @param ExtList			->	Puntatore ad un array di estensioni da cercare, nullptr o vuoto equivale a tutto
	*								N.B. Tutte le stringhe devo essere contenute nell'estensione
	* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa)
	* @param CaseSensitive		->	Se true tiene conto di maiuscole e minuscole
	* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione
	*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione
	* @param StopRequest		->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files eliminat, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool *StopRequest) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(&Result,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll,StopRequest);
		if (nFiles == -1) {
			return(-1);
		}

		int nDeleted=0;
		err::error_code ec;
		for (fs::path& Filename : Result) {
			fs::remove(Filename,ec);
			if (ec.value() == 0) {
				nDeleted++;
			}
		}

		return(nDeleted);
	}

	//! Elimina files (e solo i files) all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione
	*
	* @param PathToScan		->	Directory di partenza
	* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig).
	* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto.
	* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome.
	* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto.
	* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione.
	* @param CaseSensitive	->	Se true tiene conto di maiuscole e minuscole.
	* @param StopRequest	->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	*
	* @return il numero di files eliminati, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool *StopRequest) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(&Result,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensitive,StopRequest);
		if (nFiles == -1) {
			return(-1);
		}
		DEBUG_PRINT("Found " << Result.size() << " files to remove");

		int nDeleted=0;
		err::error_code ec;
		for (fs::path& Filename : Result) {
			fs::remove(Filename,ec);
			if (!ec) {
				nDeleted++;
			}
		}

		return(nDeleted);
	}

	//! Elimina tutti i files (e solo i files) all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione
	*
	* @param PathToScan		->	Directory di partenza.
	* @param Recoursive		->	Se true va in ricorsione.
	* @param StopRequest	->	Puntatore alla variabile che contiene il flag di richiesta di stop, quando diventa true la ricerca si interrompe.
	* N.B. In ricorsione vengono eliminati solo i files, non le cartelle.
	*
	* @return il numero di files eliminati, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive, bool *StopRequest) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(&Result,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false,StopRequest);
		if (nFiles == -1) {
			return(-1);
		}
		DEBUG_PRINT("Found " << Result.size() << " files to remove");

		int nDeleted=0;
		err::error_code ec;
		for (fs::path& Filename : Result) {
			fs::remove(Filename,ec);
			if (!ec) {
				nDeleted++;
			}
		}

		return(nDeleted);
	}

	/**
	 * @brief Wrapper for filesystem::exists.
	 * This is a workaround, from gcc 8 and above filesystem::exists() return false on some linux samba share that exists instead.
	 * @param Path	->	Path to check.
	 * @return true if the path exists, otherwise false.
	 * N.B. Exceptions will not trown.
	 */
	bool Exists(fs::path Path) {
		err::error_code ec;
		// Try first normal way
		fs::file_status Status=fs::status(Path,ec);
		bool Ret=fs::exists(Status);
		if (Ret) {
			return true;
		}

		// If Ret is false try also the alternative way
		Ret=DTools::DPath::CanAccess(Path,DTools::DPath::ACCESS_READ);
		return(Ret);
	}

	/**
	 * @brief Checks to see if a directory/file exists (posix version).
	 * @param path	->	Directory name string.
	 * @return true if the path exists, otherwise false.
	 */
	bool Exists_Posix(const char* const path) {
		struct stat info;

		int statRC=stat(path,&info);
		if(statRC != 0 ) {
			return false;
		}

		return ((info.st_mode & S_IFDIR) || (info.st_mode & S_IFREG)) ? true : false;
	}

	/**
	 * @brief Wrapper for filesystem::is_directory (exception trowing version).
	 * This is a workaround, from gcc 8 and above filesystem::is_directory() return false on some linux samba share that are directory.
	 * @param Path	->	Path to check.
	 * @return true if the path exists, otherwise false.
	 * N.B. Exceptions will be trown is error occours.
	 */
	bool IsDirectory(fs::path Path) {
		err::error_code ec;
		fs::file_status Status=fs::status(Path,ec);
		if (fs::is_directory(Status)) {
			return true;
		}

		// Aternative way
		if (fs::exists(Status)) {
			if (!fs::is_regular_file(Status)) {
				return true;
			}
		}

		return false;
	}

	/*
	// Elimina i più vecchi di 48 hr
	ErrorList.erase(
		std::remove_if(
			ErrorList.begin(),
			ErrorList.end(),
			[](boost::filesystem::path Ext) {
				return(!DTools::DPath::IsOlderThanHrs(Ext,48));
			}
		),
		ErrorList.end()
	);
	//v.erase(std::remove_if(v.begin(), v.end(), IsOdd), v.end());
	*/
} // DPath
} // DTools
