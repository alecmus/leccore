//
// settings.h - settings interface
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
#include "registry.h"
#else
#include <liblec/leccore.h>
#include <liblec/leccore/registry.h>
#endif

#include <string>

namespace liblec {
	namespace leccore {
		class leccore_api settings {
		public:
			using byte = unsigned char;

			settings(registry::scope settings_scope);
			settings(registry::scope settings_scope,
				const std::string& key, const std::string& iv);
			~settings();

			void set_registry_path(const std::string& registry_path);
			bool get_registry_path(std::string& registry_path,
				std::string& error);

			bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error);
			bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error);
			bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error);
			bool delete_recursive(const std::string& branch,
				std::string& error);

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed
			settings() = delete;
			settings(const settings&) = delete;
			settings& operator=(const settings&) = delete;
		};
	}
}