//
// web_update.h - web update interface
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
		/// <summary>Check for an updated version of your app on the web.</summary>
		class leccore_api check_update {
		public:
			/// <summary>The constructor.</summary>
			/// <param name="update_xml_url">The url to the update XML file.</param>
			check_update(const std::string& update_xml_url);
			~check_update();

			/// <summary>Update details.</summary>
			using update_info = struct {
				/// <summary>The title of the update, e.g. PC Info 1.0.0 beta 3.</summary>
				std::string title;

				/// <summary>The description of the update, e.g. Third beta release.</summary>
				std::string description;

				/// <summary>The update version, e.g. 1.0.0 beta 3.</summary>
				std::string version;

				/// <summary>The release date.</summary>
				std::string date;

				/// <summary>The url of the update file.</summary>
				std::string download_url;

				/// <summary>The size of the file.</summary>
				unsigned long long size;

				/// <summary>The sha256 hash of the file.</summary>
				std::string hash;
			};

			/// <summary>Start checking for updates.</summary>
			/// <remarks>This method returns almost immediately. The actual checking is executed
			/// on a different thread. To check the status of the checking use the <see cref="checking"></see>
			/// method.</remarks>
			void start();

			/// <summary>Check whether updates are being checked for [sic] :)</summary>
			/// <returns>Returns true if the update checking process is still underway and false otherwise.</returns>
			/// <remarks>After calling <see cref="start"></see> call this method in a loop or a timer, depending on
			/// your kind of app, then call <see cref="result"></see> once it returns false.</remarks>
			bool checking();

			/// <summary>Get the results of the update checking operation.</summary>
			/// <param name="details">The update information as defined in
			/// the <see cref="update_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if update information was fetched successfully, else false. When false the
			/// error information is written back to <see cref="error"></see>.</returns>
			bool result(update_info& details,
				std::string& error);

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed.
			check_update() = delete;
			check_update(const check_update&) = delete;
			check_update& operator=(const check_update&) = delete;
		};

		/// <summary>Download an update file from the web.</summary>
		class leccore_api download_update {
		public:
			download_update();
			~download_update();

			/// <summary>Details about the download.</summary>
			using download_info = struct {
				unsigned long long file_size;
				unsigned long long downloaded;
			};

			/// <summary>Start downloading the update file.</summary>
			/// <param name="url">The file's url.</param>
			/// <param name="directory">The directory to download the file to. Leave empty to download to
			/// the same directory as the app.</param>
			/// <remarks>This method returns almost immediately. The actual file download is executed on
			/// a seperate thread.</remarks>
			void start(const std::string& url,
				const std::string& directory);

			/// <summary>Check whether the file download is in progress.</summary>
			/// <returns>Returns true if the file download is in progress, else false.</returns>
			/// <remarks>After calling <see cref="start"></see> calling this method in a loop or a timer,
			/// depending on your app, will help determine when the download is completed, after which the
			/// method will return false.</remarks>
			bool downloading();

			/// <summary>Check whether the file download is in progress.</summary>
			/// <param name="progress">Information about the download progress as defined in
			/// <see cref="download_info"></see>.</param>
			/// <returns>Returns true if the file download is in progress, else false.</returns>
			/// <remarks>After calling <see cref="start"></see> calling this method in a loop or a timer,
			/// depending on your app, will help determine when the download is completed, after which the
			/// method will return false. This overload is also useful if you intend to use a progress bar
			/// do indicate download progress. However, take note that the file_size parameter in
			/// <see cref="download_info"></see> may be zero during part or all of the download process.
			/// </remarks>
			bool downloading(download_info& progress);

			/// <summary>Get the result of the download operation.</summary>
			/// <param name="full_path">the full path to the downloaded file.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the file was downloaded successfully, else false.</returns>
			bool result(std::string& full_path,
				std::string& error);

		private:
			class impl;
			impl& d_;

			// Copying an object of this class is not allowed
			download_update(const download_update&) = delete;
			download_update& operator=(const download_update&) = delete;
		};
	}
}
