#ifndef DPreferencesH
#define DPreferencesH

#include <vector>
#include "libdpp/DTree.h"

namespace DTools
{
	class DPreferences : public DTree {
		public:
			DPreferences(std::string PrefFilename = std::string(), bool CreateIfNotExists = true);
			virtual ~DPreferences();

			bool Load(void);
			bool Save(bool BackupBefore = false);
			bool IsReady(void);
			std::string GetFilename(void);

		private:
			std::string PrefFile;
			bool Ready;
	};
}
#endif
