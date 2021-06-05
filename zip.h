//
// zip.h - zip interface
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
#include <vector>

namespace liblec {
	namespace leccore {
		/// <summary>Class for making zip files.</summary>
		class leccore_api zip {
		public:
			zip();
			~zip();

			/// <summary>Start zipping operation.</summary>
			/// <param name="filename">The target filename, including the extension.</param>
			/// <param name="entries">The archive entries (files, directories).</param>
			/// <remarks>This method returns almost immediately. The actual zipping is executed
			/// on a seperate thread. To check the status of the zipping call the
			/// <see cref="zipping"></see> method.</remarks>
			void start(const std::string& filename,
				const std::vector<std::string>& entries);

			/// <summary>Check whether the zipping operation is still underway.</summary>
			/// <returns>Returns true if the zipping is still underway, else false.</returns>
			/// <remarks>After calling <see cref="start"></see> call this method in a loop or a timer, depending on
			/// your kind of app, then call <see cref="result"></see> once it returns false.</remarks>
			bool zipping();

			/// <summary>The result of the zipping operation.</summary>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation was successful, else false. When false the
			/// error information is written back to <see cref="error"></see>.</returns>
			bool result(std::string& error);

		private:
			class impl;
			impl& d_;
		};
	}
}
