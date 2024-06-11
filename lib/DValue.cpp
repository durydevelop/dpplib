#include "dpplib/DValue.h"
#include "dpplib/DString.h"
#include <algorithm>

namespace DTools
{
    DValue::DValue(size_t InitCount) {
        if (InitCount > 0) {
            Values.resize(InitCount,"");
        }
    }

	std::string DValue::Get(size_t Index) {
		if (Values.empty() || Index > Values.size()-1) {
			return(std::string());
		}
		return(Values.at(Index));
	}

	std::string DValue::GetAll(std::string Separator) {
		std::string Ret;
		for (size_t ixV=0;ixV<Values.size();ixV++) {
			Ret.append(Values.at(ixV));
			if (ixV < Values.size()-1) {
				Ret.append(Separator);
			}
		}
		return(Ret);
	}

	void DValue::GetVect(std::vector<std::string>& Dest) {
		for (size_t ixV=0;ixV<Values.size();ixV++) {
			Dest.emplace_back(Values.at(ixV));
		}
	}

	std::vector<std::string> DValue::GetVect(void) {
		std::vector<std::string> Dest;
		for (size_t ixV=0;ixV<Values.size();ixV++) {
			Dest.emplace_back(Values.at(ixV));
		}
		return(Dest);
	}

	size_t DValue::Count(void) {
		return(Values.size());
	}

	bool DValue::Empty(void) {
		return(Values.empty());
	}

	void DValue::Add(std::string Value) {
		Values.push_back(Value);
	}

    bool DValue::Set(size_t Index, std::string Value) {
        if (Index >= Values.size()) {
            return false;
        }
        Values[Index]=Value;
        return true;
    }

	void DValue::RemoveDuplicates(void) {
		Values.erase(std::unique(Values.begin(),Values.end()),Values.end());
	}

	bool DValue::Exists(std::string Value, bool CaseSensistive) {
		if (!CaseSensistive) {
			DTools::DString::ToUpper(Value);
		}

		for (std::string dValue : Values) {
			if (!CaseSensistive) {
				DTools::DString::ToUpper(dValue);
			}
			if (Value == dValue) {
				return true;
			}
		}
		return false;
	}
}
