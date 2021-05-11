//
// sqlcipher_connection.cpp - sqlcipher database connection implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "sqlcipher_connection.h"
#include <sqlite3.h>

#ifdef _WIN64
#pragma comment(lib, "sqlcipher64.lib")
#else
#pragma comment(lib, "sqlcipher32.lib")
#endif

using namespace liblec::leccore::database;

class sqlcipher_connection::impl {
	sqlite3* db_;

public:
	impl() :
		db_(nullptr) {}
	~impl() {
		if (db_) {
			sqlite3_close(db_);
			db_ = nullptr;
		}
	}
};

sqlcipher_connection::sqlcipher_connection(const std::string& file_name,
	const std::string& password) :
	connection_base(file_name, password),
	d_(*new impl()) {}

sqlcipher_connection::~sqlcipher_connection() {
	delete& d_;
}

bool sqlcipher_connection::connect(std::string& error) {
	error = "liblec::leccore::database::sqlcipher_connection::connect - not yet implemented";
	return false;
}

bool sqlcipher_connection::disconnect(std::string& error) {
	error = "liblec::leccore::database::sqlcipher_connection::disconnect - not yet implemented";
	return false;
}

bool sqlcipher_connection::execute(const std::string& sql,
	const std::vector<value>& values,
	std::string& error) {
	error = "liblec::leccore::database::sqlcipher_connection::execute - not yet implemented";
	return false;
}

bool sqlcipher_connection::execute_query(const std::string& sql, table& results, std::string& error) {
	error = "liblec::leccore::database::sqlcipher_connection::execute_query - not yet implemented";
	return false;
}
