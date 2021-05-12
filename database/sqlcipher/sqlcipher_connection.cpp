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
	const std::vector<std::any>& values,
	std::string& error) {
	if (!d_.db_) {
		error = "Database not open";
		return false;
	}

	// prepare statement
	sqlite3_stmt* statement = nullptr;
	if (sqlite3_prepare(d_.db_, sql.c_str(), -1, &statement, 0) == SQLITE_OK) {
		// get number of bind parameters
		const int bind_parameter_count = sqlite3_bind_parameter_count(statement);

		if (bind_parameter_count == 0) {
			int result = sqlite3_step(statement);
			if (result == SQLITE_DONE)
				result = SQLITE_OK;

			sqlite3_finalize(statement);

			if (result != SQLITE_OK) {
				error = d_.sqlite_error();
				return false;
			}

			return true;
		}

		if (bind_parameter_count != values.size()) {
			error = "Expected " + std::to_string(bind_parameter_count) + " values but " + std::to_string(values.size()) + " supplied";
			sqlite3_finalize(statement);
			return false;
		}

		// do some binding
		// supported types: int, float, double, text(const char*, std::string), blob(database::blob)
		// 
		std::vector<blob> blob_data;	// to keep blob data until sqlite3_step() is executed
		int index = 1;
		for (auto& value : values) {
			if (value.has_value()) {
				// bind integers (int)
				if (value.type() == typeid(int)) {
					const auto integer = std::any_cast<int>(value);

					if (sqlite3_bind_int(statement, index, integer) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind floats (float)
				if (value.type() == typeid(float)) {
					const auto f = std::any_cast<float>(value);

					if (sqlite3_bind_double(statement, index, f) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind doubles (double)
				if (value.type() == typeid(double)) {
					const auto d = std::any_cast<double>(value);

					if (sqlite3_bind_double(statement, index, d) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (const char*)
				if (value.type() == typeid(const char*)) {
					const char* buffer = std::any_cast<const char*>(value);
					const auto length = (int)strlen(buffer);

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (std::string)
				if (value.type() == typeid(std::string)) {
					const std::string data = std::any_cast<std::string>(value);
					const char* buffer = data.c_str();
					const auto length = (int)data.length();

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind blob (database::blob)
				if (value.type() == typeid(blob)) {
					blob_data.push_back(std::any_cast<blob>(value));
					const std::string& data = blob_data.back().get();
					const char* buffer = data.c_str();
					const auto size = (int)data.length();

					if (sqlite3_bind_blob(statement, index, buffer, size, SQLITE_STATIC) != SQLITE_OK) {
						error = d_.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}
			}

			index++;
		}

		int result = sqlite3_step(statement);
		if (result == SQLITE_DONE)
			result = SQLITE_OK;

		sqlite3_finalize(statement);

		if (result != SQLITE_OK) {
			error = d_.sqlite_error();
			return false;
		}

		return true;
	}
	else {
		error = d_.sqlite_error();
		return false;
	}
}

bool sqlcipher_connection::execute_query(const std::string& sql, table& results, std::string& error) {
	if (!d_.db_) {
		error = "Database not open";
		return false;
	}

	results.name.clear();
	results.columns.clear();
	results.data.clear();

	// prepare statement
	sqlite3_stmt* statement = nullptr;

	if (sqlite3_prepare_v2(d_.db_, sql.c_str(), -1, &statement, 0) == SQLITE_OK) {
		const int columns = sqlite3_column_count(statement);
		int result = SQLITE_OK;

		while (true) {
			result = sqlite3_step(statement);

			if (result == SQLITE_ROW) {
				row current_row;
				bool first_row = results.columns.empty();

				for (int column = 0; column < columns; column++) {
					std::string column_name;

					// get column name
					char* ccColumn = (char*)sqlite3_column_name(statement, column);

					if (ccColumn) {
						column_name = ccColumn;

						// get table name
						if (results.name.empty()) {
							char* ccTable = (char*)sqlite3_column_table_name(statement, column);
							if (ccTable) results.name = ccTable;
						}

						// get column type
						switch (sqlite3_column_type(statement, column)) {
						case SQLITE_INTEGER: {
							current_row.insert(std::make_pair(column_name, sqlite3_column_int(statement, column)));
						} break;
						case SQLITE_FLOAT: {
							current_row.insert(std::make_pair(column_name, sqlite3_column_double(statement, column)));
						} break;
						case SQLITE_TEXT: {
							char* ccData = (char*)sqlite3_column_text(statement, column);
							std::string value;
							if (ccData) value = ccData;
							current_row.insert(std::make_pair(column_name, value));
						} break;
						case SQLITE_BLOB: {
							auto read_blob = [&](char** buffer, int* size)->void {
								*size = sqlite3_column_bytes(statement, column);
								*buffer = (char*)malloc(*size);
								memcpy(*buffer, sqlite3_column_blob(statement, column), *size);
							};

							int size;
							char* buffer;

							read_blob(&buffer, &size);
							std::string value(buffer, size);
							blob b(value);
							current_row.insert(std::make_pair(column_name, b));

							free(buffer);
						}break;

						case SQLITE_NULL:
						default:
							break;
						}

						if (first_row)
							results.columns.push_back(column_name);
					}
				}

				results.data.push_back(current_row);
			}
			else
				break;
		}

		if (result == SQLITE_DONE)
			result = SQLITE_OK;

		sqlite3_finalize(statement);

		if (result != SQLITE_OK) {
			error = d_.sqlite_error();
			return false;
		}
	}
	else {
		error = d_.sqlite_error();
		return false;
	}

	return true;
}
