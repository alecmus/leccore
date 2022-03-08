//
// shell.cpp - shell helper class implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../system.h"
#include "../error/win_error.h"
#include <Windows.h>

// for PathQuoteSpaces
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

// STL
#include <filesystem>

namespace liblec {
	namespace leccore {
		const bool shell::open(const std::string& path, std::string& error) {
			error.clear();

			INT_PTR result = (INT_PTR)ShellExecuteA(0, 0, path.c_str(), 0, 0, SW_SHOW);

			if (result > 32)
				return true;
			else {
				error = get_last_error();
				return false;
			}
		}

		const bool shell::view(const std::string& path, std::string& error) {
			error.clear();

			try {
				std::filesystem::path p(path);

				if (std::filesystem::is_regular_file(std::filesystem::path(path)) ||
					std::filesystem::is_directory(std::filesystem::path(path))) {
					const std::string command = "/select, " + path;
					INT_PTR result = (INT_PTR)ShellExecuteA(0, 0, "explorer.exe", command.c_str(), 0, SW_SHOW);

					if (result > 32)
						return true;
					else {
						error = get_last_error();
						return false;
					}
				}
				else {
					error = "\"" + path + "\" is neither a file nor a directory";
					return false;
				}
			}
			catch (const std::exception& e) {
				error = e.what();
				return false;
			}
		}

		const bool shell::create_process(const std::string& fullpath,
			const std::vector<std::string>& args, std::string& error) {
			CHAR szAppPath[MAX_PATH];
			lstrcpynA(szAppPath, fullpath.c_str(), static_cast<int>(fullpath.length() + 1));
			PathQuoteSpacesA(szAppPath);

			// add commandline flags
			for (const auto& it : args) {
				lstrcatA(szAppPath, " ");
				lstrcatA(szAppPath, it.c_str());
			}

			STARTUPINFOA			si = { 0 };
			PROCESS_INFORMATION		pi = { 0 };
			si.cb = sizeof(STARTUPINFO);
			if (!CreateProcessA(NULL, szAppPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
				error = "Creating process failed: " + fullpath;
				return false;
			}

			return true;
		}
	}
}
