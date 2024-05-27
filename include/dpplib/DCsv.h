//---------------------------------------------------------------------------
#ifndef DCsvH
#define DCsvH
//---------------------------------------------------------------------------
#include <vector>
#include <string>

namespace DTools
{
namespace DCsv
{
    size_t ReadCSVRow(std::vector<std::string> &DestList, std::string Row, std::string Separators, size_t MaxFieldsCount = 0);
    size_t ReadCSVRow(std::vector<std::string> &DestList, std::string Row, char cSep, size_t MaxFieldsCount = 0);
    size_t CsvToVector(std::vector<std::string>& DestVect, std::string CsvString, std::string Separators);
    size_t CsvToVector(std::vector<std::string>& DestVect, std::string CsvString, char cSep);
    //std::vector<std::string> CsvToVector(std::string CsvString, char cSep);
    std::string ReadCSVField(std::string Row, char cSep, size_t FieldNr);
    std::string ReplaceCSVField_Copy(std::string Row, char cSep, size_t Index, std::string ReplaceStr);
/* TODO
		class DListaCSV {
	public:
		class DItemCSV {
			private:
				boost::filesystem::path RemoteFile; // File remoto
				boost::filesystem::path LocalFile;  // File locale
			public:
				std::string Name;
				bool Sync;                   		// Sincronizza remoto -> locale
				ifstream Content;               	// Input File Stream
				bool LocalPresent;                  // true se il file locale è presente
				bool RemotePresent;                 // true se il file remoto è presente
				bool Opened;                        // true se lo file locale è è aperto
				boost::filesystem::path SyncPath;   // Dir su cui creare il watch dog
				std::string LastStatus;             // Esito ultima operazione
				std::vector<std::string>Lines;      // Contenuto file in righe

				DItemCSV() {
					Name="";
					Sync=false;
					LocalPresent=false;
					RemotePresent=false;
					Opened=false;
				}

				DItemCSV(std::string ItemName, boost::filesystem::path LocalDir, boost::filesystem::path Filename, bool Syncronize) {
					Name=ItemName;
					Set(LocalDir,Filename,Syncronize);
				}

				~DItemCSV() {
					if (Content.is_open()) {
						Content.close();
					}
				}

				void Set(boost::filesystem::path LocalDir,boost::filesystem::path Filename, bool Syncronize) {
					LocalFile=LocalDir / Filename.filename();
					RemoteFile=Filename;
					Sync=Syncronize;
					LocalPresent=false;
					RemotePresent=false;
					Opened=false;

					if (Filename.parent_path() != LocalDir && !Filename.parent_path().empty()) {
						// File non locale (quindi remoto)
						if (exists(RemoteFile)) {
							RemotePresent=true;
							SyncPath=RemoteFile.parent_path();
						}
						else {
							LastStatus="File remoto NON esiste";
						}
					}

					if (RemotePresent) {
						// File non locale e presente
						//if (!exists(LocalFile)) {
							// Se non esiste in locale lo copio
							// Lo copio comunque
							try {
								copy_file(RemoteFile,LocalFile,boost::filesystem::copy_option::overwrite_if_exists);
							} catch (boost::filesystem::filesystem_error& e) {
								LastStatus=e.what();
								return;
							}
						//}
					}

					if (exists(LocalFile)) {
						LocalPresent=true;
					}

					// Crea un input file stream
					Content.open(LocalFile.c_str(),std::ifstream::in);
					if (Content.is_open()) {
						LastStatus="Local File Aperto";
						if (Load() > 0) {
							LastStatus=LastStatus+" "+std::to_string(Lines.size())+" Linee";
						}
						else {
							LastStatus=LastStatus+" vuoto";
						}

						Opened=true;
					}
					else {
						LastStatus="Local File NON Aperto";
					}

					return;
				}

				boost::filesystem::path GetFile(void) {
					return(RemoteFile);
				}

				size_t Load(void) {
					Lines.clear();
					std::string Line;
					Content.clear();
					Content.seekg(0,Content.beg);
					while (getline(Content,Line)) {
						Lines.push_back(Line);
					}
					return(Lines.size());
				}

				size_t GetLinesCount(void) {
					return(Lines.size());
				}

				void DoSync(void) {
					try {
						time_t RemoteLastWrite=boost::filesystem::last_write_time(RemoteFile);
						time_t LocalLastWrite=boost::filesystem::last_write_time(LocalFile);
						if (RemoteLastWrite > LocalLastWrite) {
							copy_file(RemoteFile,LocalFile);
						}
					} catch (boost::filesystem::filesystem_error& e) {
						LastStatus="Sync error: "+std::string(e.what());
						return;
					}
				}

				std::string GetLine(size_t Index) {
					if (Index >= Lines.size()) {
						return("");
					}
					return(Lines.at(Index));
				}

				std::vector<std::string>& GetLines(void) {
					return(Lines);
				}
		};

		std::vector<DItemCSV*>Items;
		boost::filesystem::path LocalDir;

		DListaCSV(boost::filesystem::path LocalPath) {
			LocalDir=LocalPath;
			if (!exists(LocalDir)) {
				create_directories(LocalDir);
				// TODO gestione errore
			}
		}

		~DListaCSV() {
			Clear();
		}

		DItemCSV* AddItem(std::string ItemName, boost::filesystem::path Filename, bool Syncronize) {
			DItemCSV *Item=new DItemCSV(ItemName,LocalDir,Filename,Syncronize);
			Items.push_back(Item);
			return(Items.back());
		}
		DItemCSV* SetItem(std::string ItemName, boost::filesystem::path Filename, bool Syncronize) {
			DItemCSV *Item=GetItem(ItemName);
			Item->Set(LocalDir,Filename,Syncronize);
			return(Item);
		}
		DItemCSV* SetItem(size_t ItemIndex, boost::filesystem::path Filename, bool Syncronize) {
			DItemCSV *Item=GetItem(ItemIndex);
			Item->Set(LocalDir,Filename,Syncronize);
            return(Item);
		}

		DItemCSV* GetItem(size_t ItemIndex) {
			if (ItemIndex >= Items.size()) {
				return NULL;
			}
			return(Items.at(ItemIndex));
		}

		DItemCSV* GetItem(std::string ItemName) {
			if (ItemName.empty()) {
				return(NULL);
			}

			boost::to_upper(ItemName);
			for (size_t ixI=0;ixI<Items.size();ixI++) {
				if (boost::to_upper_copy(Items.at(ixI)->Name) == ItemName) {
					return(Items.at(ixI));
				}
			}
			return NULL;
		}

		DItemCSV* GetItemFormPath(boost::filesystem::path SyncPath) {
			for (size_t ixI=0;ixI<Items.size();ixI++) {
				if (SyncPath == Items.at(ixI)->SyncPath) {
					return(Items.at(ixI));
				}
			}
			return(NULL);
		}

		std::vector<std::string> GetItemsNames(void) {
			std::vector<std::string> DestList;
			for (size_t ixI=0;ixI<Items.size();ixI++) {
				DestList.push_back(Items.at(ixI)->Name);
			}
			return(DestList);
		}

		bool ItemExists(std::string ItemName) {
            return(GetItem(ItemName) != NULL);
        }

		size_t Count(void) {
			return(Items.size());
		}

		void Clear() {
			for (UINT ixI=0;ixI<Items.size();ixI++) {
				delete Items.at(ixI);
			}
			Items.clear();
		}
};
*/
} // DCsv
} // DTools
#endif
