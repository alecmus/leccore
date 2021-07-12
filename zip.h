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
		/// <summary>Class for zipping files and folders into a zip archive.</summary>
		class leccore_api zip {
		public:
			zip();
			~zip();

			/// <summary>The compression level to use.</summary>
			enum class compression_level {
				/// <summary>Balance between size and speed. The default.</summary>
				normal,

				/// <summary>For the smallest archive size, but takes longer.</summary>
				maximum,

				/// <summary>Compromises compression level in favor of speed.</summary>
				fast,

				/// <summary>Lowest compression level for super fast zipping.</summary>
				superfast,

				/// <summary>No compression. Use this for the ultimate speed when
				/// you don't need any compression and just want to store files.</summary>
				none,
			};

			/// <summary>Start zipping operation.</summary>
			/// <param name="filename">The target filename, including the extension.</param>
			/// <param name="entries">The archive entries (files, directories).</param>
			/// <param name="level">The compression level, as defined in the
			/// <see cref="compression_level"></see> enumeration.</param>
			/// <remarks>This method returns almost immediately. The actual zipping is executed
			/// on a seperate thread. To check the status of the zipping call the
			/// <see cref="zipping"></see> method.</remarks>
			void start(const std::string& filename,
				const std::vector<std::string>& entries,
				compression_level level = compression_level::normal);

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
			impl& _d;

			// Copying an object of this class is not allowed
			zip(const zip&) = delete;
			zip& operator=(const zip&) = delete;
		};

		/// <summary>Class for unzipping a zip archive.</summary>
		class leccore_api unzip {
		public:
			unzip();
			~unzip();

			/// <summary>Unzip log.</summary>
			using unzip_log = struct {
				std::vector<std::string> message_list;
				std::vector<std::string> error_list;
			};

			/// <summary>Start zipping operation.</summary>
			/// <param name="filename">The full path to the zip archive, including the extension.</param>
			/// <param name="directory">The directory to extract the zip archive to. Use an empty string to
			/// extract to the current directory.</param>
			/// <remarks>This method returns almost immediately. The actual unzipping is executed
			/// on a seperate thread. To check the status of the unzipping call the
			/// <see cref="unzipping"></see> method.</remarks>
			void start(const std::string& filename,
				const std::string& directory);

			/// <summary>Check whether the unzipping operation is still underway.</summary>
			/// <returns>Returns true if the unzipping is still underway, else false.</returns>
			/// <remarks>After calling <see cref="start"></see> call this method in a loop or a timer, depending on
			/// your kind of app, then call <see cref="result"></see> once it returns false.</remarks>
			bool unzipping();

			/// <summary>The result of the unzipping operation.</summary>
			/// <param name="log">Unzip log as defined in the <see cref="unzip_log"></see> type. This may contain
			/// error messages for individual entries regardless of whether the method returns true or false. For example
			/// if one of the files already existed and was marked as read-only the unzipping operation continues
			/// and this method will return true yet an error will have actually occurred for that one entry.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation was successful, else false. When false the
			/// error information is written back to <see cref="error"></see>.</returns>
			bool result(unzip_log& log, std::string& error);

		private:
			class impl;
			impl& _d;

			// Copying an object of this class is not allowed
			unzip(const unzip&) = delete;
			unzip& operator=(const unzip&) = delete;
		};
	}
}
