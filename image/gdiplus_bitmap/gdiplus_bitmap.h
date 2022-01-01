//
// gdiplus_bitmap.h - GDI+ bitmap wrapper interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

/*
** based on code from http://www.codeproject.com/Articles/3537/Loading-JPG-PNG-resources-using-GDI
*/

#pragma once

#include <Windows.h>
#include <GdiPlus.h>
#include <string>

class gdiplus_bitmap {
public:
	gdiplus_bitmap();
	virtual ~gdiplus_bitmap();

	bool load(const std::string& file, std::string& error);
	bool load(HBITMAP bmp, std::string& error);

	operator Gdiplus::Bitmap* () const { return _p_bitmap; }

protected:
	Gdiplus::Bitmap* _p_bitmap;
	void empty();
};

class gdiplus_bitmap_resource : public gdiplus_bitmap {
public:
	gdiplus_bitmap_resource();
	virtual ~gdiplus_bitmap_resource();

	bool load(UINT id, LPCTSTR p_type, std::string& error, HMODULE h_inst);

protected:
	HGLOBAL h_buffer;
	void empty();

private:
	bool load(LPCTSTR p_name, std::string& error,
		LPCTSTR p_type = RT_RCDATA,
		HMODULE h_inst = NULL);

	bool load(UINT id, UINT type, std::string& error, HMODULE h_inst = NULL);
};
