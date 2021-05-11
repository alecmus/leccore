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
public:
	sqlite3* db_;

	impl() :
		db_(nullptr) {}
	~impl() {
		if (db_) {
			sqlite3_close(db_);
			db_ = nullptr;
		}
	}

	std::string sqlite_error() {
		if (db_) {
			std::string error = sqlite3_errmsg(db_);
			if (error == "not an error") error.clear();
			if (error.length() > 0) error[0] = toupper(error[0]);
			return error;
		}
		else {
			return "Database not open";
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
	if (connected()) return true;

	int error_code = 0;
	if (password_.empty()) {
		error_code = sqlite3_open_v2(connection_string_.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}
	else {
		error_code = sqlite3_open_v2(connection_string_.c_str(), &d_.db_,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);

		sqlite3_stmt* stm = nullptr;
		const char* pzTail = nullptr;

		// key the database
		auto pragma_string = "PRAGMA key = '" + password_ + "';";
		sqlite3_prepare(d_.db_, pragma_string.c_str(), -1, &stm, &pzTail);
		sqlite3_step(stm);
		sqlite3_finalize(stm);

		// test if key is correct
		error_code = sqlite3_exec(d_.db_, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);

		if (error_code == SQLITE_NOTADB) {
			error = d_.sqlite_error() + " or password is incorrect";

			// close database
			sqlite3_close(d_.db_);
			d_.db_ = nullptr;
			return false;
		}
	}

	if (error_code != SQLITE_OK) {
		// an error occured
		error = d_.sqlite_error();

		// close database
		sqlite3_close(d_.db_);
		d_.db_ = nullptr;
		return false;
	}

	set_connected(true);
	return true;
}

bool sqlcipher_connection::disconnect(std::string& error) {
	if (!d_.db_)
		return true;

	if (sqlite3_close(d_.db_) != SQLITE_OK) {
		error = d_.sqlite_error();
		return false;
	}

	d_.db_ = nullptr;
	return true;
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
