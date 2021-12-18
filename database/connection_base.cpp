//
// connection_base.h - database connection base implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "connection_base.h"

using namespace liblec::leccore::database;

connection_base::connection_base(const std::string& connection_string,
	const std::string& password) :
	_connected(false),
	_connection_string(connection_string),
	_password(password) {}

connection_base::~connection_base() {
	_connected = false;
}

bool connection_base::connected() {
	return _connected;
}

void connection_base::set_connected(bool connected) {
	_connected = connected;
}
