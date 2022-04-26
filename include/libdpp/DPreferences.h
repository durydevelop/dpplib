#ifndef DPreferencesH
#define DPreferencesH

#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace DTools
{
	class DPreferences {
		public:
			DPreferences(std::string PrefFilename = std::string(), bool CreateIfNotExists = true);
			virtual ~DPreferences();

			bool Load(void);
			bool Save(bool BackupBefore = false);
			bool IsReady(void);
			bool IsEmpty(void);

			std::string GetLastStatus(void);
			std::string GetFilename(void);
			int ReadInteger(std::string SectionTree, std::string Item, int Default, char Translator = DEFAULT_TRANSLATOR);
			std::string ReadString(std::string SectionTree, std::string Item, std::string Default, char Translator = DEFAULT_TRANSLATOR);
			//std::string ReadDotString(std::string SectionTree, std::string Item, std::string Default, char DotTranslator = '_');
			float ReadFloat(std::string SectionTree, std::string Item, float Default, char Translator = DEFAULT_TRANSLATOR);
			uint8_t ReadByte(std::string SectionTree, std::string Item, uint8_t Default, char Translator = DEFAULT_TRANSLATOR);
			bool ReadBool(std::string SectionTree, std::string Item, bool Default, char Translator = DEFAULT_TRANSLATOR);
			size_t ReadItemNames(std::string SectionTree,std::vector<std::string>& ResultList, char Translator = DEFAULT_TRANSLATOR);

			bool WriteInteger(std::string SectionTree, std::string Item, int Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteString(std::string SectionTree, std::string Item, std::string Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteString(std::string Tree, std::string Value, char Translator = DEFAULT_TRANSLATOR);
			bool WritePath(std::string SectionTree, std::string Item, std::string Path, char Translator = DEFAULT_TRANSLATOR);
			bool WriteFloat(std::string SectionTree, std::string Item, float Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteByte(std::string SectionTree, std::string Item, uint8_t Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteBool(std::string SectionTree, std::string Item, bool Value, char Translator = DEFAULT_TRANSLATOR);

			bool DeleteItem(std::string SectionTree, std::string Item, char Translator = DEFAULT_TRANSLATOR);
			bool DeleteContent(std::string SectionTree, char Translator = DEFAULT_TRANSLATOR);

		private:
			static const char DEFAULT_TRANSLATOR = '.';
			std::string PrefFile;
			boost::property_tree::ptree RootNode;
			std::string LastStatus;
			bool Ready;
	};
}
#endif
