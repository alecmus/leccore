//
// sqlcipher_connection.h - sqlcipher database connection interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#include "../connection_base.h"

namespace liblec {
	namespace leccore {
		namespace database {
			class sqlcipher_connection : public connection_base {
			public:
				sqlcipher_connection(const std::string& file_name,
					const std::string& password);
				~sqlcipher_connection();

				bool connect(std::string& error) override;
				bool disconnect(std::string& error) override;
				bool execute(const std::string& sql, const std::vector<std::any>& values, std::string& error) override;
				bool execute_query(const std::string& sql, const std::vector<std::any>& values, table& results, std::string& error) override;

			private:
				class impl;
				impl& _d;
			};
		}
	}
}
