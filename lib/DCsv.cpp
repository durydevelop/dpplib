#include "libdpp/DCsv.h"

namespace DTools
{
namespace DCsv
{
	/**
	 * @brief Read all csv fields in a string.
     * N.B.
     * '\r' or '\n' means end of line (also they are in the middle).
     * 
	 * @param DestList		    ->  pointer to a vector of strings for found fields.
	 * @param Row			    ->  string to scan.
	 * @param Separators		->	list of separators to check for.
	 * @param MaxFieldsCount	->  number of fields to collect, 0 for all.
	 * @return numbers of items found.
	 **/
	size_t ReadCSVRow(std::vector<std::string> &DestList, std::string Row, std::string Separators, size_t MaxFieldsCount) {
/* TODO:
        std::vector<std::vector<std::string>> csvRows;

        for (std::string line; std::getline(pwdFile, line);) {
            std::istringstream ss(std::move(line));
            std::vector<std::string> row;
            // std::getline can split on other characters, here we use ','
            for (std::string value; std::getline(ss, value, ':');) {
                DLog::debug("value=%s",value.c_str());
                row.emplace_back(std::move(value));
            }
            csvRows.emplace_back(std::move(row));
        }
*/
		size_t SepCount=0;	// Separators found
		size_t ixStart=0;   // Index of first char in current field
		size_t ixR=0;       // Index of current char in Row
		DestList.clear();   // ...for surely
		for (char c : Row) {
			for (char cSep : Separators) {
				if (c == cSep) {
					// End of field
					SepCount++;
					if (SepCount == MaxFieldsCount) {
						break;
					}
					DestList.push_back(Row.substr(ixStart,ixR-ixStart));
					// Update start field pos
					ixStart=ixR+1;
				}
				else if (c == '\r' || c == '\n') {
                    // Last Item
                    std::string Item=Row.substr(ixStart,ixR-ixStart);
                    if (!Item.empty()) {
                        DestList.push_back(Item);
                    }
					break;
				}
			}
			ixR++;
		}
		
		return(DestList.size());
	}

	size_t ReadCSVRow(std::vector<std::string> &DestList, std::string Row, char cSep, size_t MaxFieldsCount) {
		return(ReadCSVRow(DestList,Row,std::string(1,cSep),MaxFieldsCount));
	}

    //! Extract all csv fields from string to a vector (copy version)
    //std::vector<std::string> CsvToVector(std::string CsvString, char cSep) {
    //    std::vector<std::string> Result;
    //    ReadCSVRow(Result,CsvString,cSep);
    //    return(Result);
    //}

    // TODO:
    /*
    std::vector<std::string> CsvToVector(std::string CsvString, std::string Separators) {

    }
    */

    // Extract all csv fields from string to a vector using more separators
    size_t CsvToVector(std::vector<std::string>& DestVect, std::string CsvString, std::string Separators) {
        ReadCSVRow(DestVect,CsvString,Separators);
        return(DestVect.size());
    }

    // Extract all csv fields from string to a vector using only one separtor
    size_t CsvToVector(std::vector<std::string>& DestVect, std::string CsvString, char cSep) {
        ReadCSVRow(DestVect,CsvString,std::string(1,cSep));
        return(DestVect.size());
    }

	/**
	* @brief Extract a field from a csv string.
	* @param Row    	->  reference to string to extract from
	* @param cSep   	->  separator char
	* @param FieldNr	->  number of field to extract starting from 1. Value of 0 return the last field
	*
	* @return the extacted string if FieldNr is found otherwise an empty string. If FieldNr is 1 or 0 and no separator found, Row will be returned
	 */
	std::string ReadCSVField(std::string Row, char cSep, size_t FieldNr) {
		size_t SepCount=0;	// Separators found
		size_t ixStart=0;   // Index of first char in current field
		size_t ixR=0;       // Index of current char in Row
		for (char c : Row) {
			if (c == cSep) {
				// End of field
				SepCount++;
				if (SepCount == FieldNr) {
					break;
				}
				// Update start field pos
				ixStart=ixR+1;
			}
			ixR++;
		}

		if (SepCount == 0) {
			if (FieldNr > 1) {
				return(std::string());
			}
			else {
				return(Row);
			}
		}

		return(Row.substr(ixStart,ixR-ixStart));
	}

	//! Replace a field in a csv string
	/**
	* @param Row    	->  reference to string to extract from
	* @param cSep   	->  separator char
	* @param FieldNr	->  number of field to extract starting from 1. Value of 0 means the last field
	* @param ReplaceStr	->  new field name
	*
	* @return new row name if FieldNr is found otherwise an empty string. If FieldNr is 1 or 0 and no separator found same string as ReplaceStr will be returned
	**/
	std::string ReplaceCSVField_Copy(std::string Row, char cSep, size_t FieldNr, std::string ReplaceStr) {
		size_t SepCount=0;	// Separators found
		size_t ixStart=0;   // Index of first char in current field
		size_t ixR=0;       // Index of current char in Row
		for (char c : Row) {
			if (c == cSep) {
				// End of field
				SepCount++;
				if (SepCount == FieldNr) {
					break;
				}
				// Update start field pos
				ixStart=ixR+1;
			}
			ixR++;
		}

		if (SepCount == 0) {
			if (FieldNr > 1) {
				return(std::string());
			}
			else {
				return(ReplaceStr);
			}
		}

		std::string Ret=Row.substr(0,ixStart);
		Ret.append(ReplaceStr);
		Ret.append(Row.substr(ixR,Row.length()-ixR));

		return(Ret);
	}
} // DCsv
} // DTools
