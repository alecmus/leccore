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

			/// <summary>Image options.</summary>
			class leccore_api image_options {
				image::format _format = image::format::jpg;
				image_quality _quality = image_quality::medium;
				leccore::size _size;
				bool _enlarge_if_smaller = false;
				bool _keep_aspect_ratio = true;
				bool _crop = false;

			public:
				/// <summary>Get or set the image format.</summary>
				/// <returns>A reference to the property.</returns>
				image::format& format();

				/// <summary>Set the image format.</summary>
				/// <param name="format">The image format, as defined in <see cref='image::format'></see>.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& format(const image::format& format);

				/// <summary>Get or set the image resampling quality.</summary>
				/// <returns>A reference to the property.</returns>
				image_quality& quality();

				/// <summary>Set the image resampling quality.</summary>
				/// <param name="quality">The quality to use when resampling, as defined in <see cref='image_quality'></see>.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& quality(const image_quality& quality);

				/// <summary>Get or set the image target size.</summary>
				/// <returns>A reference to the property.</returns>
				leccore::size& size();

				/// <summary>Set the image target size.</summary>
				/// <param name="size">The target size of the image, as defined in <see cref='leccore::size'></see>.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& size(const leccore::size& size);

				/// <summary>Get or set whether to enlarge the image if it is smaller than the target size.</summary>
				/// <returns>A reference to the property.</returns>
				bool& enlarge_if_smaller();

				/// <summary>Set whether to enlarge the image if it is smaller than the target size.</summary>
				/// <param name="enlarge_if_smaller">Whether to enlarge the image if it is smaller than the target size.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& enlarge_if_smaller(const bool& enlarge_if_smaller);

				/// <summary>Get or set whether to keep the image aspect ratio.</summary>
				/// <returns>A reference to the property.</returns>
				bool& keep_aspect_ratio();

				/// <summary>Set whether to keep the image aspect ratio.</summary>
				/// <param name="keep_aspect_ratio">Whether to keep the image aspect ratio.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& keep_aspect_ratio(const bool& keep_aspect_ratio);

				/// <summary>Get or set whether to crop the image.</summary>
				/// <returns>A reference to the property.</returns>
				bool& crop();

				/// <summary>Set whether to crop the image.</summary>
				/// <param name="crop">Whether to crop the image.</param>
				/// <returns>A reference to the modified object.</returns>
				image_options& crop(const bool& crop);
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
			/// <param name="options">The image options, as defined in <see cref='image_options'></see>.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool save(std::string& full_path,
				image_options& options,
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
