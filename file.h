//
// file.h - file handling helper interface
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
		/// <summary>File handling helper class.</summary>
		class leccore_api file {
		public:
			/// <summary>Read <see cref="data"></see> from a file.</summary>
			/// <param name="fullpath">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="data">The contents of the file, whether text or binary.</param>
			/// <param name="error">Error information</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool read(const std::string& fullpath,
				std::string& data,
				std::string& error);

			/// <summary>Write <see cref="data"></see> to a file.</summary>
			/// <param name="fullpath">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="data">The data to save to the file, whether text or binary.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			/// <remarks>If the file already exists all contents will be overwritten.</remarks>
			[[nodiscard]]
			static bool write(const std::string& fullpath,
				const std::string& data,
				std::string& error);

			/// <summary>Remove (delete) a file.</summary>
			/// <param name="fullpath">The full path to the file, including the
			/// file's name and extension.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool remove(const std::string& fullpath,
				std::string& error);

			/// <summary>Create a directory path, recursively.</summary>
			/// <param name="fullpath">The full path to the directory.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool create_directory(const std::string& fullpath,
				std::string& error);

			/// <summary>Remove a directory, recursively.</summary>
			/// <param name="fullpath">The full path to the directory.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			/// <remarks>Use with care. This method will attempt to delete
			/// the directory and all of its contents, including sub-directories.
			/// </remarks>
			[[nodiscard]]
			static bool remove_directory(const std::string& fullpath,
				std::string& error);

			/// <summary>Rename a file or directory.</summary>
			/// <param name="fullpath">The full path to the file or directory, including the
			/// file's name and extension in the case of a file.</param>
			/// <param name="new_name">The new name, including the extension in the case of
			/// a file.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool rename(const std::string& fullpath,
				const std::string& new_name,
				std::string& error);

			/// <summary>Copy a file or directory.</summary>
			/// <param name="fullpath">The full path to the file or directory, including the
			/// file's name and extension in the case of a file.</param>
			/// <param name="new_name">The new name, including the extension in the case of
			/// a file.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			static bool copy(const std::string& fullpath,
				const std::string& new_name,
				std::string& error);

			/// <summary>Exclusive file lock class.</summary>
			/// <remarks>Only one instance can execute the lock, all others have to wait until that
			/// one instance releases the lock by going out of scope.</remarks>
			class leccore_api exclusive_lock {
			public:
				/// <summary>Constructor.</summary>
				/// <param name="full_path">The full path to the lock file, e.g. "C:\Users\...\my_folder\lock_file.some_extension."</param>
				exclusive_lock(const std::string& full_path);

				/// <summary>Destructor.</summary>
				/// <remarks>Releases the lock (if locked) and deletes the lock file.</remarks>
				virtual ~exclusive_lock();

				/// <summary>Exclusively lock a file.</summary>
				/// <param name="error">Error information.</param>
				/// <returns>Returns true if successful, else false.</returns>
				/// <remarks>Once a lock is executed successfully only destroying the object can release it.</remarks>
				bool lock(std::string& error);

			private:
				class impl;
				impl& _d;

				// Default constructor and copying an object of this class are not allowed
				exclusive_lock() = delete;
				exclusive_lock(const exclusive_lock&) = delete;
				exclusive_lock& operator=(const exclusive_lock&) = delete;
			};
		};
	}
}
