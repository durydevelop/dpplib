#include "DCsv.h"

namespace DTools
{
	namespace DCsv {
		//! Read all csv fields in a string
		/**
		* @param DestList		->  pointer to a vector of strings for found fields.
		* @param Row			->  string to scan.
		* @param MaxFieldsCount	->  number of fields to collect, 0 for all.
		**/
		size_t ReadCSVRow(std::vector<std::string> &DestList, std::string Row, char cSep, size_t MaxFieldsCount) {
			size_t SepCount=0;	// Separators found
			size_t ixStart=0;   // Index of first char in current field
			size_t ixR=0;       // Index of current char in Row
			DestList.clear();   // ...for surely
			for (char c : Row) {
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
					break;
				}
				ixR++;
			}
			// Last Item
			std::string Item=Row.substr(ixStart,ixR-ixStart);
			if (!Item.empty()) {
				DestList.push_back(Item);
			}
			return(DestList.size());
		}

		//! Extract a field from a csv string
		/**
		* @param Row    	->  reference to string to extract from
		* @param cSep   	->  separator char
		* @param FieldNr	->  number of field to extract starting from 1. Value of 0 return the last field
		*
		* @return the extacted string if FieldNr is found otherwise an empty string. If FieldNr is 1 or 0 and no separator found same string as Row will be returned
		**/
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

			//return(Row.substr(ixStart,ixR-ixStart));

			/*
			enum class CSVState {
				UnquotedField,
				QuotedField,
				QuotedQuote
			};
			CSVState state = CSVState::UnquotedField;
			vector<string> fields {""};
			size_t ixF=0; // Index of the current field
			//size_t ixEnd=Index; // Last Field Index
			size_t SubStrLen;	// Number of SubStr char
			string SubStr;		// Current SubString
			for (size_t ixR=0;ixR<Row.size();ixR++) {
				switch (state) {
					case CSVState::UnquotedField:
						if (Row[ixR] == cSep) {
							// end of field
							SubStr=Row.substr(ixR-SubStrLen,SubStrLen);
							SubStrLen=0;
							ixF++;
						}
						else if (Row[ixR] == '"') {
							state = CSVState::QuotedField;
						}
						else {
							SubStrLen++; // Field ixF inc char count
						}
						break;
					case CSVState::QuotedField:
						if (Row[ixR] == '"') {
							state = CSVState::QuotedQuote;
						}
						else {
							SubStrLen++; // Field ixF inc char count
						}
						break;
					case CSVState::QuotedQuote:
						if (Row[ixR] == cSep) {
							// cSep after closing quote
							SubStr=Row.substr(ixR-SubStrLen,SubStrLen);
							ixF++;
							state = CSVState::UnquotedField;
						}
						else if (Row[ixR] == '"') {
							// "" -> "
							//fields[ixF].push_back('"');
							SubStrLen++; // Field ixF inc char count
							state = CSVState::QuotedField;
						}
						else {
							// end of quote
							state = CSVState::UnquotedField;
						}
						break;
				}

				if (ixF == FieldNr) {
					Row.replace(ixR-SubStr.size()-1,ixR-1,SubStr);
					return true;
				}
			}

			return false;
            */
		}
	}
}
