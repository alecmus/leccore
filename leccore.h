//
// leccore.h - leccore interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
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
#include <sstream>

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

		void leccore_api sleep(unsigned long long milliseconds);
	}
}
