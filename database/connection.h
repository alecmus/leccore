//
// connection.h - database connection interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#if defined(LECCORE_EXPORTS)
#include "../leccore.h"
#else
#include <liblec/leccore.h>
#endif

#include <any>
#include <map>
#include <string>
#include <vector>

namespace liblec {
	namespace leccore {
		namespace database {
			using blob = struct { std::string data; };
			using row = std::map<std::string, std::any>;
			using table = struct {
				std::string name;
				std::vector<std::string> columns;
				std::vector<row> data;
			};

			class leccore_api get {
			public:
				static int integer(const std::any& value);
				static double real(const std::any& value);
				static std::string text(const std::any& value);
				static blob blob(const std::any& value);
			};

			class leccore_api connection {
			public:
				connection(const std::string& type,
					const std::string& connection_string,
					const std::string& password);
				~connection();

				bool connected();
				bool connect(std::string& error);
				bool disconnect(std::string& error);
				bool execute(const std::string& sql, const std::vector<std::any>& values, std::string& error);
				bool execute_query(const std::string& sql, table& results, std::string& error);

			private:
				class impl;
				impl& d_;

				// Default constructor and copying an object of this class are not allowed
				connection() = delete;
				connection(const connection&) = delete;
				connection& operator=(const connection&) = delete;
			};
		}
	}
}
