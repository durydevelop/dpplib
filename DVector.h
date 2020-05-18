//---------------------------------------------------------------------------
#ifndef DVectorH
#define DVectorH
//---------------------------------------------------------------------------
#include <map>
#include <algorithm>
#include <vector>

namespace DTools
{
	namespace DVector {
		std::string Join(std::vector<std::string> &Vector, std::string JoinStr);

		//! Join a vector
		/**
		* @param begin		->  begin iterator of vector
		* @param JoinStr    ->  end iterator of vector
		* @param t          ->  joined obj
		* @return joined string like "value1 + joinStr + value2 + JoinStr +...."
		**/
		template <class T, class A>
		T Join(const A &begin, const A &end, const T &t) {
			T result;
			A it = begin;
			if (it != end)
				result.append(*it++);

			for( ; it!=end; ++it)
				result.append(t).append(*it);
			return result;
		}

		//! Remove duplicates from a vector (optional sort them)
		template<typename T>
		void RemoveDuplicates(std::vector<T>& vec, bool Sort = false) {
			if (Sort) {
				std::sort(vec.begin(), vec.end());
			}
			vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
		}

		//! Extract keys from a map
		template<typename TK, typename TV>
		std::vector<TK> ExtractMapKeys(std::map<TK, TV> const& input_map) {
			std::vector<TK> retval;
			for (auto const& element : input_map) {
				retval.push_back(element.first);
			}
			return retval;
		}

        //! Extract values from a map
		template<typename TK, typename TV>
		std::vector<TV> ExtractMapValues(std::map<TK, TV> const& input_map) {
			std::vector<TV> retval;
			for (auto const& element : input_map) {
				retval.push_back(element.second);
			}
			return retval;
		}
	}
}
#endif
