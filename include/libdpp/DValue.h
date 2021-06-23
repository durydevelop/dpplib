#ifndef DValueH
#define DValueH

#include <string>
#include <vector>

class DValue {
	    public:
		    std::string Get(size_t Index = 0);
		    std::string GetAll(std::string Separator = "\r\n");
		    size_t Count(void);
			bool Empty(void);
		    void Add(std::string Value);
		    void RemoveDuplicates(void);
		    std::vector<std::string> Values;
		    bool Exists(std::string Value, bool CaseSensistive = true);
	};
	
#endif