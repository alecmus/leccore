//
// resize_gdiplus_bitmap.cpp - resize GDI+ image implementation
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

	if (ratio == 1) {
		if (keep_aspect_ratio) {
			if (_width > _height) {
				// landscape picture control
				_width = _height;
			}
			else {
				// portrait picture control
				_height = _width;
			}
		}
	}
	else {
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

	const auto crop_x = _width - control_w;
	const auto crop_y = _height - control_h;

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

int get_encoder_clsid(const std::wstring& form, CLSID& clsid) {
	UINT num;
	UINT size;
	Gdiplus::ImageCodecInfo* p_image_codec_info = NULL;
	Gdiplus::GetImageEncodersSize(&num, &size);

	if (size == 0)
		return -1;

	p_image_codec_info = (Gdiplus::ImageCodecInfo*)(malloc(size));

	if (p_image_codec_info == NULL)
		return -1;

	GetImageEncoders(num, size, p_image_codec_info);

	for (UINT j = 0; j < num; j++) {
		if (wcscmp(p_image_codec_info[j].MimeType, form.c_str()) == 0) {
			clsid = p_image_codec_info[j].Clsid;
			free(p_image_codec_info);
			return j;
		}
	}

	free(p_image_codec_info);
	return -1;
}

/*
** ensure the use of a given extension for full path specified
** forces database to be saved with the ".extension" extension regardless of what the user specified
** whatever existing extension will be removed
** formatted path will be written back to sFullPath
*/
void format_to_ext(std::string& full_path, const std::string& extension) {
	std::string ext(extension);

	// remove extension if present
	while (true) {
		const size_t idx = full_path.rfind('.');

		if (std::string::npos != idx)
			full_path.erase(idx);
		else
			break;
	}

	// remove dot(s) from supplied extension (if necessary)
	const size_t idx = ext.rfind('.');

	if (std::string::npos != idx)
		ext = ext.substr(idx + 1);

	// add extension to path
	if (!ext.empty())
		full_path = full_path + "." + ext;
}

bool gdiplus_bitmap_to_file(Gdiplus::Bitmap* bitmap,
	std::string& file_name,
	liblec::leccore::image::format format,
	std::string& error) {
	bool success = true;

	if (file_name.empty()) {
		error = "File name not specified.";
		return false;
	}

	if (bitmap) {
		// save resized bitmap to file
		CLSID enc_id;
		std::wstring mimetype;

		switch (format) {
		case liblec::leccore::image::format::bmp:
			mimetype = L"image/bmp";
			format_to_ext(file_name, "bmp");
			break;

		case liblec::leccore::image::format::jpg:
			mimetype = L"image/jpeg";
			format_to_ext(file_name, "jpg");
			break;

		case liblec::leccore::image::format::png:
		default:
			mimetype = L"image/png";
			format_to_ext(file_name, "png");
			break;
		}

		if (get_encoder_clsid(mimetype, enc_id) > -1) {
			switch (format) {
			case liblec::leccore::image::format::png: {
				// save resized bitmap to file
				Gdiplus::Status status = bitmap->Save(liblec::leccore::convert_string(file_name).c_str(), &enc_id);

				if (status != Gdiplus::Status::Ok) {
					error.assign(liblec::leccore::get_gdiplus_status_info(&status));
					success = false;
				}
				else
					success = true;
			}
			break;

			case liblec::leccore::image::format::bmp:
			case liblec::leccore::image::format::jpg:
			default: {
				// clear background in case source is in PNG and has an alpha channel
				Gdiplus::Bitmap* bmp_out = new Gdiplus::Bitmap(bitmap->GetWidth(), bitmap->GetHeight(), bitmap->GetPixelFormat());
				Gdiplus::Graphics graphics(bmp_out);
				graphics.Clear(Gdiplus::Color::White);
				graphics.DrawImage(bitmap, 0, 0);

				// save resized bitmap to file
				Gdiplus::Status status = bmp_out->Save(liblec::leccore::convert_string(file_name).c_str(), &enc_id);

				if (status != Gdiplus::Status::Ok) {
					error.assign(liblec::leccore::get_gdiplus_status_info(&status));
					success = false;
				}
				else
					success = true;

				delete bmp_out;
			}
			break;
			}
		}
	}

	return success;
}
