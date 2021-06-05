#include "libdpp/DPath.h"
#include "libdpp/DStringGrid.h"
#include "libdpp/DString.h"
#include "libdpp/DCsv.h"
#include <iostream>

#include <fcntl.h>
#ifndef O_BINARY
    #define O_BINARY 0
#endif

#ifdef _WIN32
    #include <windows.h>    //GetModuleFileNameW
#else
    #include <limits.h>
    #include <unistd.h>     //readlink
#endif

// Debug macro if DEBUG is defined, DEBUG_PRINT(Msg) macro prints Msg on stdout, otherwise do nothing (I use it to debug issues).
//#define DEBUG
#ifdef DEBUG
	#include<iostream>
	#define DEBUG_PRINT(Msg) std::cout << Msg << std::endl;
#else
	#define DEBUG_PRINT(Msg)
#endif

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

    /**
     * @brief Change the extension of a file/dir
     * @param Path      ->  Filename or directory.
     * @param NewExt    ->  New extension (with or without dot).
     * @param Execute   ->  If false only string new name is returned, if true also file/dir on disk is renamed.
     * @param ec        ->  a pointer to error_code variable to fill with result (can be null).
     * @return new file/dir name or empty path if rename on disk fails.
     */
    fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code *ec) {
        fs::path NewPath=Path.parent_path() / (Path.stem().string() + "." + DString::LTrim(NewExt,"."));

		if (Execute) {
			fs::rename(Path,NewPath,*ec);
			if (ec->value() != 0) {
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
	 * @brief Convert a time type to a time_t type (eg. a file_time_type)
	 * @param tp	->	Any time type that can be casted with chrono::syetem_clock
	 * @return a converted time_t.
	 */
	template <typename TP>
	std::time_t to_time_t(TP tp)
	{
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
		return system_clock::to_time_t(sctp);
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
		std::time_t ftimet=to_time_t(fttime);
		// then in time_point
		std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);
		// and make the difference as hours
		std::chrono::hours diff=std::chrono::duration_cast<std::chrono::hours>(now - tptime);

		return(diff.count() > Hrs);
	}

	/**
     * @brief Retrive the last modification time of file/dir.
	 * @param Path	->	file/dir to check.
	 * @return a time_point format time.
	 */
	std::chrono::system_clock::time_point LastWriteTime(const fs::path Path) {
		fs::file_time_type fttime=fs::last_write_time(Path);
		std::time_t ftimet=to_time_t(fttime);

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

    bool CanAccess(fs::path Path) {
#ifdef _WIN32
        /**
        * @param genericAccessRights	->	GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL
        **/
        bool DFiles::CanAccessFolder(AnsiString Path,DWORD genericAccessRights)
        {
            bool bRet = false;
            DWORD length = 0;
            if (!GetFileSecurity(Path.c_str(), OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL, NULL, &length ) && ERROR_INSUFFICIENT_BUFFER == ::GetLastError()) {
                PSECURITY_DESCRIPTOR security = static_cast< PSECURITY_DESCRIPTOR >( ::malloc( length ) );
                if (security && GetFileSecurity(Path.c_str(),OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, security, length, &length )) {
                    HANDLE hToken = NULL;
                    if (OpenProcessToken(GetCurrentProcess(),TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken)) {
                        HANDLE hImpersonatedToken = NULL;
                        if (DuplicateToken( hToken, SecurityImpersonation, &hImpersonatedToken)) {
                            GENERIC_MAPPING mapping = { 0xFFFFFFFF };
                            PRIVILEGE_SET privileges = { 0 };
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
#endif
        // TODO
        return true;
    }

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
	err::error_code Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode) {
		err::error_code ec;
		#if __cplusplus > 201402L // C++17
			fs::copy_options options=OverwriteExisting ? fs::copy_options::overwrite_existing : fs::copy_options::none;
		#else
			fs::copy_option options=OverwriteExisting ? fs::copy_option::overwrite_if_exists : fs::copy_option::none;
		#endif

        // Store file permission
        fs::perms FilePerms=fs::status(From).permissions();

		if (SafeMode) {
			if (OverwriteExisting) {
				if (fs::exists(To)) {
					// Safe mode work-around:
					// delete dest before copy (some version of filesystem lib does not copy if dest existing)
					fs::remove(To,ec);
					if (ec.value() != 0) {
						return(ec);
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
            std::cout << "Restore permissions " << GetPermissionsString(FilePerms) << std::endl;
            fs::permissions(To,FilePerms,ec);
        }
		return(ec);
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
    bool Copy_File_Posix(const char* SourceFile, const char* DestFile, bool OverwriteExisting, DMemberCallback MemberCallback, void *MemberCallbackClass, size_t BufferSize) {
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
			if (MemberCallback) MemberCallback(MemberCallbackClass,DEC_BYTES,reinterpret_cast<void *>(static_cast<intptr_t>(bytesLeft)));
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
	* @return 0 on success or an error_code if any arrors occours.
	**/
	err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback) {

		err::error_code ec;

		// Verifica esistenza
		if (!fs::exists(SourceDir,ec)) {
			return(ec);
		}
		if (!is_directory(SourceDir,ec)) {
			return(ec);
		}

		if (FailIfExists) {
			if (fs::exists(DestDir,ec)) {
				return(ec);
			}
		}

		// N.B. !!!! Crea solo la DestDir copiando gli attributi dalla SourceDir !!!!
		#if __cplusplus > 201402L // C++17
			fs::create_directory(SourceDir,DestDir,ec);
		#else
			fs::copy_directory(SourceDir,DestDir,ec);
		#endif
		if (ec.value() != 0) {
			return ec;
		}

		if (Callback) {
			int nFiles=CountFiles(SourceDir,false);
			int nDirs=CountDirs(SourceDir,false);

			Callback(SET_FILES,DIntToPtr(nFiles));
			Callback(SET_DIRS,DIntToPtr(nDirs));
			Callback(SET_OBJ,DIntToPtr(nFiles+nDirs));
		}
		// Itera attraverso SourceDir
		for (fs::directory_iterator iterator(SourceDir); iterator != fs::directory_iterator(); ++iterator) {
			if (is_directory(iterator->status())) {
				if (Callback) Callback(INC_DIR,nullptr);
				// In ricorsione
				ec=CopyDir(iterator->path(),DestDir / iterator->path().filename(),FailIfExists,nullptr); // nessuna callback perché viene eseguita solo per gli oggetti presenti nella root
				if (ec.value() != 0) {
					return(ec);
				}
			}
			else if (is_regular_file(iterator->status())) {
				if (Callback) Callback(INC_FILE,nullptr);
				#if __cplusplus > 201402L // C++17
					fs::copy_options options=fs::copy_options::overwrite_existing;
				#else
					fs::copy_option options=fs::copy_option::overwrite_if_exists;
				#endif

				fs::copy_file(iterator->path(),DestDir / iterator->path().filename(),options,ec);
				if (ec.value() != 0) {
					return(ec);
				}
			}
		}

		return(ec);
	}

	//! Move a directory recoursively (doing copy and delete)
	/**
	* @param SourceDir		->	Directory to move.
	* @param DestDir		->	Destination directory.
	* @param FailifExists	->	when false, destination is overwitten.
	* @param Callback		->	Callback function to call on copy operation starts. Callback pass COUNT_DIR before directory copy and COUNT_FILE before file copy.
	*
	* @return 0 on success or an error_code if any arrors occours.
	**/
	err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback) {
		if (Callback) {
			// If You want callback, need to copy and delete...
			err::error_code ec=CopyDir(SourceDir,DestDir,FailIfExists,Callback);

			if (ec.value() != 0) {
				return(ec);
			}

			fs::remove_all(SourceDir,ec);
			return(ec);
		}
		else {
			#if __cplusplus > 201402L // C++17
				// C++17 fast move
				err::error_code ec;
				fs::rename(SourceDir,DestDir,ec);
				return(ec);
			#else
				#ifdef WIN32
					// windows fast move
					if (SourceDir.string().substr(0,2) == DestDir.string().substr(0,2) && DestDir.string().substr(0,2) != "\\\\") {
						// use windows api for c++ < 17
						bool ret=MoveFileEx(SourceDir,DestDir,MOVEFILE_REPLACE_EXISTING);
						if (!ret) {
							ec.error_code=1;
						}
					}
					else {
						// use c++ copy and delete
						ec=CopyDir(SourceDir,DestDir,FailIfExists,Callback);
						if (ec.value() != 0) {
							return(ec);
						}

						dfs::remove_all(SourceDir,ec);
						return(ec);
					}
				#else
					err::error_code ec=CopyDir(SourceDir,DestDir,FailIfExists,nullptr);
					if (ec.value() != 0) {
						return(ec);
					}

					fs::remove_all(SourceDir,ec);
					return(ec);
				#endif
			#endif
		}
	}

	//! Delete a directory recoursively
	/**
	* @param Dir	->	Directory to delete.
	*
	* @return 0 on success or an error_code if any arrors occours.
	**/
	err::error_code DeleteDir(fs::path Dir) {
		err::error_code ec;
		bool Ret=fs::exists(Dir,ec);
		if (!Ret ||	ec.value() != 0) {
			return(ec);
		}
		fs::remove_all(Dir,ec);
		return(ec);
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
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	* N.B. @ref Result non viene pulita quindi se non è vuota  risultati vengono aggiunti.
	**/
	int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
		err::error_code ec;
		unsigned int Tot=0;
		DEBUG_PRINT("ListDirs in "+PathToScan.string()+" Recoursive="+std::to_string(Recoursive)+" CaseSensitive="+std::to_string(CaseSensitive)+" NameContentList="+std::to_string(NameContentList->size())+" ExtContentList="+std::to_string(ExtContentList->size()));

		// Verifica esistenza
		bool Ret=fs::exists(PathToScan,ec);
		if (!Ret || ec.value() != 0) {
			return -1;
		}

		for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
			if (is_directory(iterator->status())) {
				if (Recoursive) {
					// In ricorsione
					DEBUG_PRINT("Ricorsione su "+iterator->path().string());
					Tot+=ListDirs(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll);
				}

				fs::path CurrPath=fs::absolute(iterator->path());
				bool Found=false;

				if (NameContentList != nullptr) {
					if (NameContentList->size() > 0) {
						// Ricerca patterns all'interno del nome
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							Found=true;
							for (std::string NameContent : *NameContentList) {
								DTools::DString::RemoveChars(NameContent,"*");
								DTools::DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrPath.stem() != NameContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrPath.stem().string(),NameContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CaseSensitive) {
										if (CurrPath.stem().string().find(NameContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrPath.stem().string()).find(DTools::DString::ToUpperCopy(NameContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							Found=false;
							// Found diventa true al primo match
							std::string CurrName=CurrPath.stem().string();
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
									// Ricerca all'interno dell'estensione
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

				if (ExtContentList != nullptr) {
					if (ExtContentList->size() > 0) {
						// Cerca il pattern nell'estensione
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrPath.stem() != ExtContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrPath.stem().string(),ExtContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CaseSensitive) {
										if (CurrPath.stem().string().find(ExtContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrPath.stem().string()).find(DTools::DString::ToUpperCopy(ExtContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							Found=false;
							// Found diventa true al primo match
							std::string CurrExt=CurrPath.extension().string();
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
										if (ExtContent.find(CurrExt) != std::string::npos) {
											DEBUG_PRINT("Found Case: "+CurrPath.string()+" ext="+CurrExt+" in "+ExtContent);
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(ExtContent).find(DTools::DString::ToUpperCopy(CurrExt)) != std::string::npos) {
											DEBUG_PRINT("Found No Case: "+CurrPath.string()+" ext="+CurrExt+" in "+ExtContent);
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
		DEBUG_PRINT("Ritorno da "+PathToScan.string()+" : "+std::to_string(Tot));
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
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti.
	**/
	int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive) {
		std::vector<std::string> NameContentList;
		std::vector<std::string> ExtContentList;
		if (!NameContent.empty()) {
			NameContentList.push_back(NameContent);
		}
		if (!ExtContent.empty()) {
			ExtContentList.push_back(ExtContent);
		}
		return(ListDirs(Result,PathToScan,Recoursive,&NameContentList,NameWholeWord,&ExtContentList,ExtWholeWord,CaseSensitive,false));
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
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
		return(ListDirs(nullptr,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll));
	}

	//! Conta le cartelle all'interno di una cartella.
	/**
	* @param PathToScan	->	Directory di partenza.
	* @param NameConent	->	Stringa di ricerca per il nome, vuota equilave a tutto.
	* @param ExtContent	->	Stringa di ricerca per l'estensione, vuota equilave a tutto.
	* @param Recoursive	->	Se true va in ricorsione.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive) {
		return(ListDirs(nullptr,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensitive));
	}

	//! Conta tutte le cartelle all'interno di una cartella.
	/**
	* @param PathToScan	->	Directory di partenza.
	* @param Recoursive	->	Se true va in ricorsione.
	*
	* @return il numero di directory trovate, -1 in caso di errore.
	**/
	int CountDirs(fs::path PathToScan, bool Recoursive) {
		return(ListDirs(nullptr,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false));
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
	*
	* @return il numero di files trovati, -1 in caso di errore.
	* N.B. @ref Result non viene pulita quindi se non è vuota  risultati vengono aggiunti.
	**/
	int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
		err::error_code ec;
		unsigned int Tot=0;
		//DEBUG_PRINT("ListFiles in "+PathToScan.string()+" Recoursive="+std::to_string(Recoursive)+" CaseSensitive="+std::to_string(CaseSensitive));

		// Verifica esistenza
		bool Ret=fs::exists(PathToScan,ec);
		if (!Ret || ec.value() != 0) {
			return -1;
		}

		for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
			if (is_directory(iterator->status())) {
				if (Recoursive) {
					// In ricorsione
					//DEBUG_PRINT("Ricorsione su "+iterator->path().string());
					Tot+=ListFiles(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll);
				}
			}
			else {
				fs::path CurrPath=fs::absolute(iterator->path());
				bool Found=false;

				if (NameContentList != nullptr) {
					if (NameContentList->size() > 0) {
						// Ricerca patterns all'interno del nome
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							Found=true;
							for (std::string NameContent : *NameContentList) {
								DTools::DString::RemoveChars(NameContent,"*");
								DTools::DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrPath.stem() != NameContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrPath.stem().string(),NameContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CaseSensitive) {
										if (CurrPath.stem().string().find(NameContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrPath.stem().string()).find(DTools::DString::ToUpperCopy(NameContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							Found=false;
							// Found diventa true al primo match
							std::string CurrName=CurrPath.stem().string();
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
									// Ricerca all'interno dell'estensione
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

				if (ExtContentList != nullptr) {
					if (ExtContentList->size() > 0) {
						// Cerca il pattern nell'estensione
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CaseSensitive) {
										if (CurrPath.stem() != ExtContent) {
											Found=false;
										}
									}
									else {
										if (!DTools::DString::CmpNoCase(CurrPath.stem().string(),ExtContent)) {
											Found=false;
										}
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CaseSensitive) {
										if (CurrPath.stem().string().find(ExtContent) == std::string::npos) {
											Found=false;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(CurrPath.stem().string()).find(DTools::DString::ToUpperCopy(ExtContent)) == std::string::npos) {
											Found=false;
										}
									}
								}
							}
						}
						else {
							Found=false;
							// Found diventa true al primo match
							std::string CurrExt=CurrPath.extension().string();
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
										if (ExtContent.find(CurrExt) != std::string::npos) {
											Found=true;
											break;
										}
									}
									else {
										if (DTools::DString::ToUpperCopy(ExtContent).find(DTools::DString::ToUpperCopy(CurrExt)) != std::string::npos) {
											//DEBUG_PRINT("Found : "+CurrPath.string()+" ext="+CurrExt+" in "+ExtContent);
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
	*
	* @return il numero di files trovati, -1 in caso di errore.
	* N.B. @ref Result non viene pulita qindi se non è vuota i risultati vengono aggiunti.
	**/
	int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive) {
		std::vector<std::string> NameContentList;
		std::vector<std::string> ExtContentList;
		if (!NameContent.empty()) {
			NameContentList.push_back(NameContent);
		}
		if (!ExtContent.empty()) {
			ExtContentList.push_back(ExtContent);
		}
		return(ListFiles(Result,PathToScan,Recoursive,&NameContentList,NameWholeWord,&ExtContentList,ExtWholeWord,CaseSensitive,false));
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
	*
	* @return il numero di files trovati, -1 in caso di errore.
	**/
	int CountFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
		return(ListFiles(nullptr,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll));
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
	int CountFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensistive) {
		return(ListFiles(nullptr,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensistive));
	}

	//! Conta tutti i files all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione.
	*
	* @param PathToScan	->	Directory di partenza.
	* @param Recoursive	->	Se true va in ricorsione.
	*
	* @return il numero di files trovati, -1 in caso di errore.
	**/
	int CountFiles(fs::path PathToScan, bool Recoursive) {
		return(ListFiles(nullptr,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false));
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
	*
	* @return il numero di files eliminat, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(&Result,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll);
		if (nFiles == -1) {
			return(-1);
		}

		int nDeleted=0;
		err::error_code ec;
		for (fs::path Filename : Result) {
			fs::remove(Filename,ec);
			if (ec.value() == 0) {
				nDeleted++;
			}
		}

		return(nDeleted);
	}

	//! Elimina files all'interno di una cartella
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
	* @return il numero di files eliminati, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(&Result,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensitive);
		if (nFiles == -1) {
			return(-1);
		}

		int nDeleted=0;
		err::error_code ec;
		for (fs::path Filename : Result) {
			fs::remove(Filename,ec);
			if (ec.value() == 0) {
				nDeleted++;
			}
		}

		return(nDeleted);
	}

	//! Elimina tutti i files all'interno di una cartella
	/**
	* Ricerca tramite una stringa per nome e una stringa per l'estensione
	*
	* @param PathToScan	->	Directory di partenza.
	* @param Recoursive	->	Se true va in ricorsione.
	* N.B. In ricorsione vengono eliminati solo i files, non le cartelle.
	*
	* @return il numero di files eliminati, -1 in caso di errore
	**/
	int DeleteFiles(fs::path PathToScan, bool Recoursive) {
		std::vector<fs::path> Result;
		int nFiles=ListFiles(nullptr,PathToScan,Recoursive,nullptr,false,nullptr,false,false,false);
		if (nFiles == -1) {
			return(-1);
		}

		int nDeleted=0;
		err::error_code ec;
		for (fs::path Filename : Result) {
			fs::remove(Filename,ec);
			if (ec.value() == 0) {
				nDeleted++;
			}
		}

		return(nDeleted);
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
