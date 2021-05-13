//
// file.cpp - file helper implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../file.h"
#include <fstream>
#include <filesystem>
using namespace std;
using namespace liblec::leccore;

bool liblec::leccore::file::read(const string& file_path,
	string& data,
	string& error) {
	try {
		ifstream file(file_path, ios::binary);

		if (!file) {
			error = "Error reading file";
			return false;
		}

		file.unsetf(ios::skipws);

		file.seekg(0, ios::end);
		const auto file_size = (long)file.tellg();
		file.seekg(0, ios::beg);

		char* buffer = new char[file_size];
		file.read(buffer, file_size);
		file.close();

		data = string(buffer, file_size);

		delete[]buffer;
		return true;
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}

bool liblec::leccore::file::write(const string& file_path,
	const string& data,
	string& error) {
	try {
		ofstream file(file_path, ios::out | ios::trunc | ios::binary);

		if (!file) {
			error = "Error opening destination file";
			return false;
		}

		file.write(data.c_str(), data.length());
		file.close();
		return true;
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}

bool file::remove(const string& file_path, string& error) {
	try {
		filesystem::path path(file_path);

		if (!filesystem::is_regular_file(path))
			return true;

		error_code code;
		if (!filesystem::remove(path, code)) {
			error = code.message();
			return false;
		}

		return true;
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}
