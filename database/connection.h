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
			class leccore_api blob {
			public:
				blob(std::string& data);
				~blob();
				blob(const blob& obj);
				std::string& get();

			private:
				class blob_impl;
				blob_impl& d_;
			};

			enum class data_type {
				integer = 0,
				real,
				text,
				blob,
				null,
			};

			struct column {
				std::string name;
				data_type type;
			};

			using row = std::map<std::string, std::any>;

			struct table {
				std::string name;
				std::vector<column> columns;
				std::vector<row> data;
			};

			class leccore_api connection {
			public:
				connection(const std::string& type,
					const std::string& connection_string,
					const std::string& password);
				~connection();

				bool connected();
				bool connect(std::string& error);
				bool execute(const std::string& sql, const std::vector<std::any>& values, std::string& error);
				bool execute_query(const std::string& sql, table& results, std::string& error);

			private:
				class impl;
				impl& d_;
			};
		}
	}
}
