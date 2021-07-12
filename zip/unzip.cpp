//
// unzip.cpp - unzip implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../zip.h"
#include <thread>
#include <future>
#include <fstream>
#include <filesystem>

#define POCO_STATIC

#include <Poco/Zip/Decompress.h>
#include <Poco/Zip/ZipLocalFileHeader.h>
#include <Poco/Zip/ZipArchive.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Delegate.h>

using namespace liblec::leccore;

class unzip::impl {
public:
	std::string _filename;
	std::string _directory;
	unzip_log _log;

	struct unzip_result {
		bool success = false;
		std::string error;
	};

	std::future<unzip_result> _fut;

	impl() {}
	~impl() {}

	void on_error(const void*, std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string>& info) {
		_log.error_list.push_back(info.second);
	}

	void on_ok(const void*, std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>& info) {
		_log.message_list.push_back("Extracting: " + info.second.toString(Poco::Path::PATH_UNIX));
		std::string path = _directory + info.second.toString(Poco::Path::PATH_WINDOWS);

		// set file last modified time
		Poco::File file(path);

		Poco::DateTime dt(info.first.lastModifiedAt());
		Poco::Timestamp::UtcTimeVal utc = dt.utcTime();
		Poco::Timestamp ts = dt.timestamp();
		file.setLastModified(ts);

		// to-do: set file attributes
	}

	static unzip_result unzip_func(impl* p_impl) {
		impl& _d = *p_impl;

		unzip_result result = {};

		if (_d._filename.empty()) {
			result.error = "File not specified";
			result.success = false;
			return result;
		}

		try {
			if (!_d._directory.empty()) {
				std::filesystem::path path(_d._directory);
				if (std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
					result.error = "Invalid output directory";
					result.success = false;
					return result;
				}
			}

			if (!_d._directory.empty()) {
				// add trailing slash if it's missing
				if (_d._directory[_d._directory.length() - 1] != '\\')
					_d._directory += "\\";

				std::filesystem::path path(_d._directory);
				std::filesystem::create_directories(path);
			}

			std::ifstream in(_d._filename, std::ios::binary);

			if (!in.is_open()) {
				result.error = "Error opening file";
				result.success = false;
				return result;
			}

			Poco::Zip::Decompress decompress(in, _d._directory);

			decompress.EError += Poco::Delegate<unzip::impl,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string> >(p_impl, &unzip::impl::on_error);
			decompress.EOk += Poco::Delegate<unzip::impl,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>>(p_impl, &unzip::impl::on_ok);

			decompress.decompressAllFiles();

			decompress.EError -= Poco::Delegate<unzip::impl,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string> >(p_impl, &unzip::impl::on_error);
			decompress.EOk -= Poco::Delegate<unzip::impl,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path> >(p_impl, &unzip::impl::on_ok);

			result.success = true;
			return result;
		}
		catch (Poco::Exception& e) {
			result.error = e.displayText();
			result.success = false;
			return result;
		}
		catch (const std::exception& e) {
			result.error = e.what();
			result.success = false;
			return result;
		}
	}
};

unzip::unzip() : _d(*new impl()) {}
unzip::~unzip() {
	if (_d._fut.valid())
		_d._fut.get();

	delete& _d;
}

void unzip::start(const std::string& filename,
	const std::string& directory) {
	if (unzipping()) {
		// allow only one instance
		return;
	}

	_d._filename = filename;
	_d._directory = directory;
	_d._log = {};

	// run task asynchronously
	_d._fut = std::async(std::launch::async, _d.unzip_func, &_d);
	return;
}

bool unzip::unzipping() {
	if (_d._fut.valid())
		return _d._fut.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool unzip::result(unzip_log& log, std::string& error) {
	error.clear();
	log = {};

	if (unzipping()) {
		error = "Task not yet complete";
		return false;
	}

	if (_d._fut.valid()) {
		auto result = _d._fut.get();
		error = result.error;
		log = _d._log;
		return result.success;
	}

	error = "unexpected error";
	log = _d._log;
	return false;
}
