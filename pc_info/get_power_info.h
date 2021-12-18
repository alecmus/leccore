//
// get_power_info.h - get power information interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#include "../pc_info.h"

namespace liblec {
	namespace leccore {
		bool get_power_info(pc_info::power_info& info,
			std::string& error);
	}
}
