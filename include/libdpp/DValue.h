#ifndef DValueH
#define DValueH

#include <string>
#include <vector>
#include <libdpp/DError.h>

namespace DTools
{
	class DValue {
        public:
            DValue(size_t InitCount = 0);
		    std::string Get(size_t Index = 0);
		    std::string GetAll(std::string Separator = "\r\n");
			void GetVect(std::vector<std::string>& Dest);
			std::vector<std::string> GetVect(void);
		    size_t Count(void);
			bool Empty(void);
		    void Add(std::string Value);
            bool Set(size_t Index, std::string Value);
		    void RemoveDuplicates(void);
		    std::vector<std::string> Values;
		    bool Exists(std::string Value, bool CaseSensistive = true);
			DError::DErrorCode ErrorCode;
	};
}
	
#endif
