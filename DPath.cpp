#include "DPath.h"
#include "DStringGrid.h"
#include "DString.h"
#include "DCsv.h"

namespace DTools
{
	namespace DPath {

		fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code *ec) {
			fs::path NewPath=Path.parent_path() / (Path.stem().string() + "." + DString::LTrim(NewExt,"."));

			if (Execute) {
				fs::rename(Path,NewPath,*ec);
				if (ec != 0) {
					return (fs::path());
				}
			}
			return (NewPath);
		}

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

		template <typename TP>
		std::time_t to_time_t(TP tp)
		{
			using namespace std::chrono;
			auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
					  + system_clock::now());
			return system_clock::to_time_t(sctp);
		}

		bool IsOlderThanHrs(const fs::path& Path, const int Hrs) {
			// get now as time_point
			std::chrono::system_clock::time_point now=std::chrono::system_clock::now();
			// get file_time of file
			/*
			#if __cplusplus > 201402L // C++17
				std::filesystem::file_time_type fttime=std::filesystem::last_write_time(Path);
				// transform it to time_t
				std::time_t ftimet=decltype(fttime)::clock::to_time_t(fttime);
			#else
				std::time_t ftimet=boost::filesystem::last_write_time(Path);
			#endif
			*/
			fs::file_time_type fttime=fs::last_write_time(Path);
			std::time_t ftimet=to_time_t(fttime);

			// then in time_point
			std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);
			// and make the difference as hours
			std::chrono::hours diff=std::chrono::duration_cast<std::chrono::hours>(now - tptime);

			return(diff.count() > Hrs);
		}

		std::chrono::system_clock::time_point LastWriteTime(const fs::path Path) {
			/*
            #if __cplusplus > 201402L // C++17
				std::filesystem::file_time_type fttime=std::filesystem::last_write_time(Path);
				// transform it to time_t
				std::time_t ftimet=decltype(fttime)::clock::to_time_t(fttime);
			#else
				std::time_t ftimet=boost::filesystem::last_write_time(Path);
			#endif
			*/
			fs::file_time_type fttime=fs::last_write_time(Path);
			std::time_t ftimet=to_time_t(fttime);

			// then in time_point
			std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);

			return(tptime);
		}

		//! Copy a file
		/**
		* @param From	->	Source File.
		* @param To		->	Destination file.
        *
		* @return 0 on success or an error_code if any arrors occours.
		**/
		err::error_code Copy_File(const std::string &From, const std::string &To, bool OverwriteExisting) {
			err::error_code ec;
			#if __cplusplus > 201402L // C++17
				fs::copy_options options=OverwriteExisting ? fs::copy_options::overwrite_existing : fs::copy_options::none;
			#else
				fs::copy_option options=OverwriteExisting ? fs::copy_option::overwrite_if_exists : fs::copy_option::none;
			#endif

			fs::copy_file(From,To,options,ec);
            return(ec);
		}

		//! Copy a directory recoursively
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
					if (Callback) Callback(INC_DIR,NULL);
					// In ricorsione
					ec=CopyDir(iterator->path(),DestDir / iterator->path().filename(),FailIfExists,NULL); // nessuna callback perché viene eseguita solo per gli oggetti presenti nella root
					if (ec.value() != 0) {
						return(ec);
					}
				}
				else if (is_regular_file(iterator->status())) {
					if (Callback) Callback(INC_FILE,NULL);
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
			// TODO: fastmove
			err::error_code ec=CopyDir(SourceDir,DestDir,FailIfExists,Callback);

			if (ec.value() != 0) {
				return(ec);
			}

			fs::remove_all(SourceDir,ec);
			return(ec);
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

		//! Cerca tutte le cartelle all'interno di una cartella
		/**
		* Ricerca tramite una lista per nome e una lista per l'estensione
		*
		* @param Result				->	Puntatore ad una array di path da popolare con i risultati, se NULL ritorna solo il conteggio
		* @param PathToScan			->	Directory di partenza
		* @param Recoursive			->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nel nome
		* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa)
		* @param ExtContentList			->	Puntatore ad un array di estensioni da cercare, NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nell'estensione
		* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa)
		* @param CaseSensitive		->	Se true tiene conto di maiuscole e minuscole
		* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione
		*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione
		*
		* @return il numero di directory trovate, -1 in caso di errore
		* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti
		**/
		int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
			unsigned int Tot=0;
			// Verifica esistenza
			try {
				if (!fs::exists(PathToScan)) {
					return -1;
				}
				if (!is_directory(PathToScan)) {
					return -1;
				}
			} catch (fs::filesystem_error& e) {
				return(-1);
			}

			if (!CaseSensitive) {
				// Campi di ricerca in maiuscolo
				if (ExtContentList != NULL) {
					for (size_t ixE=0;ixE<ExtContentList->size();ixE++) {
						DString::ToUpper(ExtContentList->at(ixE));
					}
				}
				if (NameContentList != NULL) {
					for (size_t ixN=0;ixN<NameContentList->size();ixN++) {
						DString::ToUpper(NameContentList->at(ixN));
					}
				}
			}

			for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
				if (is_directory(iterator->status())) {
					if (Recoursive) {
						// In ricorsione
						Tot+=ListDirs(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,CaseSensitive,FindAll);
					}

					// Directory attuale
					fs::path CurrPath;
					if (!CaseSensitive) {
						// Ingnora case
						CurrPath=DString::ToUpperCopy(iterator->path().string());
					}
					else {
						CurrPath=iterator->path();
					}

					// Cerca il pattern nel nome
					bool Found=true;
					if (NameContentList != NULL) {
						// Ricerca patterns all'interno del nome
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CurrPath.stem() != NameContent) {
										Found=false;
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CurrPath.stem().string().find(NameContent) == std::string::npos) {
										Found=false;
									}
								}
							}
						}
						else {
							// Found diventa true al primo match
							Found=false;
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CurrPath.stem() == NameContent) {
										Found=true;
										break;
									}
								}
								else {
									// Ricerca al''interno dell'estensione
									if (CurrPath.stem().string().find(NameContent) != std::string::npos) {
										Found=true;
										break;
									}
								}
							}
						}
					}

					if (!Found) {
						continue;
					}

					// Cerca il pattern nell'estensione
					if (ExtContentList != NULL) {
						std::string CurrExt=CurrPath.extension().string();
						DString::RemoveChars(CurrExt,".");
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CurrExt != ExtContent) {
										Found=false;
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CurrExt.find(ExtContent) == std::string::npos) {
										Found=false;
									}
								}
							}
						}
						else {
							// Found diventa true al primo match
							Found=false;
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CurrExt == ExtContent) {
										Found=true;
										break;
									}
								}
								else {
									// Ricerca al''interno dell'estensione
									if (CurrExt.find(ExtContent) != std::string::npos) {
										Found=true;
										break;
									}
								}
							}
						}
					}

					if (Found) {
						Tot++;
						if (Result != NULL) {
							Result->push_back(iterator->path());
						}
					}
				}
			}
			return(Tot);
		}

		// Cerca tutte le cartelle all'interno di una cartella
		/**
		* Ricerca tramite una stringa per nome e una stringa per l'estensione
		*
		* @param Result			->	Puntatore ad una array di path da popolare con i risultati, se NULL ritorna solo il conteggio
		* @param PathToScan		->	Directory di partenza
		* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto
		* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome
		* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto
		* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione
		* @param CaseSensitive	->	Se true tiene conto di maiuscole e minuscole
		*
		* @return il numero di directory trovate, -1 in caso di errore
		* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti
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

		// Conta le cartelle all'interno di una cartella
		/**
		* @param PathToScan		->	Directory di partenza
		* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), NULL o vuoto equivale a tutto
		* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome
		* @param ExtContentList	->	Puntatore ad un array di estensioni da cercare, NULL o vuoto equivale a tutto
		*							N.B. Tutte le stringhe devo essere contenute nell'estensione
		* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione
		* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione
		*							Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione
		*
		* @return il numero di directory trovate, -1 in caso di errore
		**/
		int CountDirs(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool FindAll) {
			return(ListDirs(NULL,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,Recoursive,FindAll));
		}

		// Conta le cartelle all'interno di una cartella
		/**
		* @param PathToScan	->	Directory di partenza
		* @param NameConent	->	Stringa di ricerca per il nome, vuota equilave a tutto
		* @param ExtContent	->	Stringa di ricerca per l'estensione, vuota equilave a tutto
		* @param Recoursive	->	Se true va in ricorsione
		*
		* @return il numero di directory trovate, -1 in caso di errore
		**/
		int CountDirs(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive) {
			return(ListDirs(NULL,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord,CaseSensitive));
		}

		// Conta tutte le cartelle all'interno di una cartella
		/**
		* @param PathToScan	->	Directory di partenza
		* @param Recoursive	->	Se true va in ricorsione
		*
		* @return il numero di directory trovate, -1 in caso di errore
		**/
		int CountDirs(fs::path PathToScan, bool Recoursive) {
			return(ListDirs(NULL,PathToScan,Recoursive,NULL,false,NULL,false,true));
		}

		// Cerca tutti i files all'interno di una cartella
		/**
		* Ricerca tramite una lista per nome e una lista per l'estensione
		*
		* @param Result				->	Puntatore ad una array di path da popolare con i risultati, se NULL ritorna solo il conteggio
		* @param PathToScan			->	Directory di partenza
		* @param Recoursive			->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nel nome
		* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa)
		* @param ExtList			->	Puntatore ad un array di estensioni da cercare, NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nell'estensione
		* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa)
		* @param CaseSensitive		->	Se true tiene conto di maiuscole e minuscole
		* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione
		*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione
		*
		* @return il numero di files trovati, -1 in caso di errore
		* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti
		**/
		int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll) {
			unsigned int Tot=0;

			// Verifica esistenza
			try {
				if (!fs::exists(PathToScan)) {
					return -1;
				}
				if (!is_directory(PathToScan)) {
					return -1;
				}
			} catch (fs::filesystem_error& e) {
				return(-1);
			}

			if (!CaseSensitive) {
				// Campi di ricerca in maiuscolo
				if (ExtContentList != NULL) {
					for (size_t ixE=0;ixE<ExtContentList->size();ixE++) {
						DString::ToUpper(ExtContentList->at(ixE));
					}
				}
				if (NameContentList != NULL) {
					for (size_t ixN=0;ixN<NameContentList->size();ixN++) {
						DString::ToUpper(NameContentList->at(ixN));
					}
				}
			}

			for (fs::directory_iterator iterator(PathToScan); iterator != fs::directory_iterator(); ++iterator) {
				if (is_directory(iterator->status())) {
					if (Recoursive) {
						// In ricorsione
						Tot+=ListFiles(Result,iterator->path(),Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,Recoursive,FindAll);
					}
				}
				else {
					// Cerca il pattern nel nome
					bool Found=true;
					// Directory attuale
					fs::path CurrPath;
					if (!CaseSensitive) {
						// Ingnora case
						CurrPath=DString::ToUpperCopy(iterator->path().string());
					}
					else {
						CurrPath=iterator->path();
					}

					if (NameContentList != NULL) {
						// Ricerca patterns all'interno del nome
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CurrPath.stem() != NameContent) {
										Found=false;
									}
								}
								else {
									// Ricerca all'interno del nome file
									if (CurrPath.stem().string().find(NameContent) == std::string::npos) {
										Found=false;
									}
								}
							}
						}
						else {
							// Found diventa true al primo match
							Found=false;
							for (std::string NameContent : *NameContentList) {
								DString::RemoveChars(NameContent,"*");
								DString::RemoveChars(NameContent,".");
								if (NameWholeWord) {
									// Ricerca parola intera
									if (CurrPath.stem() == NameContent) {
										Found=true;
										break;
									}
								}
								else {
									// Ricerca al''interno dell'estensione
									if (CurrPath.stem().string().find(NameContent) != std::string::npos) {
										Found=true;
										break;
									}
								}
							}
						}
					}

					if (!Found) {
						continue;
					}

					// Cerca il pattern nell'estensione
					if (ExtContentList != NULL) {
						std::string CurrExt=CurrPath.extension().string();
						DString::RemoveChars(CurrExt,".");
						if (FindAll) {
							// Found rimane true solo se vengono trovati tutti i pattern
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CurrExt != ExtContent) {
										Found=false;
									}
								}
								else {
									// Ricerca all'interno dell'estensione
									if (CurrExt.find(ExtContent) == std::string::npos) {
										Found=false;
									}
								}
							}
						}
						else {
							// Found diventa true al primo match
							Found=false;
							for (std::string ExtContent : *ExtContentList) {
								DString::RemoveChars(ExtContent,"*");
								DString::RemoveChars(ExtContent,".");
								if (ExtWholeWord) {
									// Ricerca parola intera
									if (CurrExt == ExtContent) {
										Found=true;
										break;
									}
								}
								else {
									// Ricerca al''interno dell'estensione
									if (CurrExt.find(ExtContent) != std::string::npos) {
										Found=true;
										break;
									}
								}
							}
						}
					}

					if (Found) {
						Tot++;
						if (Result != NULL) {
							Result->push_back(iterator->path());
						}
					}
				}
			}
			return(Tot);
		}

		// Cerca tutti i files all'interno di una cartella
		/**
		* Ricerca tramite una stringa per nome e una stringa per l'estensione
		*
		* @param Result			->	Puntatore ad una array di path da popolare con i risultati, se NULL ritorna solo il conteggio
		* @param PathToScan		->	Directory di partenza
		* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContent	->	Stringa di ricerca per il nome, vuota equilave a tutto
		* @param NameWholeWord	->	Se true confronta la ricerca con l'intero nome
		* @param ExtContent		->	Stringa di ricerca per l'estensione, vuota equilave a tutto
		* @param ExtWholeWord	->	Se true confronta la ricerca con l'intera estensione
		*
		* @return il numero di files trovati, -1 in caso di errore
		* N.B. @ref Result non viene pulita qindi s non è vuoa  risultati vengono aggiunti
		**/
		int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord) {
			std::vector<std::string> NameContentList;
			std::vector<std::string> ExtContentList;
			if (!NameContent.empty()) {
				NameContentList.push_back(NameContent);
			}
			if (!ExtContent.empty()) {
				ExtContentList.push_back(ExtContent);
			}
			return(ListFiles(Result,PathToScan,Recoursive,&NameContentList,NameWholeWord,&ExtContentList,ExtWholeWord,Recoursive,false));
		}

		// Conta i files all'interno di una cartella
		/**
		* Ricerca tramite una lista per nome e una lista per l'estensione
		*
		* @param PathToScan		->	Directory di partenza
		* @param Recoursive			->	Se true va in ricorsione (solo se non è un ordine Prodig)
		* @param NameContentList	->	Puntatore ad un array di stringhe di ricerca per il nome (esclusa l'estensione), NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nel nome
		* @param NameWholeWord		->	Se true confronta la ricerca con l'intero nome (Ignorato se @ref NameContentList contiene più di una stringa)
		* @param ExtList			->	Puntatore ad un array di estensioni da cercare, NULL o vuoto equivale a tutto
		*								N.B. Tutte le stringhe devo essere contenute nell'estensione
		* @param ExtWholeWord		->	Se true confronta la ricerca con l'intera estensione (Ignorato se @ref ExtContentList contiene più di una stringa)
		* @param FindAll			->	Se true esegue una ricerca di tipo AND: tutte le stringhe di @ref NameContent e @ExtContent devono essere trovate in ogni nome ed estensione
		*								Se false esegue una ricerca di tipo OR: basta che una delle stringhe di @ref NameContent e @ExtContent venga trovata in ogni nome ed estensione
		*
		* @return il numero di files trovati, -1 in caso di errore
		**/
		int CountFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool FindAll) {
			return(ListFiles(NULL,PathToScan,Recoursive,NameContentList,NameWholeWord,ExtContentList,ExtWholeWord,Recoursive,FindAll));
		}

		// Conta i files all'interno di una cartella
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
		int CountFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord) {
			return(ListFiles(NULL,PathToScan,Recoursive,NameContent,NameWholeWord,ExtContent,ExtWholeWord));
		}

		// Conta tutti i files all'interno di una cartella
		/**
		* Ricerca tramite una stringa per nome e una stringa per l'estensione
		*
		* @param PathToScan	->	Directory di partenza
		* @param Recoursive		->	Se true va in ricorsione (solo se non è un ordine Prodig)
		*
		* @return il numero di files trovati, -1 in caso di errore
		**/
		int CountFiles(fs::path PathToScan, bool Recoursive) {
			return(ListFiles(NULL,PathToScan,Recoursive,NULL,false,NULL,false,true));
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
	}
}

/*
		#if defined _WIN32 || defined _WIN64
*/
			//! Get version of EXE or DLL
			/*
			* @param Filename   ->  filename to extract version from, if empty, current process filename is used (default: empty)
			* @param TrimDots	->  if true trims all dots from string (default: true).
			* @return string containing file version. If TrimDots is false returen string is like "1.0.0.0" ,otherwise is like "1000"
			*/
/*
			std::string GetFileVersion(std::string Filename, bool TrimDots) {
				LPVOID lpStr1 = NULL;
				LPVOID lpStr2 = NULL;
				WORD* wTmp;
				DWORD dwHandlev = NULL;
				unsigned int nRet;
				unsigned int dwLength;
				char sFileName[1024]={0};
				char sTmp[1024]={0};
				LPVOID* pVersionInfo;
				std::string sRet;

				if (Filename.empty())
					GetModuleFileName(NULL,sFileName,1024);
				else
					strcpy(sFileName,Filename.c_str());

				DWORD dwInfoSize = GetFileVersionInfoSize((char*)(LPCTSTR)sFileName, &dwHandlev);
				if (dwInfoSize) {
					pVersionInfo = new LPVOID[dwInfoSize];
					nRet = GetFileVersionInfo((char*)(LPCTSTR)sFileName, dwHandlev, dwInfoSize, pVersionInfo);
					if(nRet) {
						nRet = VerQueryValue(pVersionInfo, "\\VarFileInfo\\Translation", &lpStr1, &dwLength);
						if (nRet) {
							wTmp = (WORD*)lpStr1;
							wsprintf(sTmp,"\\StringFileInfo\\%04x%04x\\FileVersion", *wTmp, *(wTmp + 1));
							nRet = VerQueryValue(pVersionInfo, sTmp, &lpStr2, &dwLength);
							if(nRet) {
								sRet=std::string((char*)lpStr2);
							}
						}
					}
					delete[] pVersionInfo;
				}
				return (sRet);
			}
		#endif
*/
