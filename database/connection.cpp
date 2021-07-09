//
// connection.cpp - database connection implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
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
		type_(type),
		connection_string_(connection_string),
		password_(password) {

		if (type == "sqlcipher")
			p_db_ = new sqlcipher_connection(connection_string_, password);
		else
			p_db_ = new sqlcipher_connection(connection_string_, password);
	}
	~impl() {
		if (p_db_) {
			delete p_db_;
			p_db_ = nullptr;
		}
	}

	const std::string type_;
	const std::string connection_string_;
	const std::string password_;
	connection_base* p_db_;
};

connection::connection(const std::string& type,
	const std::string& connection_string,
	const std::string& password) :
	d_(*new impl(type, connection_string, password)) {}

connection::~connection() {
	delete& d_;
}

bool connection::connected() {
	if (d_.p_db_)
		return d_.p_db_->connected();
	else
		return false;
}

bool connection::connect(std::string& error) {
	error.clear();
	if (d_.p_db_)
		return d_.p_db_->connect(error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}

bool connection::disconnect(std::string& error) {
	error.clear();
	if (d_.p_db_)
		return d_.p_db_->disconnect(error);
	else
		return true;
}

bool connection::execute(const std::string& sql,
	const std::vector<std::any>& values,
	std::string& error) {
	error.clear();
	if (d_.p_db_)
		return d_.p_db_->execute(sql, values, error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}

bool connection::execute_query(const std::string& sql,
	table& results,
	std::string& error) {
	error.clear();
	if (d_.p_db_)
		return d_.p_db_->execute_query(sql, results, error);
	else {
		error = "liblec::leccore::database::connection - initialization error";
		return false;
	}
}
