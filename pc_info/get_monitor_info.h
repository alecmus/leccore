//
// get_monitor_info.h - get monitor information interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include "../pc_info.h"

using namespace liblec::leccore;

bool get_monitor_info(std::vector<pc_info::monitor_info>& info,
	std::string& error);
