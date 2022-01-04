//
// connection.cpp - database connection implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../database.h"
#include "connection_base.h"
#include "sqlcipher/sqlcipher_connection.h"

using namespace liblec::leccore::database;

int get::integer(const std::any& value) {
	return std::any_cast<int>(value);
}

double get::real(const std::any& value) {
	if (value.type() == typeid(float))
		return std::any_cast<float>(value);
	else
		return std::any_cast<double>(value);
}

std::string get::text(const std::any& value) {
	if (value.type() == typeid(const char*))
		return std::string(std::any_cast<const char*>(value));
	else
		return std::any_cast<std::string>(value);
}

blob liblec::leccore::database::get::blob(const std::any& value) {
	return std::any_cast<database::blob>(value);
}

class connection::impl {
public:
	impl(const std::string& type,
		const std::string& connection_string,
		const std::string& password) :
		_type(type),
		_connection_string(connection_string),
		_password(password) {

		if (type == "sqlcipher")
			_p_db = new sqlcipher_connection(_connection_string, password);
		else
			_p_db = new sqlcipher_connection(_connection_string, password);
	}
	~impl() {
		if (_p_db) {
			delete _p_db;
			_p_db = nullptr;
		}
	}

	const std::string _type;
	const std::string _connection_string;
	const std::string _password;
	connection_base* _p_db;
};

connection::connection(const std::string& type,
	const std::string& connection_string,
	const std::string& password) :
	_d(*new impl(type, connection_string, password)) {}

connection::~connection() {
	delete& _d;
}

bool connection::connected() {
	if (_d._p_db)
		return _d._p_db->connected();
	else
		return false;
}

bool connection::connect(std::string& error) {
	error.clear();
	if (_d._p_db)
		return _d._p_db->connect(error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}

bool connection::disconnect(std::string& error) {
	error.clear();
	if (_d._p_db)
		return _d._p_db->disconnect(error);
	else
		return true;
}

bool connection::execute(const std::string& sql,
	const std::vector<std::any>& values,
	std::string& error) {
	error.clear();
	if (_d._p_db)
		return _d._p_db->execute(sql, values, error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}

bool connection::execute_query(const std::string& sql, const std::vector<std::any>& values,
	table& results, std::string& error) {
	error.clear();
	if (_d._p_db)
		return _d._p_db->execute_query(sql, values, results, error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}
