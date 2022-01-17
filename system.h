//
// system.h - system interface
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
#include <vector>

namespace liblec {
	namespace leccore {
		/// <summary>Class for getting the path to known user folders.</summary>
		/// <remarks>If the path doesn't exist, an attempt will be made to create it.
		/// An empty string is returned if there is an error.
		/// </remarks>
		class leccore_api user_folder {
		public:
			/// <summary>Get the user's home directory.</summary>
			/// <returns>The home directory.</returns>
			static const std::string home();

			/// <summary>Get the user's document directory.</summary>
			/// <returns>The documents directory.</returns>
			static const std::string documents();

			/// <summary>Get the user's downloads directory.</summary>
			/// <returns>The downloads directory.</returns>
			static const std::string downloads();

			/// <summary>Get the user's desktop directory.</summary>
			/// <returns>The desktop directory.</returns>
			static const std::string desktop();

			/// <summary>Get the user's music directory.</summary>
			/// <returns>The music directory.</returns>
			static const std::string music();

			/// <summary>Get the user's videos directory.</summary>
			/// <returns>The videos directory.</returns>
			static const std::string videos();

			/// <summary>Get the user's pictures directory.</summary>
			/// <returns>The pictures directory.</returns>
			static const std::string pictures();

			/// <summary>Get the user's local appdata directory.</summary>
			/// <returns>The appdata directory.</returns>
			static const std::string local_appdata();

			/// <summary>Get the path to the folder designated for temporary files.</summary>
			/// <returns>The temporary files directory.</returns>
			static const std::string temp();
		};

		/// <summary>Command line arguments parser.</summary>
		class leccore_api commandline_arguments {
		public:
			/// <summary>Get command line arguments.</summary>
			/// <returns>The list of arguments/tokens.</returns>
			static const std::vector<std::string>& get();

			/// <summary>Check whether a given token is in the list of arguments.</summary>
			/// <param name="token">The token to check.</param>
			/// <returns>Returns true if the token is in the list, else false.</returns>
			static bool contains(const std::string& token);

		private:
			class impl;
		};

		/// <summary>Shell helper class.</summary>
		class leccore_api shell {
		public:
			/// <summary>Open a resource using the Operating System default action, equivalent to a double click action.</summary>
			/// <param name="path">The path to the resource, e.g. "C:\", "C:\Files\MyFile.pdf" or "https://github.com/alecmus/leccore".</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation is successful, else false.</returns>
			static const bool open(const std::string& path, std::string& error);

			/// <summary>Create a process.</summary>
			/// <param name="fullpath">The full path to the executable file.</param>
			/// <param name="args">The list of command-line arguments.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation is successful, else false.</returns>
			static const bool create_process(const std::string& fullpath,
				const std::vector<std::string>& args, std::string& error);
		};

		/// <summary>Clipboard class.</summary>
		class leccore_api clipboard {
		public:
			/// <summary>Set clipboard text.</summary>
			/// <param name="text">The text to copy to the clipboard.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation is successful, else false.</returns>
			static const bool set_text(const std::string& text, std::string& error);

			/// <summary>Get text from the clipboard.</summary>
			/// <param name="text">The text copied from the clipboard.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the operation is successful, else false.</returns>
			static const bool get_text(std::string& text, std::string& error);
		};
	}
}
