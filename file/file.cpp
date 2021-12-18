//
// file.cpp - file handling helper implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../file.h"
#include <fstream>
#include <filesystem>
#include <Windows.h>
using namespace std;
using namespace liblec::leccore;

bool liblec::leccore::file::read(const string& fullpath,
	string& data,
	string& error) {
	error.clear();
	data.clear();
	try {
		// make a path object for the file
		filesystem::path path(fullpath);

		// check if the file exists
		if (!filesystem::exists(path)) {
			error = fullpath + " does not exist";
			return false;
		}

		// verify that it's a file
		if (!filesystem::is_regular_file(path)) {
			error = fullpath + " is not a file";
			return false;
		}

		// open the file
		ifstream file(fullpath, ios::binary);

		if (!file || !file.is_open()) {
			error = "Opening file failed";
			return false;
		}

		// compute file size
		file.unsetf(ios::skipws);

		file.seekg(0, ios::end);
		const auto file_size = (long)file.tellg();
		file.seekg(0, ios::beg);

		// dynamically allocate memory for a buffer that matches the file size
		char* buffer = new char[file_size];

		try {
			// read the file into the buffer and close the file
			file.read(buffer, file_size);
			file.close();

			// write back the data to the caller
			data = string(buffer, file_size);

			// free the dynamically allocated memory
			delete[] buffer;
			return true;
		}
		catch (const std::exception& e) {
			// free the dynamically allocated memory
			delete[] buffer;
			return true;

			error = e.what();
			return false;
		}
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}

bool liblec::leccore::file::write(const string& fullpath,
	const string& data,
	string& error) {
	error.clear();
	try {
		// make a path object for the file
		filesystem::path path(fullpath);

		// check if the file exists
		if (filesystem::exists(path)) {
			// verify that it's a file
			if (!filesystem::is_regular_file(path)) {
				error = fullpath + " is not a file";
				return false;
			}

			// check if the file is read-only
			DWORD attributes = GetFileAttributesA(fullpath.c_str());
			if (attributes != INVALID_FILE_ATTRIBUTES) {
				if (attributes & FILE_ATTRIBUTE_READONLY) {
					error = "File is read-only";
					return false;
				}
			}
		}

		// open the file
		ofstream file(fullpath, ios::out | ios::trunc | ios::binary);

		if (!file || !file.is_open()) {
			error = "Opening destination file failed";
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

bool file::remove(const string& fullpath, string& error) {
	error.clear();
	try {
		// make a path object for the file
		filesystem::path path(fullpath);

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

bool file::create_directory(const std::string& fullpath,
	std::string& error) {
	try {
		std::filesystem::path path(fullpath);

		if (filesystem::exists(path)) {
			if (filesystem::is_directory(path))
				return true;
			else {
				error = fullpath + " is not a directory";
				return false;
			}
		}

		filesystem::create_directories(path);
		return true;
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}

bool file::remove_directory(const std::string& fullpath,
	std::string& error) {
	try {
		std::filesystem::path path(fullpath);

		if (filesystem::exists(path) && !filesystem::is_directory(path)) {
			error = fullpath + " is not a directory";
			return false;
		}

		filesystem::remove_all(path);
		return true;
	}
	catch (const exception& e) {
		error = e.what();
		return false;
	}
}

bool file::rename(const std::string& fullpath,
	const std::string& new_name, std::string& error) {
	error.clear();
	try {
		// make a path object for the file
		filesystem::path path(fullpath);

		// check if the path exists
		if (!filesystem::exists(path)) {
			error = "Invalid path: " + fullpath;
			return false;
		}

		auto new_path = path;
		new_path.replace_filename(new_name);
		filesystem::rename(path, new_path);
		return true;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}
