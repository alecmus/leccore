//
// connection_base.h - database connection base implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "connection_base.h"

using namespace liblec::leccore::database;

connection_base::connection_base(const std::string& connection_string,
	const std::string& password) :
	connected_(false),
	connection_string_(connection_string),
	password_(password) {}

connection_base::~connection_base() {
	connected_ = false;
}

bool connection_base::connected() {
	return connected_;
}

void connection_base::set_connected(bool connected) {
	connected_ = connected;
}