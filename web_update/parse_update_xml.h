//
// parse_update_xml.h - parse update xml interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#include "../web_update.h"

namespace liblec {
	namespace leccore {
		bool parse_update_xml(const std::string& xml,
			liblec::leccore::check_update::update_info& details,
			std::string& error);
	}
}
