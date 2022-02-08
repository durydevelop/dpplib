#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include "libdpp/DPreferences.h"
#include "libdpp/DFilesystem.h"
#include "libdpp/DPath.h"
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <boost/exception/all.hpp>

namespace pt=boost::property_tree;

namespace DTools
{
	//! Constructor
	DPreferences::DPreferences(std::string Filename, bool CreateIfNotExists)
	{
		if (Filename.empty()) {
        	// Default name
        	Filename=fs::path(fs::current_path() / "Settings.json").string();
    	}

		PrefFile=Filename;
		if (!DTools::DPath::Exists(PrefFile)) {
			if (CreateIfNotExists) {
				Ready=Save();
			}
			else {
				Ready=false;
			}
		}
		else {
			Ready=Load();
		}
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
		if (!DTools::DPath::Exists(PrefFile)) {
			Ready=false;
			return Ready;
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	int DPreferences::ReadInteger(std::string SectionTree, std::string Item, int Default, char Translator) {
		if (!Ready) return Default;
		return(RootNode.get(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Default));
	}

	//! Retrive a string value
	/**
	* Try to get the string value of @ref Item inside @ref SectionTree
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	std::string DPreferences::ReadString(std::string SectionTree, std::string Item, std::string Default, char Translator) {
		if (!Ready) return Default;
		return(RootNode.get<std::string>(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Default));
	}

	//! Retrive a path type value. This method is usefull to retrive a Value containing dots, like path.
	/**
	* Try to get the string value of Item inside SectionTree
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param DotTranslator	->	alternative char to use
	* WARNING: SectionTree MUST NOT contain node names with dots. If in your tree there are node name containing dots, the result tree for read becomes wrong.
	* In this case You MUST REPLACE Your dots in node names with an alternatice char and pass it as DotTranslator.
	* e.g.:
	* In json node:
	*	@code
	*	{
	*		"Files": {
	*			"List": {
	*				"Settings.json": { <---- DOTTED NAME
	*					"Action": "DELETE",
	*				}
	*			}
	*		}
	*	}
	*	@code
	*	The result tree becomes "Files.List.Settings.json" and read will missing.
	*
	*	You must create Sectiontree string as "Files.List.Settings_json" and then retrive "Action" Item:
	*	@code
	*	std::string TreeString="Files.List.Settings_json";
	*	std::string Value=ReadDotString(TreeString,"Action","",'_');
	*	@code
	*
	**/
/*
	std::string DPreferences::ReadDotString(std::string SectionTree, std::string Item, std::string Default, char DotTranslator) {
		if (!Ready) return Default;

		// By default all '.' becomes '/'
		// and all '_' becomes '.'
		char NodeTranslator='/';
		// but if DotTranslator passed is '/'
		// needs to change NodeTranslator
		if (DotTranslator == '/') {
			NodeTranslator='_';
		}
		// replace translator in tree
		std::replace(SectionTree.begin(),SectionTree.end(),'.',NodeTranslator);
		std::replace(SectionTree.begin(),SectionTree.end(),DotTranslator,'.');

		// use getter with translator
		return(RootNode.get<std::string>(pt::ptree::path_type(SectionTree+NodeTranslator+Item,NodeTranslator),Default));
	}
*/
	//! Retrive a float value
	/**
	* Try to get the float value of @ref Item inside @ref SectionTree
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	float DPreferences::ReadFloat(std::string SectionTree, std::string Item, float Default, char Translator) {
		if (!Ready) return Default;
		return(RootNode.get<float>(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Default));
	}

	//! Retrive a byte value
	/**
	* Try to get the uint8_t value of @ref Item inside @ref SectionTree
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	uint8_t DPreferences::ReadByte(std::string SectionTree, std::string Item, uint8_t Default, char Translator) {
		if (!Ready) return Default;
		return(RootNode.get<uint8_t>(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Default));
	}

	//! Retrive a boolean value
	/**
	* Try to get the boolean value of @ref Item inside @ref SectionTree
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	**/
	bool DPreferences::ReadBool(std::string SectionTree, std::string Item, bool Default, char Translator) {
		if (!Ready) return Default;
		return(RootNode.get<bool>(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Default));
	}

	//! Read all item names in (@link SectionTree) node and put them in @ResultList
	/**
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param ResultList		->	reference to vector of strings to fill with results.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return numbers of items found
	**/
	size_t DPreferences::ReadItemNames(std::string SectionTree,std::vector<std::string>& ResultList, char Translator) {
		ResultList.clear();
		if (!Ready) return 0;
		// Find SectionTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SectionTree,Translator));
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DPreferences::WriteInteger(std::string SectionTree, std::string Item, int Value, char Translator)
	{
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Value);
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Value if exists
	**/
	bool DPreferences::WriteString(std::string SectionTree, std::string Item, std::string Value, char Translator)
	{
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Value);
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
	* @param Tree		->	node tree separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Value		->	string value to write.
	* @param Translator	->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Value if exists
	**/
	bool DPreferences::WriteString(std::string Tree, std::string Value, char Translator)
	{
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(Tree,Translator),Value);
		}
		catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}

		LastStatus="Write succesfully";
		return true;
	}

	// TODO WriteDotString()

	//! Write filesystem path as "value"
	/**
	* Write filesystem path value into Item inside SectionTree node.
	* N.B.:
	* -Value is converted to a fs::path type (to avoid issues with trailing chars).
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Path			->	path string to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists.
	**/
	bool DPreferences::WritePath(std::string SectionTree, std::string Item, std::string Path, char Translator) {
		if (!Ready) return false;
		try {
			// Convert to fs::path
			fs::path p(Path);
			// Write
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),p.string());
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DPreferences::WriteFloat(std::string SectionTree, std::string Item, float Value, char Translator) {
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Value);
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DPreferences::WriteByte(std::string SectionTree, std::string Item, uint8_t Value, char Translator) {
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Value);
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
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DPreferences::WriteBool(std::string SectionTree, std::string Item, bool Value, char Translator)	{
		if (!Ready) return false;
		try {
			RootNode.put(pt::ptree::path_type(SectionTree+Translator+Item,Translator),Value);
		}
		catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}

		LastStatus="Write succesfully";
		return true;
	}

	/**
	* @brief Delete Item (and its value) of SectionTree.
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*/
	bool DPreferences::DeleteItem(std::string SectionTree, std::string Item, char Translator) {
		if (!Ready) return false;
		// Find SectionTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SectionTree,Translator));
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

	/**
	* @brief Delete all SectionTree content.
	* @param SectionTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*/
	bool DPreferences::DeleteContent(std::string SectionTree, char Translator) {
		if (!Ready) return false;
		// Find SectionTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SectionTree,Translator));
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
