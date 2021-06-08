//
// user_folder.cpp - user folder implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../system.h"
#include "../leccore_common.h"
#include "../file.h"

#include <initguid.h>
#include <KnownFolders.h>
#include <ShlObj.h>

namespace liblec {
	namespace leccore {
		const std::string get_known_folder(const KNOWNFOLDERID& folder_id) {
			std::string path;
			PWSTR pszPath = NULL;
			if (SHGetKnownFolderPath(folder_id,
				KF_FLAG_CREATE,	// if the path doesn't exist, attempt to create it
				NULL, &pszPath) == S_OK)
				path = convert_string(pszPath);

			CoTaskMemFree(pszPath);
			return path;
		}

		const std::string user_folder::home() {
			return get_known_folder(FOLDERID_Profile);
		}

		const std::string user_folder::documents() {
			return get_known_folder(FOLDERID_Documents);
		}

		const std::string user_folder::downloads() {
			return get_known_folder(FOLDERID_Downloads);
		}

		const std::string user_folder::desktop() {
			return get_known_folder(FOLDERID_Desktop);
		}

		const std::string user_folder::music() {
			return get_known_folder(FOLDERID_Music);
		}

		const std::string user_folder::videos() {
			return get_known_folder(FOLDERID_Videos);
		}

		const std::string user_folder::pictures() {
			return get_known_folder(FOLDERID_Pictures);
		}

		const std::string user_folder::local_appdata() {
			return get_known_folder(FOLDERID_LocalAppData);
		}

		const std::string user_folder::temp() {
			std::string path;
			CHAR szPath[MAX_PATH + 1];
			if (GetTempPathA(MAX_PATH + 1, szPath) != 0) {
				path = szPath;

				// for consistency, remove trailing slash
				if (path[path.length() - 1] == '\\')
					path.erase(path.length() - 1, 1);

				// if the path doesn't exist, attempt to create it
				std::string error;
				if (!file::create_directory(path, error))
					path.clear();
			}
			
			return path;
		}
	}
}
