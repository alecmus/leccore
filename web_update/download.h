//
// download.h - download interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include <string>

namespace liblec {
	namespace leccore {
		class download_sink {
		public:
			virtual void set_length(size_t len) = 0;
			virtual void set_filename(const std::string& filename) = 0;
			virtual void add_chunk(const void* data, size_t len) = 0;
		};

		class string_download_sink : public download_sink {
		public:
			std::string data;
			virtual void set_length (size_t) override {}
			virtual void set_filename(const std::string&) override {}
			virtual void add_chunk(const void* data, size_t len) override {
				this->data.append(reinterpret_cast<const char*>(data), len);
			}
		};

		bool download(const std::string& url,
			download_sink& sink, bool cache_data, std::string& error);
	}
}
