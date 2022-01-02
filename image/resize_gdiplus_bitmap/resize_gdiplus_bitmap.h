//
// resize_gdiplus_bitmap.h - resize GDI+ bitmap interface
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

Gdiplus::Bitmap* resize_gdiplus_bitmap(
	Gdiplus::Bitmap* p_bmpin,
	const liblec::leccore::size target_size,
	bool keep_aspect_ratio,
	liblec::leccore::image_quality quality,
	bool enlarge_if_smaller,
	bool crop,
	liblec::leccore::size& final_size);
