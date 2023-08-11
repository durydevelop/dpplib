//#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <libdpp/DTree.h>
#include <libdpp/DFilesystem.h>
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
	DTree::DTree(boost::property_tree::iptree RootTree) {
		RootNode=RootTree;

	}

	bool DTree::LoadJsonContent(std::istream& JsonContent) {
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
		return(RootNode.get(pt::iptree::path_type(SubTree+Translator+Item,Translator),Default));
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
		return(RootNode.get<std::string>(pt::iptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a string value from root tree
	/**
	* Try to get the string value of @ref Item inside @ref SubTree
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	std::string DTree::ReadString(std::string Item, std::string Default, char Translator) {
        if (Item.empty()) {
            return(RootNode.get<std::string>("",Default));
        }
		return(RootNode.get<std::string>(pt::iptree::path_type(Item,Translator),Default));
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
		return(RootNode.get<float>(pt::iptree::path_type(SubTree+Translator+Item,Translator),Default));
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
		return(RootNode.get<uint8_t>(pt::iptree::path_type(SubTree+Translator+Item,Translator),Default));
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
		return(RootNode.get<bool>(pt::iptree::path_type(SubTree+Translator+Item,Translator),Default));
	}

	//! Retrive a boolean value
	/**
	* Try to get the boolean value of @ref Item inside @ref SubTree
	* @param Item			->	the item name to retrive.
	* @param Default		->	default value to return if @ref Item is empty or missing
	* @param Translator		->	Alternative json tree translator char other that '.' (which is default).
	**/
	bool DTree::ReadBool(std::string Item, bool Default, char Translator) {
		return(RootNode.get<bool>(pt::iptree::path_type(Item,Translator),Default));
	}

    /**
     * @brief Read all item names of the root node and put them in @ResultList.
     * N.B. @ResultList is cleared before.
     * @param SubTree	->	Sub-node name, can be one or more section nodes separated by '.'.
     * @param ResultList ->	reference to vector of strings to fill with results.
     * @param Translator	->	Alternative json tree translator char other that '.' (which is default).
     * @return numbers of items found
     */
    size_t DTree::ReadNames(std::vector<std::string>& ResultList) {
        ResultList.clear();
        for (auto& ItemName : RootNode) {
            std::string s=ItemName.first.data();
            ResultList.emplace_back(s);
        }
        return(ResultList.size());
    }

    /**
     * @brief Read all item names of a node and put them in @ResultList.
     * N.B. @ResultList is cleared before.
    * @param SubTree	->	Sub-node name, can be one or more section nodes separated by '.'.
    * @param ResultList ->	reference to vector of strings to fill with results.
    * @param Translator	->	Alternative json tree translator char other that '.' (which is default).
	* @return numbers of items found
	**/
    size_t DTree::ReadNames(std::string SubItemName,std::vector<std::string>& ResultList, char Translator) {
		ResultList.clear();
		// Find SubTree
        auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubItemName,Translator));
		if (!inValue.is_initialized()) {
			return 0;
		}
		pt::iptree &Node=inValue.get();

		// Read all first items
		for (auto& ItemName : Node) {
			std::string s=ItemName.first.data();
            ResultList.emplace_back(s);
		}

		return(ResultList.size());
	}

    //! Read all names of the childern in a node.
    /**
    * @param SubTree	->	Can be one or more section nodes separated by '.'.
    * @param Translator	->	Alternative json tree translator char other that '.' (which is default).
    * @return a vector of string containing childern names.
    **/
    std::vector<std::string> DTree::ReadChildrenNames(std::string SubTree, char Translator) {
        std::vector<std::string> Children;

		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
            return Children;
		}
        auto node=inValue.get();

		// Read all first items
        for (auto& item : node) {
            std::string s=item.second.data();
            Children.emplace_back(s);
		}

        return(Children);
	}

    /**
     * @brief Get node child as a new DTree by index.
     * @param Index         ->  Index of the child to read (first node as index 0).
     * @return If SubTree is found, returns a new DTree starting from here, otherwise return an empty DTree.
     */
    DTree DTree::GetTree(size_t SubTreeIndex) {
        size_t ixC=0;
        for (auto& Item : RootNode) {
            if (ixC == SubTreeIndex) {
                return (DTree(Item.second));
            }
            ixC++;
        }
        return(DTree());
    }

	/**
     * @brief Get node content as a new DTree.
     * @param SubTree		->	Node name, can be one or more section nodes separated by '.'.
	 * @param Translator	->	Alternative json tree translator char other that '.' (which is default).
     * @return If SubTree is found, returns a new DTree starting from here, otherwise return an empty DTree.
	 */
    DTree DTree::GetTree(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return(DTree());
		}

		// return inValue as new DTree
		return(DTree(inValue.get()));
	}

	bool DTree::Exists(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return false;
		}
		return true;
	}

	bool DTree::HasChildren(void) {
		return(RootNode.size() > 0);
	}

	bool DTree::HasChildren(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return false;
		}

		auto value=inValue.get();
		auto n=value.size();
		return(n > 0);
	}

	bool DTree::HasData(void) {
		return(!RootNode.data().empty());
	}

	bool DTree::HasData(std::string SubTree, char Translator) {
		// Find SubTree
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
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

    size_t DTree::GetItemsCount(void) {
		return(RootNode.size());
	}

    size_t DTree::GetItemsCount(std::string SubItemName, char Translator) {
		// Find SubTree
        auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubItemName,Translator));
		if (!inValue.is_initialized()) {
			return 0;
		}
		pt::iptree &Node=inValue.get();
		return(Node.size());
	}

	bool DTree::WriteTree(std::string SubTree, DTree NewTree, char Translator) {
		try {
			RootNode.add_child(pt::iptree::path_type(SubTree,Translator),NewTree.RootNode);
		}
		catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}
		return true;
	}

	bool DTree::WriteTree(DTree NewTree) {
		try {
			RootNode=NewTree.RootNode;
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),Value);
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),Value);
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
			RootNode.put(pt::iptree::path_type(Tree,Translator),Value);
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),p.string());
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),Value);
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),Value);
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
			RootNode.put(pt::iptree::path_type(SubTree+Translator+Item,Translator),Value);
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
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			return true;
		}
		pt::iptree &Node=inValue.get();
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
		auto inValue=RootNode.get_child_optional(pt::iptree::path_type(SubTree,Translator));
		if (!inValue.is_initialized()) {
			LastStatus=SubTree + "Not found";
			return true;
		}
		pt::iptree &Node=inValue.get();
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

    std::string DTree::PrintTree(void) {
        return PrintTree(this);
    }

    std::string DTree::PrintTree(DTree *Tree) {
        std::string Ret="{\r\n";
        std::vector<std::string> ItemsNames;
        Tree->ReadNames(ItemsNames);

        for (size_t ixI=0; ixI<ItemsNames.size(); ixI++) {
            std::string Name=ItemsNames[ixI];
            std::string Value;
            DTree SubTree=Tree->GetTree(ixI);

            std::vector<std::string> nn;
            SubTree.ReadNames(nn);

            if (SubTree.HasData()) {
                Value=ReadString(Name,"");
            }
            else if (SubTree.HasChildren()) {
                Value=SubTree.PrintTree();
            }
            Ret.append(Name+" : "+Value+"\r\n");
        }
        return (Ret+"}\r\n");
    }
}
