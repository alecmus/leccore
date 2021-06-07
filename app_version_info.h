//
// app_version_info.h - application version information interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
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
		/// <summary>Compare two version strings.</summary>
		/// <param name="left">The left side version number, e.g. 1.0.0 alpha 2.</param>
		/// <param name="right">The right side version number, e.g. 1.0.1.</param>
		/// <returns>Returns 1 if the version number on the left is greater,
		/// -1 if it's smaller, and 0 if the two are the same.</returns>
		/// <remarks>Users dots as delimiters, and also splits up the version number
		/// once the character type changes, e.g. 1.0.0 alpha 2 is split into
		/// [1][0][0][alpha][2].</remarks>
		int leccore_api compare_versions(const std::string& left,
			const std::string& right);

		class leccore_api app_version_info {
		public:
			app_version_info();
			app_version_info(const std::string& app_name,
				const std::string& app_version,
				const std::string& company_name);

			bool get_app_name(std::string& app_name,
				std::string& error);
			bool get_app_version(std::string& app_version,
				std::string& error);
			bool get_company_name(std::string& company_name,
				std::string& error);

		private:
			class impl;
			impl& d_;

			// Copying an object of this class is not allowed
			app_version_info(const app_version_info&) = delete;
			app_version_info& operator=(const app_version_info&) = delete;
		};
	}
}
