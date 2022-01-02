//
// gdiplus_bitmap_to_file.h - save GDI+ bitmap to file interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#include "../../image.h"
#include <Windows.h>
#include <GdiPlus.h>

bool gdiplus_bitmap_to_file(Gdiplus::Bitmap* bitmap_in,
	std::string& full_path,
	liblec::leccore::image::format format,
	std::string& error);
