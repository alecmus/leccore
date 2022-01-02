//
// resize_gdiplus_bitmap.cpp - resize GDI+ bitmap implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "resize_gdiplus_bitmap.h"
#include "../../leccore_common.h"

Gdiplus::Bitmap* resize_gdiplus_bitmap(
	Gdiplus::Bitmap* p_bmpin,
	const liblec::leccore::size target_size,
	bool keep_aspect_ratio,
	liblec::leccore::image_quality quality,
	bool enlarge_if_smaller,
	bool crop,
	liblec::leccore::size& final_size) {
	if (!p_bmpin)
		return nullptr;

	int _width = (int)target_size.get_width();
	int _height = (int)target_size.get_height();

	// deduce old height and ratio
	UINT old_height = p_bmpin->GetHeight();
	UINT old_width = p_bmpin->GetWidth();
	float ratio = ((float)old_width) / ((float)old_height);

	if (enlarge_if_smaller == false) {
		if ((int)old_width < _width && (int)old_height < _height) {
			// both sides of the image are smaller than the target dimensions, preserve size
			_width = old_width;
			_height = old_height;
		}
	}

	// save target dimensions of picture control
	const int control_w = _width;
	const int control_h = _height;

	if (keep_aspect_ratio) {
		if (crop) {
			// adjust either new width or height to keep aspect ratio
			if (old_width < old_height) {
				_height = (int)(_width / ratio);

				if (_height < control_h) {
					_height = control_h;
					_width = (int)(_height * ratio);
				}
			}
			else {
				_width = (int)(_height * ratio);

				if (_width < control_w) {
					_width = control_w;
					_height = (int)(_width / ratio);
				}
			}
		}
		else {
			// adjust either new width or height to keep aspect ratio
			if (old_width > old_height) {
				_height = (int)(_width / ratio);

				if (_height > control_h) {
					_height = control_h;
					_width = (int)(_height * ratio);
				}
			}
			else {
				_width = (int)(_height * ratio);

				if (_width > control_w) {
					_width = control_w;
					_height = (int)(_width / ratio);
				}
			}
		}
	}

	// set quality of resizing image
	// by adjusting the Gdiplus interpolation mode
	Gdiplus::InterpolationMode interpolation_mode;
	Gdiplus::PixelOffsetMode pixel_mode;

	switch (quality) {
	case liblec::leccore::image_quality::low:
		interpolation_mode = Gdiplus::InterpolationMode::InterpolationModeLowQuality;
		pixel_mode = Gdiplus::PixelOffsetMode::PixelOffsetModeDefault;
		break;

	case liblec::leccore::image_quality::medium:
		interpolation_mode = Gdiplus::InterpolationMode::InterpolationModeBilinear;
		pixel_mode = Gdiplus::PixelOffsetMode::PixelOffsetModeDefault;
		break;

	case liblec::leccore::image_quality::high:
		interpolation_mode = Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear;
		pixel_mode = Gdiplus::PixelOffsetMode::PixelOffsetModeHalf;
		break;

	default:
		interpolation_mode = Gdiplus::InterpolationMode::InterpolationModeDefault;
		pixel_mode = Gdiplus::PixelOffsetMode::PixelOffsetModeDefault;
		break;
	}

	const auto crop_x = liblec::leccore::largest(0, _width - control_w);
	const auto crop_y = liblec::leccore::largest(0, _height - control_h);

	Gdiplus::Bitmap* bmp_out = new Gdiplus::Bitmap(_width - crop_x, _height - crop_y, p_bmpin->GetPixelFormat());
	Gdiplus::Graphics graphics(bmp_out);
	graphics.SetInterpolationMode(interpolation_mode);
	graphics.SetPixelOffsetMode(pixel_mode);
	graphics.DrawImage(p_bmpin, -crop_x / 2, -crop_y / 2, _width, _height);

	final_size
		.width((float)_width - crop_x)
		.height((float)_height - crop_y);

	return bmp_out;
}
