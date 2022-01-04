//
// sqlcipher_connection.cpp - sqlcipher database connection implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
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
	sqlite3* _db;

	impl() :
		_db(nullptr) {}
	~impl() {
		if (_db) {
			sqlite3_close(_db);
			_db = nullptr;
		}
	}

	std::string sqlite_error() {
		if (_db) {
			std::string error = sqlite3_errmsg(_db);
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
	_d(*new impl()) {}

sqlcipher_connection::~sqlcipher_connection() {
	delete& _d;
}

bool sqlcipher_connection::connect(std::string& error) {
	error.clear();
	if (connected()) return true;

	int error_code = 0;
	if (_password.empty()) {
		error_code = sqlite3_open_v2(_connection_string.c_str(), &_d._db,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
		error_code = sqlite3_exec(_d._db, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);
	}
	else {
		error_code = sqlite3_open_v2(_connection_string.c_str(), &_d._db,
			SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);

		sqlite3_stmt* stm = nullptr;
		const char* pzTail = nullptr;

		// key the database
		auto pragma_string = "PRAGMA key = '" + _password + "';";
		sqlite3_prepare(_d._db, pragma_string.c_str(), -1, &stm, &pzTail);
		sqlite3_step(stm);
		sqlite3_finalize(stm);

		// test if key is correct
		error_code = sqlite3_exec(_d._db, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL);

		if (error_code == SQLITE_NOTADB) {
			error = _d.sqlite_error() + " or password is incorrect";

			// close database
			sqlite3_close(_d._db);
			_d._db = nullptr;
			return false;
		}
	}

	if (error_code != SQLITE_OK) {
		// an error occured
		error = _d.sqlite_error();

		// close database
		sqlite3_close(_d._db);
		_d._db = nullptr;
		return false;
	}

	set_connected(true);
	return true;
}

bool sqlcipher_connection::disconnect(std::string& error) {
	error.clear();
	if (!_d._db)
		return true;

	if (sqlite3_close(_d._db) != SQLITE_OK) {
		error = _d.sqlite_error();
		return false;
	}

	_d._db = nullptr;
	set_connected(false);
	return true;
}

bool sqlcipher_connection::execute(const std::string& sql,
	const std::vector<std::any>& values,
	std::string& error) {
	error.clear();
	if (!_d._db) {
		error = "Database not open";
		return false;
	}

	// check for unsupported types
	for (const auto& value : values) {
		if (value.type() != typeid(int) &&
			value.type() != typeid(float) &&
			value.type() != typeid(double) &&
			value.type() != typeid(const char*) &&
			value.type() != typeid(std::string) &&
			value.type() != typeid(blob)
			) {
			error = "Unsupported type: " + std::string(value.type().name());
			return false;
		}
	}

	// prepare statement
	sqlite3_stmt* statement = nullptr;
	if (sqlite3_prepare(_d._db, sql.c_str(), -1, &statement, 0) == SQLITE_OK) {
		// get number of bind parameters
		const int bind_parameter_count = sqlite3_bind_parameter_count(statement);

		if (bind_parameter_count != values.size()) {
			error = "Expected " + std::to_string(bind_parameter_count) + " values but " + std::to_string(values.size()) + " supplied";
			sqlite3_finalize(statement);
			return false;
		}

		// do some binding
		// supported types: int, float, double, text(const char*, std::string), blob(database::blob)
		// 
		std::map<int, std::string> buffer_data;	// to keep buffer data until sqlite3_step() is executed
		int index = 1;
		for (auto& value : values) {
			if (value.has_value()) {
				// bind integers (int)
				if (value.type() == typeid(int)) {
					auto integer = std::any_cast<int>(value);

					if (sqlite3_bind_int(statement, index, integer) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind floats (float)
				if (value.type() == typeid(float)) {
					auto f = std::any_cast<float>(value);

					if (sqlite3_bind_double(statement, index, f) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind doubles (double)
				if (value.type() == typeid(double)) {
					auto d = std::any_cast<double>(value);

					if (sqlite3_bind_double(statement, index, d) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (const char*)
				if (value.type() == typeid(const char*)) {
					auto buffer = std::any_cast<const char*>(value);
					auto length = (int)strlen(buffer);

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (std::string)
				if (value.type() == typeid(std::string)) {
					buffer_data[index] = std::any_cast<std::string>(value);	// make a copy
					std::string& data = buffer_data.at(index);	// refer to the copy, not the source
					char* buffer = (char*)data.c_str();
					auto length = (int)data.length();

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind blob (database::blob)
				if (value.type() == typeid(blob)) {
					buffer_data[index] = std::any_cast<blob>(value).data;	// make a copy
					std::string& data = buffer_data.at(index);	// refer to the copy, not the source
					char* buffer = (char*)data.c_str();
					auto size = (int)data.length();

					if (sqlite3_bind_blob(statement, index, buffer, size, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
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
			error = _d.sqlite_error();
			return false;
		}

		return true;
	}
	else {
		error = _d.sqlite_error();
		return false;
	}
}

bool sqlcipher_connection::execute_query(const std::string& sql, const std::vector<std::any>& values, table& results, std::string& error) {
	error.clear();
	results.name.clear();
	results.columns.clear();
	results.data.clear();

	if (!_d._db) {
		error = "Database not open";
		return false;
	}

	// check for unsupported types
	for (const auto& value : values) {
		if (value.type() != typeid(int) &&
			value.type() != typeid(float) &&
			value.type() != typeid(double) &&
			value.type() != typeid(const char*) &&
			value.type() != typeid(std::string) &&
			value.type() != typeid(blob)
			) {
			error = "Unsupported type: " + std::string(value.type().name());
			return false;
		}
	}

	// prepare statement
	sqlite3_stmt* statement = nullptr;

	if (sqlite3_prepare_v2(_d._db, sql.c_str(), -1, &statement, 0) == SQLITE_OK) {
		// get number of bind parameters
		const int bind_parameter_count = sqlite3_bind_parameter_count(statement);

		if (bind_parameter_count != values.size()) {
			error = "Expected " + std::to_string(bind_parameter_count) + " values but " + std::to_string(values.size()) + " supplied";
			sqlite3_finalize(statement);
			return false;
		}

		// do some binding
		// supported types: int, float, double, text(const char*, std::string), blob(database::blob)
		// 
		std::map<int, std::string> buffer_data;	// to keep buffer data until sqlite3_step() is executed
		int index = 1;
		for (auto& value : values) {
			if (value.has_value()) {
				// bind integers (int)
				if (value.type() == typeid(int)) {
					auto integer = std::any_cast<int>(value);

					if (sqlite3_bind_int(statement, index, integer) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind floats (float)
				if (value.type() == typeid(float)) {
					auto f = std::any_cast<float>(value);

					if (sqlite3_bind_double(statement, index, f) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind doubles (double)
				if (value.type() == typeid(double)) {
					auto d = std::any_cast<double>(value);

					if (sqlite3_bind_double(statement, index, d) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (const char*)
				if (value.type() == typeid(const char*)) {
					auto buffer = std::any_cast<const char*>(value);
					auto length = (int)strlen(buffer);

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind text (std::string)
				if (value.type() == typeid(std::string)) {
					buffer_data[index] = std::any_cast<std::string>(value);	// make a copy
					std::string& data = buffer_data.at(index);	// refer to the copy, not the source
					char* buffer = (char*)data.c_str();
					auto length = (int)data.length();

					if (sqlite3_bind_text(statement, index, buffer, length, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}

				// bind blob (database::blob)
				if (value.type() == typeid(blob)) {
					buffer_data[index] = std::any_cast<blob>(value).data;	// make a copy
					std::string& data = buffer_data.at(index);	// refer to the copy, not the source
					char* buffer = (char*)data.c_str();
					auto size = (int)data.length();

					if (sqlite3_bind_blob(statement, index, buffer, size, SQLITE_STATIC) != SQLITE_OK) {
						error = _d.sqlite_error();
						sqlite3_finalize(statement);
						return false;
					}
				}
			}

			index++;
		}

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
							blob b{ value };
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
			error = _d.sqlite_error();
			return false;
		}
	}
	else {
		error = _d.sqlite_error();
		return false;
	}

	return true;
}
