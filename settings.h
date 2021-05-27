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

			settings();
			virtual ~settings();

			virtual bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) = 0;
			virtual bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) = 0;
			virtual bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) = 0;
			virtual bool delete_recursive(const std::string& branch,
				std::string& error) = 0;

		private:
			// Copying an object of this class is not allowed
			settings(const settings&) = delete;
			settings& operator=(const settings&) = delete;
		};

		class leccore_api registry_settings : public settings {
		public:
			registry_settings(registry::scope settings_scope);
			registry_settings(registry::scope settings_scope,
				const std::string& key, const std::string& iv);
			~registry_settings();

			void set_registry_path(const std::string& registry_path);
			bool get_registry_path(std::string& registry_path,
				std::string& error);

			bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) override;
			bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) override;
			bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) override;
			bool delete_recursive(const std::string& branch,
				std::string& error) override;

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed
			registry_settings() = delete;
			registry_settings(const registry_settings&) = delete;
			registry_settings& operator=(const registry_settings&) = delete;
		};

		class leccore_api ini_settings : public settings {
		public:
			ini_settings(const std::string& file_name);
			ini_settings(const std::string& file_name,
				const std::string& key, const std::string& iv);
			~ini_settings();

			void set_ini_path(const std::string& ini_path);
			bool get_ini_path(std::string& ini_path,
				std::string& error);

			bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) override;
			bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) override;
			bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) override;
			bool delete_recursive(const std::string& branch,
				std::string& error) override;

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed
			ini_settings() = delete;
			ini_settings(const ini_settings&) = delete;
			ini_settings& operator=(const ini_settings&) = delete;
		};
	}
}
