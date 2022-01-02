//
// gdiplus_bitmap_to_file.cpp - save GDI+ bitmap to file implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "gdiplus_bitmap_to_file.h"
#include "../../leccore_common.h"

// ensure the use of a given extension for full path specified
// forces file to be saved with the ".extension" extension regardless of what the user specified
// whatever existing extension will be removed
// formatted path will be written back to full_path
void format_to_ext(std::string& full_path, const std::string& extension) {
	std::string ext(extension);

	// determine the position of the last slash (after which the file's name begins)
	auto directory_end_index = full_path.rfind('\\');

	if (directory_end_index == std::string::npos) {
		// maybe its unix style
		directory_end_index = full_path.rfind('/');

		if (directory_end_index == std::string::npos)
			directory_end_index = 0;	// this is probably just a file name (for current folder)
	}

	// remove extension if present in the file's name
	if (true) {
		const size_t idx = full_path.rfind('.');

		if (idx != std::string::npos && idx > directory_end_index)
			full_path.erase(idx);
	}

	// remove dot(s) from supplied extension (if necessary)
	const size_t idx = ext.rfind('.');

	if (std::string::npos != idx)
		ext = ext.substr(idx + 1);

	// add extension to path
	if (!ext.empty())
		full_path = full_path + "." + ext;
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

bool gdiplus_bitmap_to_file(Gdiplus::Bitmap* bitmap,
	std::string& full_path,
	liblec::leccore::image::format format,
	std::string& error) {
	bool success = true;

	if (full_path.empty()) {
		error = "File name not specified.";
		return false;
	}

	if (bitmap) {
		CLSID enc_id;
		std::wstring mimetype;

		switch (format) {
		case liblec::leccore::image::format::bmp:
			mimetype = L"image/bmp";
			format_to_ext(full_path, "bmp");
			break;

		case liblec::leccore::image::format::jpg:
			mimetype = L"image/jpeg";
			format_to_ext(full_path, "jpg");
			break;

		case liblec::leccore::image::format::png:
		default:
			mimetype = L"image/png";
			format_to_ext(full_path, "png");
			break;
		}

		if (get_encoder_clsid(mimetype, enc_id) > -1) {
			// check if there is an alpha channel
			const bool source_has_alpha_channel = bitmap->GetPixelFormat() & PixelFormatAlpha;
			const bool destination_has_alpha_channel = format == liblec::leccore::image::format::png;

			if (source_has_alpha_channel && !destination_has_alpha_channel) {
				// clone the bitmap
				Gdiplus::Bitmap* bmp_out = bitmap->Clone(0, 0, bitmap->GetWidth(), bitmap->GetHeight(), bitmap->GetPixelFormat());
				Gdiplus::Graphics graphics(bmp_out);

				// modify clone by clearing the background
				graphics.Clear(Gdiplus::Color::White);
				graphics.DrawImage(bitmap, 0, 0);

				// save the modified clone to file
				Gdiplus::Status status = bmp_out->Save(liblec::leccore::convert_string(full_path).c_str(), &enc_id);

				if (status != Gdiplus::Status::Ok) {
					error.assign(liblec::leccore::get_gdiplus_status_info(&status));
					success = false;
				}
				else
					success = true;

				// cleanup the clone
				delete bmp_out;
			}
			else {
				// save bitmap to file
				Gdiplus::Status status = bitmap->Save(liblec::leccore::convert_string(full_path).c_str(), &enc_id);

				if (status != Gdiplus::Status::Ok) {
					error.assign(liblec::leccore::get_gdiplus_status_info(&status));
					success = false;
				}
				else
					success = true;
			}
		}
	}
	else {
		error = "Invalid bitmap.";
		return false;
	}

	return success;
}
