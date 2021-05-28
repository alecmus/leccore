//
// file.h - file handling helper interface
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
		/// <summary>File handling helper class.</summary>
		class leccore_api file {
		public:
			/// <summary>Read <see cref="data"></see> from a file.</summary>
			/// <param name="file_path">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="data">The contents of the file, whether text or binary.</param>
			/// <param name="error">Error information</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool read(const std::string& file_path,
				std::string& data,
				std::string& error);

			/// <summary>Write <see cref="data"></see> to a file.</summary>
			/// <param name="file_path">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="data">The data to save to the file, whether text or binary.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			/// <remarks>If the file already exists all contents will be overwritten.</remarks>
			[[nodiscard]]
			static bool write(const std::string& file_path,
				const std::string& data,
				std::string& error);

			/// <summary>Remove (delete) a file.</summary>
			/// <param name="file_path">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool remove(const std::string& file_path,
				std::string& error);
		};
	}
}
