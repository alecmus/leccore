//
// appcast.h - appcast interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include "../web_update.h"

namespace liblec {
	namespace leccore {
		bool load_appcast(const std::string& xml,
			liblec::leccore::check_update::update_info& details,
			std::string& error);
	}
}
