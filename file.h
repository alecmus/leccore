//
// file.h - file helper interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#if defined(LECCORE_EXPORTS)
#include "leccore.h"
#else
#include <liblec/leccore.h>
#endif

#include <string>

namespace liblec {
	namespace leccore {
		class leccore_api file {
		public:
			static bool read(const std::string& file_path,
				std::string& data,
				std::string& error);
			static bool write(const std::string& file_path,
				const std::string& data,
				std::string& error);
			static bool remove(const std::string& file_path,
				std::string& error);
		};
	}
}
