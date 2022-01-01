//
// gdiplus_bitmap.cpp - GDI+ bitmap wrapper implementation
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

#include "gdiplus_bitmap.h"
#include "../../error/win_error.h"
#include "../../leccore_common.h"

#include <comdef.h>	// for _com_error

#pragma comment(lib, "GdiPlus.lib")

gdiplus_bitmap::gdiplus_bitmap() { _p_bitmap = nullptr; }
gdiplus_bitmap::~gdiplus_bitmap() { empty(); }

void gdiplus_bitmap::empty() {
	if (_p_bitmap) {
		delete _p_bitmap;
		_p_bitmap = nullptr;
	}
}

bool gdiplus_bitmap::load(const std::string& file, std::string& error) {
	empty();
	_p_bitmap = Gdiplus::Bitmap::FromFile(liblec::leccore::convert_string(file).c_str());

	Gdiplus::Status status = _p_bitmap->GetLastStatus();

	if (status != Gdiplus::Ok) {
		error.assign(liblec::leccore::get_gdiplus_status_info(&status));
		empty();
		return false;
	}

	return true;
}

bool gdiplus_bitmap::load(HBITMAP bmp, std::string& error) {
	empty();
	_p_bitmap = Gdiplus::Bitmap::FromHBITMAP(bmp, NULL);

	Gdiplus::Status status = _p_bitmap->GetLastStatus();

	if (status != Gdiplus::Ok) {
		error.assign(liblec::leccore::get_gdiplus_status_info(&status));
		empty();
		return false;
	}

	return true;
}

gdiplus_bitmap_resource::gdiplus_bitmap_resource() {
	h_buffer = nullptr;
}

gdiplus_bitmap_resource::~gdiplus_bitmap_resource() {
	empty();
}

bool gdiplus_bitmap_resource::load(UINT id, LPCTSTR p_type, std::string& error, HMODULE h_inst) {
	return load(MAKEINTRESOURCE(id), error, p_type, h_inst);
}

bool gdiplus_bitmap_resource::load(UINT id, UINT type, std::string& error, HMODULE h_inst) {
	return load(MAKEINTRESOURCE(id), error, MAKEINTRESOURCE(type), h_inst);
}

void gdiplus_bitmap_resource::empty() {
	gdiplus_bitmap::empty();

	if (h_buffer) {
		::GlobalUnlock(h_buffer);
		::GlobalFree(h_buffer);
		h_buffer = NULL;
	}
}

bool gdiplus_bitmap_resource::load(LPCTSTR p_name, std::string& error, LPCTSTR p_type, HMODULE h_inst) {
	empty();

	HRSRC h_resource = ::FindResource(h_inst, p_name, p_type);

	if (!h_resource) {
		error = get_last_error();
		return false;
	}

	DWORD image_size = ::SizeofResource(h_inst, h_resource);

	if (!image_size) {
		error = get_last_error();
		return false;
	}

	const void* p_resource_data = ::LockResource(::LoadResource(h_inst, h_resource));

	if (!p_resource_data) {
		error = get_last_error();
		return false;
	}

	h_buffer = ::GlobalAlloc(GMEM_MOVEABLE, image_size);

	if (h_buffer) {
		void* p_buffer = ::GlobalLock(h_buffer);

		if (p_buffer) {
			CopyMemory(p_buffer, p_resource_data, image_size);

			IStream* p_stream = NULL;

			HRESULT h_result = CreateStreamOnHGlobal(h_buffer, FALSE, &p_stream);

			if (h_result == S_OK) {
				_p_bitmap = Gdiplus::Bitmap::FromStream(p_stream);
				p_stream->Release();

				Gdiplus::Status status = _p_bitmap->GetLastStatus();

				if (_p_bitmap) {
					if (status == Gdiplus::Ok)
						return true;
					else
						error.assign(liblec::leccore::get_gdiplus_status_info(&status));

					delete _p_bitmap;
					_p_bitmap = NULL;
				}
				else {
					error.assign(liblec::leccore::get_gdiplus_status_info(&status));
				}
			}
			else {
				_com_error err(h_result);
				error = liblec::leccore::convert_string(err.ErrorMessage());
			}

			::GlobalUnlock(h_buffer);
		}
		else
			error = get_last_error();

		::GlobalFree(h_buffer);
		h_buffer = NULL;
	}
	else
		error = get_last_error();

	return false;
}
