//
// connection_base.h - database connection base interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#include "../database.h"

namespace liblec {
	namespace leccore {
		namespace database {
			class connection_base {
			public:
				const std::string connection_string_;
				const std::string password_;

				connection_base(const std::string& connection_string, const std::string& password);
				virtual ~connection_base();

				bool connected();
				void set_connected(bool connected);

				virtual bool connect(std::string& error) = 0;
				virtual bool disconnect(std::string& error) = 0;
				virtual bool execute(const std::string& sql, const std::vector<std::any>& values, std::string& error) = 0;
				virtual bool execute_query(const std::string& sql, table& results, std::string& error) = 0;

			private:
				bool connected_;
			};
		}
	}
}
