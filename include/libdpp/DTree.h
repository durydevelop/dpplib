#ifndef DTreeH
#define DTreeH

#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace DTools
{
	class DTree {
		public:
			DTree();
			DTree(boost::property_tree::ptree RootTree);

			bool LoadJsonText(std::istream& JsonContent);

			bool IsEmpty(void);

			int ReadInteger(std::string SubTree, std::string Item, int Default, char Translator = DEFAULT_TRANSLATOR);
			std::string ReadString(std::string SubTree, std::string Item, std::string Default, char Translator = DEFAULT_TRANSLATOR);
			std::string ReadString(std::string Item, std::string Default, char Translator = DEFAULT_TRANSLATOR);
			float ReadFloat(std::string SubTree, std::string Item, float Default, char Translator = DEFAULT_TRANSLATOR);
			uint8_t ReadByte(std::string SubTree, std::string Item, uint8_t Default, char Translator = DEFAULT_TRANSLATOR);
			bool ReadBool(std::string SubTree, std::string Item, bool Default, char Translator = DEFAULT_TRANSLATOR);
			bool ReadBool(std::string Item, bool Default, char Translator = DEFAULT_TRANSLATOR);

			bool HasChildern(void);
			bool HasData(void);
			bool HasChildren(std::string SubTree, char Translator = DEFAULT_TRANSLATOR);
			bool HasData(std::string SubTree, char Translator = DEFAULT_TRANSLATOR);
			size_t ChildrenCount();
			size_t ChildrenCount(std::string SubTree, char Translator = DEFAULT_TRANSLATOR);
			size_t ReadItemNames(std::string SubTree,std::vector<std::string>& ResultList, char Translator = DEFAULT_TRANSLATOR);
			DTree ReadTree(std::string SubTree, char Translator = DEFAULT_TRANSLATOR);

			bool WriteTree(std::string SubTree, DTree NewTree, char Translator = DEFAULT_TRANSLATOR);
			bool WriteInteger(std::string SubTree, std::string Item, int Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteString(std::string SubTree, std::string Item, std::string Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteString(std::string Tree, std::string Value, char Translator = DEFAULT_TRANSLATOR);
			bool WritePath(std::string SubTree, std::string Item, std::string Path, char Translator = DEFAULT_TRANSLATOR);
			bool WriteFloat(std::string SubTree, std::string Item, float Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteByte(std::string SubTree, std::string Item, uint8_t Value, char Translator = DEFAULT_TRANSLATOR);
			bool WriteBool(std::string SubTree, std::string Item, bool Value, char Translator = DEFAULT_TRANSLATOR);

			bool DeleteItem(std::string SubTree, std::string Item, char Translator = DEFAULT_TRANSLATOR);
			bool DeleteContent(std::string SubTree, char Translator = DEFAULT_TRANSLATOR);

			std::string GetLastStatus(void);

		protected:
			boost::property_tree::ptree RootNode;
			std::string LastStatus;

		private:
			static const char DEFAULT_TRANSLATOR = '.';
	};
}
#endif
