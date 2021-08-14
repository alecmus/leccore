//
// shell.cpp - shell helper class implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../system.h"
#include "../error/win_error.h"
#include <Windows.h>

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
	}
}
