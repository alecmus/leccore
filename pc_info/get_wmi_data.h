//
// get_wmi_data.h - get wmi (windows management instrumentation) data interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include <comdef.h>
#include <string>
#include <vector>
#include <map>
#include <any>

namespace liblec {
	namespace leccore {
		bool get_wmi_data(bstr_t class_name,
			std::vector<bstr_t> properties,
			std::map<std::string, std::vector<std::any>>& wmi_data,
			std::string& error);
	}
}
