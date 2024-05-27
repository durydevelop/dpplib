#include "dpplib/DStringGrid.h"
#include <istream>
#include <fstream>
#include "dpplib/DCsv.h"

/**
vector<vector<string> > Grid
  Row     Col

Grid.push_back(Row)

Grid.at(RowNr).push_back(col)
**/

#define DEFAULT_SEPCHAR ';'
#define DEFAULT_LINESEP "\r\n"

namespace DTools {
	//! Constructor
	DStringGrid::DStringGrid()
	{
		std::vector<std::string> Row(1);
		Grid.push_back(Row);
		RowCount=1;
		ColCount=1;
	}

	DStringGrid::~DStringGrid()
	{
	}

	//! Add an empty row after last one.
	/**
	* Added row contains ColCount empty fields
	* @return index of added row
	**/
	size_t DStringGrid::AddRow(void)
	{
		// Create empty row of ColCount size
		std::vector<std::string> Row(ColCount);
		// Add the row
		Grid.push_back(Row);
		RowCount++;
		return(RowCount-1);
	}

	//! Add some rows after last one.
	/**
	* Added row contains ColCount empty fields
	* @param Items  ->  vector of strings to add
	* @return index of added row
	**/
	size_t DStringGrid::AddRow(std::vector<std::string> Items)
	{
		// Add the row
		/* TODO
		if (Items.size() > ColCount) {
			SetColCount(Items.size)
		}
		*/
		Grid.push_back(Items);
		RowCount++;
		return(RowCount-1);
	}

	//! Add an empty row after last one.
	/**
	* Added row contains ColCount empty fields
	* @param Index	->	Index of row to set
	* @return false if Index is grater than RowCount, otherwise true.
	**/
	bool DStringGrid::SetRow(size_t Index, std::vector<std::string> &Items)
	{
		if (Items.size() > ColCount) {
			// Aggiungo le colonne che mancano
			AddCols(Items.size()+ColCount);
		}

		if (Index >= RowCount) {
			return false;
		}

		// Riga da modificare
		std::vector<std::string> &CurrRow=Grid.at(Index);
		for (size_t ixCol=0;ixCol<CurrRow.size();ixCol++) {
			if (ixCol < Items.size()) {
				// sostituisco con i nuovi valori
				CurrRow.at(ixCol)=Items.at(ixCol);
			}
			else {
				// oltre a Items.size() li svuoto
				CurrRow.at(ixCol)="";
			}
		}

		return true;
	}

	/**
	* Add an empty col after last one.
	* N.B. To add an empty column all other lines must be resized to new ColCount value
	* @return   ->  Index of added col
	**/
	size_t DStringGrid::AddCol(void)
	{
		AddCols(1);
		return(ColCount-1);
	}

	//! Add some columns after last one.
	/**
	* @param Count	->  Numbers of columns to add.
	* N.B. All other lines must be resized to new ColCount value
	**/
	void DStringGrid::AddCols(size_t Count)
	{
		ColCount+=Count;
		for (unsigned int ixR=0;ixR<Grid.size();ixR++) {
			std::vector<std::string> &Row=Grid.at(ixR);
			if (Row.size() < ColCount) {
				Row.resize(ColCount);
			}
		}
	}

	void DStringGrid::SetColCount(size_t NewColCount)
	{
		if (NewColCount > ColCount) {
			AddCols(NewColCount-ColCount);
		}
		else if (NewColCount < ColCount) {
			//TODO: DeleteCols
		}
	}

	/**
	* Set value content of the Cell located at RowIndex,ColIndex
	* @param RowIndex	->  Index of cow
	* @param ColIndex	->  Index of column
	* @param Value      ->  Value to Write
	**/
	void DStringGrid::Set(unsigned int RowIndex, unsigned int ColIndex, std::string Value)
	{
		Grid.at(RowIndex).at(ColIndex)=Value;
	}

	/**
	* Get content of the Cell located at RowIndex,ColIndex
	* @param RowIndex	->  Index of cow
	* @param ColIndex	->  Index of column
	* @return cell content
	**/
	std::string DStringGrid::Get(unsigned int RowIndex, unsigned int ColIndex)
	{
		return(Grid.at(RowIndex).at(ColIndex));
	}

	//! @return current nr of columns
	unsigned int DStringGrid::GetColCount(void)
	{
		return(ColCount);
	}

	//! @return current nr of rows
	unsigned int DStringGrid::GetRowCount(void)
	{
		return(RowCount);
	}

	//! @return Index of last column
	unsigned int DStringGrid::LastColIx(void)
	{
		return(ColCount-1);
	}

	//! @return Index of last row
	unsigned int DStringGrid::LastRowIx(void)
	{
		return(RowCount-1);
	}

	//! Save Grid content to File
	bool DStringGrid::SaveToCsvFile(std::string Filename, char StringSeparator)
	{
		std::ofstream FileStream(Filename);
		if (!FileStream.is_open()) {
			return false;
		}

		for (std::vector<std::string> vLine : Grid) {
			for (auto it=vLine.begin();it != vLine.end()-1; it++) {
				FileStream << *it << StringSeparator;
			}
			FileStream << vLine.back();
			FileStream << std::endl;
		}

		FileStream.close();

		return true;
	}

	//! Load Grid content from File
	bool DStringGrid::LoadFromCsvFile(std::string Filename, char StringSeparator)
	{
		std::ifstream FileStream(Filename);
		if (!FileStream.is_open()) {
			return false;
		}

		std::string Line;
		while(std::getline(FileStream,Line)) {
			std::vector<std::string> LineItems;
			DCsv::ReadCSVRow(LineItems,Line,StringSeparator);
			AddRow(LineItems);
		}

		FileStream.close();

		return true;
	}
}

