//
// leccore.h - leccore interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#if defined(LECCORE_EXPORTS)
	#define leccore_api __declspec(dllexport)
#else
	#define leccore_api __declspec(dllimport)

	#if defined(_WIN64)
		#if defined(_DEBUG)
			#pragma comment(lib, "leccore64d.lib")
		#else
			#pragma comment(lib, "leccore64.lib")
		#endif
	#else
		#if defined(_DEBUG)
			#pragma comment(lib, "leccore32d.lib")
		#else
			#pragma comment(lib, "leccore32.lib")
		#endif
	#endif
#endif

#include <string>
#include <vector>

namespace liblec {
	namespace leccore {
		/// <summary>Get the version of the leccore library.</summary>
		/// <returns>Returns the version number as a string in the form
		/// "leccore 1.0.0, 11 May 2021".</returns>
		std::string leccore_api version();

		/// <summary>
		/// Format data size in B, KB, MB, GB or TB.
		/// </summary>
		/// 
		/// <param name="size">The size, in bytes.</param>
		/// <param name="precision">The number of decimal places to use for the formatted string.</param>
		///
		/// <returns>
		/// Returns a formatted string in the form 5B, 45KB, 146MB, 52GB, 9TB etc.
		/// </returns>
		std::string leccore_api format_size(unsigned long long size, unsigned short precision = 0);

		/// <summary>Rounding off class.</summary>
		class leccore_api round_off {
		public:
			/// <summary>Round-off a double to a string.</summary>
			/// <param name="d">The double to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value, as a string.</returns>
			static std::string to_string(const double& d, int precision);

			/// <summary>Round-off a double to another double.</summary>
			/// <param name="d">The double to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value.</returns>
			static double to_double(const double& d, int precision);

			/// <summary>Round-off a float to another float.</summary>
			/// <param name="d">The float to round-off.</param>
			/// <param name="precision">The number of decimal places to round it off to.</param>
			/// <returns>The rounded-off value.</returns>
			static float to_float(const float& f, int precision);
		};

		/// <summary>Put the current thread to sleep.</summary>
		/// <param name="milliseconds">The length of time to put the thread to sleep for, in milliseconds.</param>
		void leccore_api sleep(unsigned long long milliseconds);

		/// <summary>Password quality specifications.</summary>
		struct password_quality_specs {
			/// <summary>A strength up to 50% is considered weak, up to 80% considered average, and above
			/// 80% is considered high. This is a simple guide to be taken with a grain of salt.</summary>
			float strength = 0.f;

			/// <summary>Issues with the password.</summary>
			/// <remarks>These issues suggest improvement areas.</remarks>
			std::vector<std::string> issues;

			/// <summary>A summary of the password issues.</summary>
			std::string issues_summary;
		};

		/// <summary>Determine the quality of a password.</summary>
		/// <param name="password">The password string.</param>
		/// <returns>The quality of the password as defined in <see cref='password_quality_specs'></see>.</returns>
		password_quality_specs leccore_api password_quality(const std::string& password);

		/// <summary>Image render quality. Used when resampling images.</summary>
		enum class image_quality {
			/// <summary>Low image quality, for faster rendering.</summary>
			low,

			/// <summary>Medium image quality, for a balance between quality and rendering speed.</summary>
			medium,

			/// <summary>High image quality, for best image quality possibly at the cost of rendering speed.</summary>
			high,
		};

		/// <summary>Size of a rectangular structure.</summary>
		class leccore_api size {
			float _width = 0.f;
			float _height = 0.f;

		public:
			/// <summary>Size constructor. Initializes with a size of 0.f x 0.f.</summary>
			size();

			/// <summary>Size constructor.</summary>
			/// <param name="width">The width of the rectangular structure, in pixels.</param>
			/// <param name="height">The height of the rectangular structure, in pixels.</param>
			size(const float width, const float height);

			/// <summary>Get or set the width of the rectangular structure, in pixels.</summary>
			/// <returns>A reference to the property.</returns>
			float& width();

			/// <summary>Get or set the width of the rectangular structure, in pixels.</summary>
			/// <returns>The width, in pixels.</returns>
			float get_width() const;

			/// <summary>Set the width of a rectangular structure, in pixels.</summary>
			/// <param name="width">The width, in pixels.</param>
			/// <returns>A reference to the modified object.</returns>
			size& width(const float& width);

			/// <summary>Get or set the height of the rectangular structure, in pixels.</summary>
			/// <returns>A reference to the property.</returns>
			float& height();

			/// <summary>Get or set the height of the rectangular structure, in pixels.</summary>
			/// <returns>The height, in pixels.</returns>
			float get_height() const;

			/// <summary>Set the height of a rectangular structure, in pixels.</summary>
			/// <param name="height">The height, in pixels.</param>
			/// <returns>A reference to the modified object.</returns>
			size& height(const float& height);
		};
	}
}
