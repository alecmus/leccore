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
		/// <summary>Application settings base class. This class is for
		/// saving/loading application settings.</summary>
		class leccore_api settings {
		public:
			settings();
			virtual ~settings();

			/// <summary>Write a <see cref="value"></see> to the settings file.</summary>
			/// <param name="branch">The branch under which the <see cref="value_name"></see> is
			/// under, e.g. "Email Options". Use an empty string if it's in the root path.</param>
			/// <param name="value_name">The value name, e.g. "SMTP Port".</param>
			/// <param name="value">The value to be written under the <see cref="value_name"></see>,
			/// e.g. 587.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			virtual bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) = 0;

			/// <summary>Read a <see cref="value"></see> from the settings file.</summary>
			/// <param name="branch">The branch under which the <see cref="value_name"></see> is,
			/// e.g. "Email Options". Use an empty string if it's in the root path.</param>
			/// <param name="value_name">The value name, e.g. "SMTP Port".</param>
			/// <param name="value">The value read from the settings.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			virtual bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) = 0;

			/// <summary>Delete a <see cref="value_name"></see> from the settings file.</summary>
			/// <param name="branch">The branch under which the <see cref="value_name"></see> is,
			/// e.g. "Email Options".</param>
			/// <param name="value_name">The value name, e.g. "SMTP Port".</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			virtual bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) = 0;

			/// <summary>Delete a branch and all it's contents recursively.</summary>
			/// <param name="branch">The branch to delete from the settings,
			/// e.g. "Email Options". Use an empty string to delete the root path and
			/// all it's contents.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			virtual bool delete_recursive(const std::string& branch,
				std::string& error) = 0;

			/// <summary>Get a reference to the base class.</summary>
			/// <returns>A reference to the base class.</returns>
			/// <remarks>Useful for use in the ternary operator, e.g. if r is and instance of
			/// <see cref="registry_settings"></see> and i is an instance of <see cref="inisettings"></see>
			/// then one can initialize a settings reference s by calling s = condition ? r : i.</remarks>
			settings& base();

		private:
			// Copying an object of this class is not allowed
			settings(const settings&) = delete;
			settings& operator=(const settings&) = delete;
		};

		/// <summary>Registry settings derived class. For saving/loading application settings
		/// using the Windows registry.</summary>
		class leccore_api registry_settings : public settings {
		public:
			/// <summary>Plain registry settings constructor.</summary>
			/// <param name="settings_scope">The registry scope as defined in the
			/// <see cref="scope"></see> enumeration.</param>
			/// <remarks>This constructor is for plain registry settings (not encrypted).</remarks>
			registry_settings(registry::scope settings_scope);

			/// <summary>Encrypted registry settings constructor.</summary>
			/// <param name="settings_scope">The registry scope as defined in the
			/// <see cref="scope"></see> enumeration.</param>
			/// <param name="key">The encryption key. This must be kept private.</param>
			/// <param name="iv">The initialization vector. This must be unique.</param>
			/// <remarks>This constructor is for encrypted registry settings; implemented
			/// using 256bit AES encryption.</remarks>
			registry_settings(registry::scope settings_scope,
				const std::string& key, const std::string& iv);
			~registry_settings();

			/// <summary>Set the registry path, relative to the scope defined in the constructor.</summary>
			/// <param name="registry_path">The root path to use for the registry settings, e.g.
			/// "Software\\com.github.alecmus\\PC Info.</param>
			/// <remarks>If this method is not called the registry path is set internally to
			/// "Software\\company-name\\app-name" as read from the application's version information.
			/// The <see cref="app_version_info"></see> class is called internally for this. If the
			/// current application does not have any version information then all write and read
			/// functions will return an error until a path is explicitly defined using this method.
			/// </remarks>
			void set_registry_path(const std::string& registry_path);

			/// <summary>Get the currently set registry path.</summary>
			/// <param name="registry_path">The registry path, relative to the scope defined in
			/// the constructor.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			/// <remarks>If <see cref="set_registry_path"></see> is not called then the registry path is
			/// internally set to "Software\\company-name\\app-name" as read from the application's
			/// version information. The <see cref="app_version_info"></see> class is called internally
			/// for this. If the current application does not have any version information then this
			/// method will return an error.</remarks>
			[[nodiscard]]
			bool get_registry_path(std::string& registry_path,
				std::string& error);

			// virtual method overrides

			[[nodiscard]]
			bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) override;
			[[nodiscard]]
			bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) override;
			[[nodiscard]]
			bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) override;
			[[nodiscard]]
			bool delete_recursive(const std::string& branch,
				std::string& error) override;

		private:
			class impl;
			impl& _d;

			// Default constructor and copying an object of this class are not allowed
			registry_settings() = delete;
			registry_settings(const registry_settings&) = delete;
			registry_settings& operator=(const registry_settings&) = delete;
		};

		/// <summary>Settings derived class that uses ini files. For saving/loading application settings
		/// using ini configuration files.</summary>
		class leccore_api ini_settings : public settings {
		public:
			/// <summary>Plain ini settings constructor.</summary>
			/// <param name="file_name">The name of the settings ini file, e.g. "app_settings.cfg". If an
			/// empty string is passed the file name defaults internally to "config.ini".</param>
			/// <remarks>This constructor is for plain ini settings (not encrypted).</remarks>
			ini_settings(const std::string& file_name);

			/// <summary>Encrypted ini settings constructor.</summary>
			/// <param name="file_name">The name of the settings ini file, e.g. "app_settings.cfg". If an
			/// empty string is passed the file name defaults internally to "config.ini".</param>
			/// <param name="key">The encryption key. This must be kept private.</param>
			/// <param name="iv">The initialization vector. This must be unique.</param>
			/// <remarks>This constructor is for encrypted ini settings; implemented
			/// using 256bit AES.</remarks>
			ini_settings(const std::string& file_name,
				const std::string& key, const std::string& iv);
			~ini_settings();
			
			/// <summary>Set the folder under which the ini file is to be saved, e.g.
			/// "C:\Users\user-name\AppData\Local\com.github.alecmus\PC Info".</summary>
			/// <param name="ini_path">The path to the folder the ini file is to be saved in. Use an
			/// empty string to store the file in the same directory as the application (useful when
			/// running the application in portable-mode).</param>
			/// <remarks>If this method is not called the ini path is set internally to
			/// "C:\Users\user-name\AppData\Local\company-name\app-name" as read from the application's
			/// version information. The <see cref="app_version_info"></see> class is called internally
			/// for this. If the current application does not have any version information then this
			/// method will return an error.</remarks>
			void set_ini_path(const std::string& ini_path);

			/// <summary>Get the currently set ini path.</summary>
			/// <param name="ini_path">The path to the folder the ini file is to be stored in.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			/// <remarks>If the <see cref="set_ini_path"></see> method is not called then the ini path
			/// is set internally to "C:\Users\user-name\AppData\Local\company-name\app-name" as read
			/// from the application's version information. The <see cref="app_version_info"></see>
			/// class is called internally for this. If the current application does not have any
			/// version information then this method will return an error.</remarks>
			[[nodiscard]]
			bool get_ini_path(std::string& ini_path,
				std::string& error);

			// virtual method overrides

			[[nodiscard]]
			bool write_value(const std::string& branch,
				const std::string& value_name,
				const std::string& value,
				std::string& error) override;
			[[nodiscard]]
			bool read_value(const std::string& branch,
				const std::string& value_name,
				std::string& value,
				std::string& error) override;
			[[nodiscard]]
			bool delete_value(const std::string& branch,
				const std::string& value_name,
				std::string& error) override;
			[[nodiscard]]
			bool delete_recursive(const std::string& branch,
				std::string& error) override;

		private:
			class impl;
			impl& _d;

			// Default constructor and copying an object of this class are not allowed
			ini_settings() = delete;
			ini_settings(const ini_settings&) = delete;
			ini_settings& operator=(const ini_settings&) = delete;
		};
	}
}
