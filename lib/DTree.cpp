//#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include "libdpp/DTree.h"
#include "libdpp/DFilesystem.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/all.hpp>

namespace pt=boost::property_tree;

namespace DTools
{
	/**
	 * @brief Construct an emptry DTree object.
	 */
	DTree::DTree()	{
	}

	/**
	 * @brief Contruct a DTree object from a ptree object.
	 * @param RootTree	->	boost ptree object used as root node.
	 */
	DTree::DTree(boost::property_tree::ptree RootTree) {
		RootNode=RootTree;

	}

	bool DTree::LoadJsonText(std::istream& JsonContent) {
		try {
			pt::json_parser::read_json(JsonContent,RootNode);
		}catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}
		LastStatus="Json content loaded";
		return true;
	}

    //! @return true if no sections extsts
    bool DTree::IsEmpty(void) {
        return(RootNode.empty());
    }

	//! Retrive an integer value
	/**
	* Try to get the integer value of @ref Item inside @ref SubTree
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	int DTree::ReadInteger(std::string SubTree, std::string Item, int Default, char Translator) {
		return(RootNode.get(pt::ptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a string value from a sub-tree
	/**
	* Try to get the string value of @ref Item inside @ref SubTree
	* @param SubTree		->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	std::string DTree::ReadString(std::string SubTree, std::string Item, std::string Default, char Translator) {
		return(RootNode.get<std::string>(pt::ptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a string value from root tree
	/**
	* Try to get the string value of @ref Item inside @ref SubTree
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	std::string DTree::ReadString(std::string Item, std::string Default, char Translator) {
		return(RootNode.get<std::string>(pt::ptree::path_type(Item,Translator),Default));
	}

	//! Retrive a float value
	/**
	* Try to get the float value of @ref Item inside @ref SubTree
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	float DTree::ReadFloat(std::string SubTree, std::string Item, float Default, char Translator) {
		return(RootNode.get<float>(pt::ptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a byte value
	/**
	* Try to get the uint8_t value of @ref Item inside @ref SubTree
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	uint8_t DTree::ReadByte(std::string SubTree, std::string Item, uint8_t Default, char Translator) {
		return(RootNode.get<uint8_t>(pt::ptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a boolean value
	/**
	* Try to get the boolean value of @ref Item inside @ref SubTree
	* @param SubTree		->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	bool DTree::ReadBool(std::string SubTree, std::string Item, bool Default, char Translator) {
		return(RootNode.get<bool>(pt::ptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a boolean value
	/**
	* Try to get the boolean value of @ref Item inside @ref SubTree
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	bool DTree::ReadBool(std::string Item, bool Default, char Translator) {
		return(RootNode.get<bool>(pt::ptree::path_type(Item,Translator),Default));
	}

	//! Read all item names in (@link SubTree) node and put them in @ResultList
	/**
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param ResultList		->	reference to vector of strings to fill with results.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return numbers of items found
	**/
	size_t DTree::ReadItemNames(std::string SubTree,std::vector<std::string>& ResultList, char Translator) {
		ResultList.clear();
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
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
/*
	DTree DTree::ReadChildren(std::string SubTree, char Translator) {
		DTree dTree;
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return(dTree);
		}
		pt::ptree &Node=inValue.get();

		// Read all first items
		for (auto& Item : Node) {
			DTree Tree(Item.second);
			dTree.WriteTree("",Tree);
			//std::string s=ItemName.first.data();
		}
		return(dTree);
	}
*/
	/**
	 * @brief Get node content as DTree.
	 * @param SubTree		->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	 * @param Translator	->	Alternative json tree translator char other that '.' (which is default).
	 * @return If SubTree is found , return a new DTree starting from here, otherwise return an empty DTree.
	 */
	DTree DTree::ReadTree(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return(DTree());
		}

		// return inValue as new DTree
		return(DTree(inValue.get()));
	}

	bool DTree::HasChildern(void) {
		return(RootNode.size() > 0);
	}

	bool DTree::HasChildren(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return false;
		}

		return(inValue.get().size() > 0);
	}

	bool DTree::HasData(void) {
		return(!RootNode.data().empty());
	}

	bool DTree::HasData(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return false;
		}

		return(!inValue.get().data().empty());
	}

//	std::string DTree::GetRootName(void) {
//		if (RootNode.size() != 1) {
//			return(std::string());
//		}
//		return(RootNode.data());
//	}

	size_t DTree::ChildrenCount(void) {
		return(RootNode.size());
	}

	size_t DTree::ChildrenCount(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return 0;
		}
		pt::ptree &Node=inValue.get();
		return(Node.size());
	}

	bool DTree::WriteTree(std::string SubTree, DTree NewTree, char Translator) {
		try {
			RootNode.add_child(pt::ptree::path_type(SubTree,Translator),NewTree.RootNode);
		}
		catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}
		return true;
	}

	//! Write integer data into node
	/**
	* Write integer value into @ref Item inside @ref SubTree node
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DTree::WriteInteger(std::string SubTree, std::string Item, int Value, char Translator)
	{
		try {
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),Value);
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
	* Write string value into @ref Item inside @ref SubTree node
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	string value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Value if exists
	**/
	bool DTree::WriteString(std::string SubTree, std::string Item, std::string Value, char Translator)
	{
		try {
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),Value);
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
	* Write string value into @ref Item inside @ref SubTree node
	* @param Tree		->	node tree separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Value		->	string value to write.
	* @param Translator	->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Value if exists
	**/
	bool DTree::WriteString(std::string Tree, std::string Value, char Translator)
	{
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

	//! Write filesystem path as "value"
	/**
	* Write filesystem path value into Item inside SubTree node.
	* N.B.:
	* -Value is converted to a fs::path type (to avoid issues with trailing chars).
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Path			->	path string to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists.
	**/
	bool DTree::WritePath(std::string SubTree, std::string Item, std::string Path, char Translator) {
		try {
			// Convert to fs::path
			fs::path p(Path);
			// Write
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),p.string());
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
	* Write floating point value into @ref Item inside @ref SubTree node
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DTree::WriteFloat(std::string SubTree, std::string Item, float Value, char Translator) {
		try {
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),Value);
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
	* Write byte value into @ref Item inside @ref SubTree node
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DTree::WriteByte(std::string SubTree, std::string Item, uint8_t Value, char Translator) {
		try {
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),Value);
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
	* Write boolean value into @ref Item inside @ref SubTree node
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*
	* N.B. This method overwrite existing Item if exists
	**/
	bool DTree::WriteBool(std::string SubTree, std::string Item, bool Value, char Translator)	{
		try {
			RootNode.put(pt::ptree::path_type(SubTree+Translator+Item,Translator),Value);
		}
		catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}

		LastStatus="Write succesfully";
		return true;
	}

	/**
	* @brief Delete Item (and its value) of SubTree.
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Item			->	the item name to write.
	* @param Value			->	value to write.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*/
	bool DTree::DeleteItem(std::string SubTree, std::string Item, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
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
	* @brief Delete all SubTree content.
	* @param SubTree	->	can be one or more section nodes separated by '.' e.g. "Names.Name" navigate untin Name node under Names one.
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	* @return true on successfully write otherwhise false (to get error message call @ref GetLastStatus()).
	*/
	bool DTree::DeleteContent(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::ptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			LastStatus=SubTree + "Not found";
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
	std::string DTree::GetLastStatus(void) {
		return(LastStatus);
	}
}
