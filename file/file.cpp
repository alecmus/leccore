//
// file.cpp - file handling helper implementation
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
		// open the file
		ifstream file(file_path, ios::binary);

		if (!file) {
			error = "Error reading file";
			return false;
		}

		// compute file size
		file.unsetf(ios::skipws);

		file.seekg(0, ios::end);
		const auto file_size = (long)file.tellg();
		file.seekg(0, ios::beg);

		// dynamically allocate memory for a buffer that matches the file size
		char* buffer = new char[file_size];

		// read the file into the buffer and close the file
		file.read(buffer, file_size);
		file.close();

		// write back the data to the caller
		data = string(buffer, file_size);

		// free the dynamically allocated memory
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
		// open the file
		ofstream file(file_path, ios::out | ios::trunc | ios::binary);

		if (!file) {
			error = "Error opening destination file";
			return false;
		}

		// write data to file and close the file
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
		// make a path object for the file
		filesystem::path path(file_path);

		// verify that it's a file
		if (!filesystem::is_regular_file(path))
			return true;

		// remove the file
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
