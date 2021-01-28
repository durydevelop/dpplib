#ifndef DSyncFileH
#define DSyncFileH

#include "DFilesystem.h"
#include <string>
#include "DPath.h"

namespace DTools {
	class DSyncFile {
		public:
			enum DSyncStatus {SYNC_ERROR=-1, SYNC_NOT_YET=0, SYNC_NO_NEEDED=1, SYNC_DONE=2, CALLBACK_STR_MSG=16};
			DSyncFile(DTools::fs::path SourceFilename, DTools::fs::path DestFilename, bool SyncNow);
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
