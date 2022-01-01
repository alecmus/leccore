//
// image.cpp - image implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../image.h"
#include "gdiplus_bitmap/gdiplus_bitmap.h"
#include "resize_gdiplus_bitmap/resize_gdiplus_bitmap.h"

using namespace liblec::leccore;

class image::impl {
	gdiplus_bitmap _bitmap;
	gdiplus_bitmap_resource _bitmap_resource;

	bool _png = false;

	bool _load_successful = false;
	std::string _load_error;

public:
	impl(int png_resource, const std::string& file) {

		std::string error;
		if (png_resource) {
			_png = true;
			if (!_bitmap_resource.load(png_resource, L"PNG", error, GetModuleHandle(NULL))) {
				_load_successful = false;
				_load_error = error;
			}
		}
		else {
			_png = false;
			if (!_bitmap.load(file, error)) {
				_load_successful = false;
				_load_error = error;
			}
		}
	}

	~impl() {

	}

	Gdiplus::Bitmap* get() {
		if (_png)
			return _bitmap_resource;
		else
			return _bitmap;
	}
};

image::image(int png_resource) : _d(*new impl(png_resource, "")) {}
image::image(const std::string& file) : _d(*new impl(0, file)) {}
image::~image() { delete& _d; }

bool image::save(std::string& full_path, image::format format, std::string& error) {
	return gdiplus_bitmap_to_file(_d.get(), full_path, format, error);
}

bool image::save(std::string& full_path, image::format format, image_quality quality,
	leccore::size size, bool crop, bool keep_aspect_ratio, std::string& error) {
	leccore::size final_size;
	auto resized_image = resize_gdiplus_bitmap(_d.get(), size, keep_aspect_ratio, quality, false, final_size);
	bool success = gdiplus_bitmap_to_file(resized_image, full_path, format, error);
	if (success)
		delete resized_image;
	
	return success;
}
