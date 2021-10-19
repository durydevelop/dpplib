#ifndef DSyncFileH
#define DSyncFileH

#include "libdpp/DFilesystem.h"
#include <string>
#include "libdpp/DPath.h"

namespace DTools
{
	class DSyncFile {
		public:
			enum DSyncStatus { SYNC_ERROR=-1, SYNC_NOT_YET=0, SYNC_NO_NEEDED=1, SYNC_DONE=2, SYNC_RESTORED=3, SYNC_CALLBACK_STR_MSG=16 };
			DSyncFile(DTools::fs::path SourceFilename, DTools::fs::path DestFilename, bool SyncNow, bool SafeCopyMode);
			DSyncStatus DoSync(void);
			bool IsReady(void);
			void InvertSync(void);
			void SetSafeMode(bool Enabled);
			bool GetSafeMode(void);

			DTools::fs::path Source;
			DTools::fs::path Dest;
			DSyncStatus LastSyncStatus;
			std::string LastSyncTime;
			std::string LastStrStatus;
		private:
			bool Ready;
			bool SafeMode;
	};
}

#endif
