//
// registry.h - registry interface
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

namespace liblec {
	namespace leccore {
		class leccore_api registry {
		public:
			enum class scope { current_user, local_machine };

			registry(scope registry_scope);
			~registry();

			bool do_read(const std::string& path,
				const std::string& value_name,
				std::string& value,
				std::string& error);
			bool do_write(const std::string& path,
				const std::string& value_name,
				const std::string& value,
				std::string& error);
			bool do_delete(const std::string& path,
				const std::string& value_name,
				std::string& error);
			void do_recursive_delete(const std::string& sub_key);

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed
			registry() = delete;
			registry(const registry&) = delete;
			registry& operator==(const registry&) = delete;
		};
	}
}
