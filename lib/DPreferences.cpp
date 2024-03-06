#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <libdpp/DPreferences.h>
#include <libdpp/DFilesystem.h>
#include <libdpp/DPath.h>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <boost/exception/all.hpp>

namespace pt=boost::property_tree;

namespace DTools
{
	//! Constructor
	DPreferences::DPreferences(std::string Filename, bool CreateIfNotExists)
	{
		if (Filename.empty()) {
        	// Default name
        	Filename=fs::path(fs::current_path() / "Settings.json").string();
    	}

		PrefFile=Filename;
		if (!fs::path(PrefFile).has_parent_path()) {
			PrefFile=fs::path(fs::current_path() / Filename).string();
		}
		Ready=true;

		if (!DTools::DPath::Exists(PrefFile)) {
			if (CreateIfNotExists) {
				LastStatus.append(PrefFile+" missing, try to create...");
				Ready=Save();
			}
			else {
				LastStatus.append(PrefFile+" missing, preferences not ready");
				Ready=false;
			}
		}
		else {
			Ready=Load();
		}
	}

	DPreferences::~DPreferences()
	{
		//dtor
	}

	//! Load Preferences from @PrefFile
	/**
	*  @return true on success, otherwise false.
	*  To get error message call @ref GetLastStatus()
	**/
	bool DPreferences::Load(void)
	{
		if (!DTools::DPath::Exists(PrefFile)) {
			Ready=false;
			return Ready;
		}

		try {
			pt::json_parser::read_json(PrefFile,RootNode);
		}catch (boost::exception& e) {
			LastStatus.append(boost::diagnostic_information(e));
			return false;
		}
		return true;
	}

	//! Save Preferences to PrefFile
	/**
	*  @return true on success, otherwise false.
	*  To get error message call @ref GetLastStatus()
	**/
	bool DPreferences::Save(bool BackupBefore)
	{
		if (!Ready) {
			LastStatus="Unable to save, Preference non ready";
			return false;
		}

		if (BackupBefore) {
			fs::path File=fs::path(PrefFile);
			std::string Name=File.filename().string()+ ".bak";
			fs::path BackupFile=File.parent_path() / Name;
			#if __cplusplus > 201402L // C++17
				fs::copy_options options=fs::copy_options::overwrite_existing;
			#else
				fs::copy_option options=fs::copy_option::overwrite_if_exists;
			#endif

			try {
				fs::copy_file(PrefFile,BackupFile,options);
			}catch (fs::filesystem_error &e) {
				LastStatus="ERRORE Backup "+PrefFile+" : "+e.what();
				return false;
			}
		}
		try {
			pt::json_parser::write_json(PrefFile,RootNode);
			//info_parser::write_info(sFilename,RootNode);
		}catch (boost::exception& e) {
			LastStatus=boost::diagnostic_information(e);
			return false;
		}
		return true;
	}

	//! @return true if json file is opened and parsed without errors
	bool DPreferences::IsReady(void) {
		return(Ready);
	}

	//! @return filename in which preferences are stored
	std::string DPreferences::GetFilename(void)	{
		return(PrefFile);
	}

    std::string DPreferences::ToString(void) {
        std::stringstream ss;
        pt::json_parser::write_json(ss,RootNode);
        return ss.str();
    }
}
