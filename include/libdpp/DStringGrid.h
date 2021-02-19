#ifndef DStringGridH
#define DStringGridH

#include <string>
#include <vector>
#include <sstream>

namespace DTools
{
	class DStringGrid {
		public:
			DStringGrid();
			~DStringGrid();

			size_t AddRow(void);
			size_t AddRow(std::vector<std::string> Items);
			size_t AddCol(void);
			void AddCols(size_t Count);
			bool SetRow(size_t Index, std::vector<std::string> &Items);
			void SetColCount(size_t NewColCount);
			std::string Get(unsigned int RowIndex, unsigned int ColIndex);
			void Set(unsigned int RowIndex, unsigned int ColIndex, std::string Value);
			unsigned int GetColCount(void);
			unsigned int GetRowCount(void);
			unsigned int LastColIx(void);
			unsigned int LastRowIx(void);

			bool SaveToCsvFile(std::string Filename, char StringSeparator = ';');

			std::vector<std::string> readCSVRow(std::string &row);
			std::vector<std::vector<std::string> > readCSV(std::istream &in);
		protected:

		private:
			unsigned int RowCount;  //! Current number of Rows
			unsigned int ColCount;  //! Current number of Columns
			std::vector<std::vector<std::string>> Grid;    //! Matrix of strings
	};
}
#endif
