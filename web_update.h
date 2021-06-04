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
		class leccore_api check_update {
		public:
			check_update(const std::string& appcast_url);
			~check_update();

			using appcast = struct {
				std::string version;
				std::string short_version;
				std::string download_url;
				std::string title;
				std::string description;
				std::string release_notes;
				std::string operating_system;
			};

			void start();
			bool checking();
			bool result(appcast& details,
				std::string& error);

		private:
			class impl;
			impl& d_;

			check_update() = delete;
			check_update(const check_update&) = delete;
			check_update& operator=(const check_update&) = delete;
		};

		class leccore_api download_update {
		public:
			download_update();
			~download_update();

			using download_info = struct {
				unsigned long long file_size;
				unsigned long long downloaded;
			};

			void start(const std::string& url,
				const std::string& directory);
			bool downloading();
			bool downloading(download_info& progress);
			bool result(std::string& error);

		private:
			class impl;
			impl& d_;
		};
	}
}
