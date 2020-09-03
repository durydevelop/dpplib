#include "DPreferences.h"

#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <boost/exception/all.hpp>

namespace pt=boost::property_tree;

namespace DTools {
	namespace DPreferences {
		//! Constructor
		DPreferences::DPreferences(std::string Filename)
		{
			PrefFile=Filename;
			Ready=Load();
		}

		DPreferences::~DPreferences()
		{
			//dtor
		}

		//! Load Preferences from @PrefFile
		/**
		*  @return true on success, otherwise false.
		*  To get error message call @ref GetLastStatus()
		**/
		bool DPreferences::Load(void)
		{
			if (!fs::exists(PrefFile)) {
				Save();
			}

			try {
				pt::json_parser::read_json(PrefFile,RootNode);
				//info_parser::read_info(sFilename,RootNode);
			}catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}
			return true;
		}

		//! Save Preferences to PrefFile
		/**
		*  @return true on success, otherwise false.
		*  To get error message call @ref GetLastStatus()
		**/
		bool DPreferences::Save(bool BackupBefore)
		{
			if (BackupBefore) {
				fs::path File=fs::path(PrefFile);
				std::string Name=File.filename().string()+ ".bak";
				fs::path BackupFile=File.parent_path() / Name;
				#if __cplusplus > 201402L // C++17
					fs::copy_options options=fs::copy_options::overwrite_existing;
				#else
					fs::copy_option options=fs::copy_option::overwrite_if_exists;
				#endif

				try {
					fs::copy_file(PrefFile,BackupFile,options);
				}catch (fs::filesystem_error &e) {
					LastStatus="ERRORE Backup "+PrefFile+" : "+e.what();
					return false;
				}
			}
			try {
				pt::json_parser::write_json(PrefFile,RootNode);
				//info_parser::write_info(sFilename,RootNode);
			}catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}
			return true;
		}

		//! @return true if json file is opened and parsed without errors
		bool DPreferences::IsReady(void) {
			return(Ready);
		}

        //! @return true if no sections extsts
        bool DPreferences::IsEmpty(void) {
            return(RootNode.empty());
        }

		//! Retrive an integer value
		/**
		* Try to get the integer value of @ref Item inside @ref SectionTree
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to retrive.
		* @param Default		->	default value to return if @ref Item is empty or missing
		**/
		int DPreferences::ReadInteger(std::string SectionTree, std::string Item, int Default) {
			if (!Ready) return Default;
			return(RootNode.get(SectionTree+"."+Item,Default));
		}

		//! Retrive a string value
		/**
		* Try to get the string value of @ref Item inside @ref SectionTree
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to retrive.
		* @param Default		->	default value to return if @ref Item is empty or missing
		**/
		std::string DPreferences::ReadString(std::string SectionTree, std::string Item, std::string Default) {
			if (!Ready) return Default;
			return(RootNode.get<std::string>(SectionTree+"."+Item,Default));
			/*
			// Find SectionTree
			auto inValue=RootNode.get_child_optional(SectionTree+"."+Item);
			if (!inValue.is_initialized()) {
				return(std::string());
			}
			pt::ptree &Node=inValue.get();

			return(Node.get_value(Default));
			*/
		}

		//! Retrive a float value
		/**
		* Try to get the float value of @ref Item inside @ref SectionTree
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to retrive.
		* @param Default		->	default value to return if @ref Item is empty or missing
		**/
		float DPreferences::ReadFloat(std::string SectionTree, std::string Item, float Default) {
			if (!Ready) return Default;
			return(RootNode.get<float>(SectionTree+"."+Item,Default));
		}

		//! Retrive a byte value
		/**
		* Try to get the uint8_t value of @ref Item inside @ref SectionTree
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to retrive.
		* @param Default		->	default value to return if @ref Item is empty or missing
		**/
		uint8_t DPreferences::ReadByte(std::string SectionTree, std::string Item, uint8_t Default) {
			if (!Ready) return Default;
			return(RootNode.get<uint8_t>(SectionTree+"."+Item,Default));
		}

		//! Retrive a boolean value
		/**
		* Try to get the boolean value of @ref Item inside @ref SectionTree
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to retrive.
		* @param Default		->	default value to return if @ref Item is empty or missing
		**/
		bool DPreferences::ReadBool(std::string SectionTree, std::string Item, bool Default) {
			if (!Ready) return Default;
			return(RootNode.get<bool>(SectionTree+"."+Item,Default));
		}

		//! Read all item names in (@link SectionTree) node and put them in @ResultList
		/**
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param ResultList		->	reference to vector of strings to fill with results.
		* @return numbers of items found
		**/
		size_t DPreferences::ReadItemNames(std::string SectionTree,std::vector<std::string>& ResultList) {
			ResultList.clear();
			if (!Ready) return 0;
			// Find SectionTree
			auto inValue=RootNode.get_child_optional(SectionTree);
			if (!inValue.is_initialized()) {
				return 0;
			}
			pt::ptree &Node=inValue.get();

			// Read all first items
			for (auto& ItemName : Node) {
				std::string s=ItemName.first.data();
				ResultList.push_back(s);
			}

			return(ResultList.size());
		}

		//! Write integer data into node
		/**
		* Write integer value into @ref Item inside @ref SectionTree node
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	value to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WriteInteger(std::string SectionTree, std::string Item, int Value)
		{
			if (!Ready) return false;
			try {
				RootNode.put(SectionTree+"."+Item,Value);
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		//! Write string data into node
		/**
		* Write string value into @ref Item inside @ref SectionTree node
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	string value to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WriteString(std::string SectionTree, std::string Item, std::string Value)
		{
			if (!Ready) return false;
			try {
				RootNode.put(SectionTree+"."+Item,Value);
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		//! Write path data as "value"
		/**
		* Write path_type value into @ref Item inside @ref SectionTree node.
		* This is usefull when You need to insert int value a path like filename with '.' char.
		* The Value is converted to a @ref fs::path type to avoid issues with trailing chars
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	path string to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WritePath(std::string SectionTree, std::string Item, std::string Path) {
			if (!Ready) return false;
			try {
				fs::path p(Path);
				RootNode.put(SectionTree+"."+Item,p.string());
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		//! Write float point data into node
		/**
		* Write floating point value into @ref Item inside @ref SectionTree node
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	value to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WriteFloat(std::string SectionTree, std::string Item, float Value) {
			if (!Ready) return false;
			try {
				RootNode.put(SectionTree+"."+Item,Value);
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		//! Write byte data into node
		/**
		* Write byte value into @ref Item inside @ref SectionTree node
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	value to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WriteByte(std::string SectionTree, std::string Item, uint8_t Value) {
			if (!Ready) return false;
			try {
				RootNode.put(SectionTree+"."+Item,Value);
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		//! Write boolean data into node
		/**
		* Write boolean value into @ref Item inside @ref SectionTree node
		* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
		* @param Item			->	the item name to write.
		* @param Value			->	value to write.
		* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
		*
		* N.B. This method overwrite existing Item if exists
		**/
		bool DPreferences::WriteBool(std::string SectionTree, std::string Item, bool Value)	{
			if (!Ready) return false;
			try {
				RootNode.put(SectionTree+"."+Item,Value);
			}
			catch (boost::exception& e) {
				LastStatus=boost::diagnostic_information(e);
				return false;
			}

			LastStatus="Write succesfully";
			return true;
		}

		bool DPreferences::DeleteItem(std::string SectionTree, std::string Item) {
			if (!Ready) return false;
			// Find SectionTree
			auto inValue=RootNode.get_child_optional(SectionTree);
			if (!inValue.is_initialized()) {
				return true;
			}
			pt::ptree &Node=inValue.get();
			Node.erase(Item);
			if (Node.empty()) {
				return true;
			}
			else {
				return false;
			}
		}

		bool DPreferences::DeleteContent(std::string SectionTree) {
			if (!Ready) return false;
			// Find SectionTree
			auto inValue=RootNode.get_child_optional(SectionTree);
			if (!inValue.is_initialized()) {
				LastStatus=SectionTree + "Not found";
				return true;
			}
			pt::ptree &Node=inValue.get();
			Node.clear();
			if (Node.empty()) {
				return true;
			}
			else {
				return false;
			}
		}

		//! @return last operation status message
		std::string DPreferences::GetLastStatus(void) {
			return(LastStatus);
		}

		//! @return filename in which preferences are stored
		std::string DPreferences::GetFilename(void)	{
			return(PrefFile);
		}
	}
}
