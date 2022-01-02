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
#include "../leccore_common.h"
#include "gdiplus_bitmap/gdiplus_bitmap.h"
#include "resize_gdiplus_bitmap/resize_gdiplus_bitmap.h"
#include "gdiplus_bitmap_to_file/gdiplus_bitmap_to_file.h"

using namespace liblec::leccore;

class image::impl {
	gdiplus_manager _gdiplus_man;	// to ensure GDI+ is active for as long as the class object is in memory.
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
			else {
				_load_successful = true;
				_load_error.clear();
			}
		}
		else {
			_png = false;

			if (!_bitmap.load(file, error)) {
				_load_successful = false;
				_load_error = error;
			}
			else {
				_load_successful = true;
				_load_error.clear();
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

	bool load_successful() { return _load_successful; }
	const std::string& load_error() { return _load_error; }
};

image::format& image::image_options::format() { return _format; }
image::image_options& image::image_options::format(const image::format& format) {
	_format = format;
	return *this;
}

image_quality& image::image_options::quality() { return _quality; }
image::image_options& image::image_options::quality(const image_quality& quality) {
	_quality = quality;
	return *this;
}

liblec::leccore::size& image::image_options::size() { return _size; }
image::image_options& image::image_options::size(const leccore::size& size) {
	_size = size;
	return *this;
}

bool& image::image_options::enlarge_if_smaller() { return _enlarge_if_smaller; }
image::image_options& image::image_options::enlarge_if_smaller(const bool& enlarge_if_smaller) {
	_enlarge_if_smaller = enlarge_if_smaller;
	return *this;
}

bool& image::image_options::keep_aspect_ratio() { return _keep_aspect_ratio; }
image::image_options& image::image_options::keep_aspect_ratio(const bool& keep_aspect_ratio) {
	_keep_aspect_ratio = keep_aspect_ratio;
	return *this;
}

bool& image::image_options::crop() { return _crop; }

image::image_options& liblec::leccore::image::image_options::crop(const bool& crop) {
	_crop = crop;
	return *this;
}

image::image(int png_resource) : _d(*new impl(png_resource, "")) {}
image::image(const std::string& file) : _d(*new impl(0, file)) {}
image::~image() { delete& _d; }

bool image::save(std::string& full_path, image::format format, std::string& error) {
	if (_d.load_successful())
		return gdiplus_bitmap_to_file(_d.get(), full_path, format, error);
	else {
		error = _d.load_error();
		return false;
	}
}

bool image::save(std::string& full_path, image_options& options, std::string& error) {
	if (_d.load_successful()) {
		leccore::size final_size;
		auto resized_image = resize_gdiplus_bitmap(_d.get(), options.size(), options.keep_aspect_ratio(), options.quality(), options.enlarge_if_smaller(), options.crop(), final_size);
		bool success = gdiplus_bitmap_to_file(resized_image, full_path, options.format(), error);
		delete resized_image;
		return success;
	}
	else {
		error = _d.load_error();
		return false;
	}
}
