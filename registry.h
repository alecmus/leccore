//
// registry.h - registry interface
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
		/// <summary>Class for reading and writing to the Windows registry.</summary>
		class leccore_api registry {
		public:
			/// <summary>The registry scope.</summary>
			enum class scope {
				/// <summary>Settings for the currently logged in user only.</summary>
				current_user,
				/// <summary>Settings for everyone that uses this computer.</summary>
				/// <remarks>The application must be executed with administrator rights
				/// to use this scope otherwise access will be denied.</remarks>
				local_machine
			};

			/// <summary>The constructor.</summary>
			/// <param name="registry_scope">The registry scope as defined in the
			/// <see cref="scope"></see> enumeration.</param>
			registry(scope registry_scope);
			~registry();

			/// <summary>Read from the registry.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="value_name">The value name, e.g. "Color"</param>
			/// <param name="value">The value read from the registry.</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			[[nodiscard]]
			bool do_read(const std::string& path,
				const std::string& value_name,
				std::string& value,
				std::string& error);

			/// <summary>Read binary data from the registry.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="value_name">The value name, e.g. "Installation ID"</param>
			/// <param name="data">The raw binary data read from the registry.</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			[[nodiscard]]
			bool do_read_binary(const std::string& path,
				const std::string& value_name,
				std::string& data,
				std::string& error);

			/// <summary>Write data to the registry.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="value_name">The value name, e.g. "Color"</param>
			/// <param name="value">The value to be written to the registry
			/// under <see cref="value_name"></see>, e.g. "#ffff00"</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			[[nodiscard]]
			bool do_write(const std::string& path,
				const std::string& value_name,
				const std::string& value,
				std::string& error);

			/// <summary>Write binary data to the registry.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="value_name">The value name, e.g. "Installation ID"</param>
			/// <param name="data">The raw binary data to be written to the registry
			/// under <see cref="value_name"></see>.</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			[[nodiscard]]
			bool do_write_binary(const std::string& path,
				const std::string& value_name,
				const std::string& data,
				std::string& error);

			/// <summary>Delete a <see cref="value_name"></see> from the registry.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="value_name">The value name, e.g. "Installation ID"</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			[[nodiscard]]
			bool do_delete(const std::string& path,
				const std::string& value_name,
				std::string& error);

			/// <summary>Delete a sub-key if it's empty.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			/// <remarks>Only deletes the sub-key if it's empty. If the sub-key is not empty
			/// an access is denied error is returned. This is a useful method to call on the
			/// Software\company-name sub-key after deleting a specific application's sub-key, e.g.
			/// after deleting the sub-key Software\com.github.alecmus\PC Info using the
			/// <see cref="do_recursive_delete"></see> it's a great idea to then call this method. If
			/// no settings for other applications are stored under the Software\com.github.alecmus
			/// subkey then it is deleted as well, leaving a clean registry. This is highly
			/// recommended as it leaves a clean registry after all apps for a given company
			/// are uninstalled.</remarks>
			[[nodiscard]]
			bool do_delete(const std::string& path,
				std::string& error);

			/// <summary>Delete a sub-key and everything contained within it,
			/// including other sub-keys.</summary>
			/// <param name="path">The path, relative to the scope as defined in the
			/// <see cref="scope"></see> enumeration, e.g. Software\com.github.alecmus\PC Info</param>
			/// <param name="error">Error information.</param>
			/// <returns>True if successful, else false.</returns>
			/// <remarks>Use with care as the changes are irreversible.</remarks>
			[[nodiscard]]
			bool do_recursive_delete(const std::string& path,
				std::string& error);

		private:
			class impl;
			impl& _d;

			// Default constructor and copying an object of this class are not allowed
			registry() = delete;
			registry(const registry&) = delete;
			registry& operator=(const registry&) = delete;
		};
	}
}
