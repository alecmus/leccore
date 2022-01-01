//
// image.h - image interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#if defined(LECCORE_EXPORTS)
#include "leccore.h"
#else
#include <liblec/leccore.h>
#endif

#include <string>

namespace liblec {
	namespace leccore {
		/// <summary>Image class.</summary>
		class leccore_api image {
		public:
			image(int png_resource);
			image(const std::string& file);
			~image();

			/// <summary>Image format.</summary>
			enum class format {
				/// <summary>Portable Network Graphics.</summary>
				png,

				/// <summary>JPEG image.</summary>
				jpg,

				/// <summary>Bitmap image.</summary>
				bmp,
			};

			/// <summary>Save the image to file as-is.</summary>
			/// <param name="full_path">The full path to the destination file. On a successful save the
			/// 'actual' path is written back to this parameter.</param>
			/// <param name="format">The image format, as defined in <see cref='image::format'></see>.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool save(std::string& full_path,
				image::format format,
				std::string& error);

			/// <summary>Save the image to file, resampling it to a defined size.</summary>
			/// <param name="full_path">The full path to the destination file. On a successful save the
			/// 'actual' path is written back to this parameter.</param>
			/// <param name="format">The image format, as defined in <see cref='image::format'></see>.</param>
			/// <param name="quality">The quality to use when resampling, as defined in <see cref='image_quality'></see>.</param>
			/// <param name="size">The size of the image, as defined in <see cref='leccore::size'></see>.</param>
			/// <param name="crop">Whether to crop the image.</param>
			/// <param name="keep_aspect_ratio">Whether to keep the aspect ratio of the image.</param>
			/// <param name="enlarge_if_smaller">Whether to enlarge the image if the target size is smaller
			/// than the current size of the image.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool save(std::string& full_path,
				image::format format,
				image_quality quality,
				leccore::size size,
				bool crop,
				bool keep_aspect_ratio,
				bool enlarge_if_smaller,
				std::string& error);

		private:
			class impl;
			impl& _d;

			// Default constructor and copying an object of this class are not allowed
			image() = delete;
			image(const image&) = delete;
			image& operator=(const image&) = delete;
		};
	}
}
