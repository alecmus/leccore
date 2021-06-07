//
// parse_update_xml.cpp - parse update xml implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "parse_update_xml.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

bool read_value(const std::string& xml,
	const std::string& value_name, std::string& value, std::string& error) {
	error.clear();
	value.clear();

	try{
		std::stringstream ss;
		ss << xml;
		boost::property_tree::ptree pt;
		boost::property_tree::read_xml(ss, pt);
		value = pt.get(value_name, std::string());
		return true;
	}
	catch (const std::exception& e){
		error = e.what();
		return false;
	}

	return true;
}

bool liblec::leccore::parse_update_xml(const std::string& xml,
	liblec::leccore::check_update::update_info& details,
	std::string& error) {
	error.clear();
	details = {};

	check_update::update_info details_ = {};
	if (!read_value(xml, "leccore.update.title", details_.title, error))
		return false;

	if (!read_value(xml, "leccore.update.description", details_.description, error))
		return false;

	if (!read_value(xml, "leccore.update.version", details_.version, error))
		return false;

	if (!read_value(xml, "leccore.update.date", details_.date, error))
		return false;

	std::string architecture;
#ifdef _WIN64
	architecture = "x64";
#else
	architecture = "x86";
#endif

	if (!read_value(xml, "leccore.update." + architecture + ".download_url", details_.download_url, error))
		return false;

	std::string size;
	if (!read_value(xml, "leccore.update." + architecture + ".size", size, error))
		return false;
	else {
		try {
			std::stringstream ss;
			ss << size;
			ss >> details_.size;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}

	if (!read_value(xml, "leccore.update." + architecture + ".hash.sha256", details_.hash, error))
		return false;

	details = details_;
	return true;
}
