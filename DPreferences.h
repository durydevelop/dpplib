#ifndef DPreferencesH
#define DPreferencesH

#include <vector>
#include <boost/property_tree/ptree.hpp>

// Filesystem library include
#if __cplusplus > 201402L // C++17
	#if __GNUC__ >= 8
		#include <filesystem>
	#else
		#include <experimental/filesystem>
	#endif
#else
	#include <boost/filesystem.hpp>
#endif

namespace DTools {
	namespace DPreferences {
		// Filesystem library namespace
		#if __cplusplus > 201402L // C++17
			#if __GNUC__ >= 8
				namespace fs = std::filesystem;
			#else
				namespace fs = std::experimental::filesystem;
			#endif
			namespace err = std;
		#else
			namespace fs=boost::filesystem;
			namespace err = boost::system;
		#endif

		class DPreferences {
			public:
				DPreferences(std::string PrefFilename);
				virtual ~DPreferences();
				bool Load(void);
				bool Save(bool BackupBefore = false);
				bool IsReady(void);
                bool IsEmpty(void);

				std::string GetLastStatus(void);
				std::string GetFilename(void);
				int ReadInteger(std::string SectionTree, std::string Item, int Default);
				std::string ReadString(std::string SectionTree, std::string Item, std::string Default);
				float ReadFloat(std::string SectionTree, std::string Item, float Default);
				uint8_t ReadByte(std::string SectionTree, std::string Item, uint8_t Default);
				bool ReadBool(std::string SectionTree, std::string Item, bool Default);
				size_t ReadItemNames(std::string SectionTree,std::vector<std::string>& ResultList);

				bool WriteInteger(std::string SectionTree, std::string Item, int Value);
				bool WriteString(std::string SectionTree, std::string Item, std::string Value);
				bool WritePath(std::string SectionTree, std::string Item, std::string Path);
				bool WriteFloat(std::string SectionTree, std::string Item, float Value);
				bool WriteByte(std::string SectionTree, std::string Item, uint8_t Value);
				bool WriteBool(std::string SectionTree, std::string Item, bool Value);

				bool DeleteItem(std::string SectionTree, std::string Item);
				bool DeleteContent(std::string SectionTree);

			private:
				std::string PrefFile;
				boost::property_tree::ptree RootNode;
				std::string LastStatus;
				bool Ready;
		};
    }
}
#endif
