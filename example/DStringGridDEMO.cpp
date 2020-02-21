#include "DStringGridDEMO.h"
#include "DStringGrid.hpp"
#include "DCsv.hpp"
#include <iostream>
#include <fstream>

void DStringGridDEMO(std::string Filename, char cSep)
{
    DStringGrid csv;

    // Open file
	std::ifstream fileStream(Filename);
	if (!fileStream.is_open()) {
		std::cout << "Error opening " << Filename;
		return;
	}

	// For each lines add a row
	std::string line;
	while (std::getline(fileStream, line)) {
		if (!line.empty()) {
			std::vector<std::string> Items;
			// split csv line
			DTools::DCsv::ReadCSVRow(Items,line,cSep);
			csv.AddRow(Items);
		}
	}
	fileStream.close();

    // Write DStringGrid content as a matrix
	for (size_t ixRow=0;ixRow<csv.GetRowCount();ixRow++) {
	    line.clear();
        for (size_t ixCol=0;ixCol<csv.GetColCount();ixCol++) {
            line.append(csv.Get(ixRow,ixCol)+"\t");
        }
        std::cout << line << std::endl;
	}
}
