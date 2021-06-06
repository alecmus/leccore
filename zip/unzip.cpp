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
	std::string filename_;
	std::string directory_;
	unzip_log log_;

	struct unzip_result {
		bool success = false;
		std::string error;
	};

	std::future<unzip_result> fut_;

	impl() {}
	~impl() {}

	void on_error(const void*, std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string>& info) {
		log_.error_list.push_back(info.second);
	}

	void on_ok(const void*, std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>& info) {
		log_.message_list.push_back("Extracting: " + info.second.toString(Poco::Path::PATH_UNIX));
		std::string path = directory_ + info.second.toString(Poco::Path::PATH_WINDOWS);

		// set file last modified time
		Poco::File file(path);

		Poco::DateTime dt(info.first.lastModifiedAt());
		Poco::Timestamp::UtcTimeVal utc = dt.utcTime();
		Poco::Timestamp ts = dt.timestamp();
		file.setLastModified(ts);

		// to-do: set file attributes
	}

	static unzip_result unzip_func(impl* p_impl) {
		impl& d_ = *p_impl;

		unzip_result result = {};

		if (d_.filename_.empty()) {
			result.error = "File not specified";
			result.success = false;
			return result;
		}

		try {
			if (!d_.directory_.empty()) {
				std::filesystem::path path(d_.directory_);
				if (std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
					result.error = "Invalid output directory";
					result.success = false;
					return result;
				}
			}

			if (!d_.directory_.empty()) {
				// add trailing slash if it's missing
				if (d_.directory_[d_.directory_.length() - 1] != '\\')
					d_.directory_ += "\\";

				std::filesystem::path path(d_.directory_);
				std::filesystem::create_directories(path);
			}

			std::ifstream in(d_.filename_, std::ios::binary);

			if (!in.is_open()) {
				result.error = "Error opening file";
				result.success = false;
				return result;
			}

			Poco::Zip::Decompress decompress(in, d_.directory_);

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

unzip::unzip() : d_(*new impl()) {}
unzip::~unzip() {
	if (d_.fut_.valid())
		d_.fut_.get();

	delete& d_;
}

void unzip::start(const std::string& filename,
	const std::string& directory) {
	if (unzipping()) {
		// allow only one instance
		return;
	}

	d_.filename_ = filename;
	d_.directory_ = directory;
	d_.log_ = {};

	// run task asynchronously
	d_.fut_ = std::async(std::launch::async, d_.unzip_func, &d_);
	return;
}

bool unzip::unzipping() {
	if (d_.fut_.valid())
		return d_.fut_.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
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

	if (d_.fut_.valid()) {
		auto result = d_.fut_.get();
		error = result.error;
		log = d_.log_;
		return result.success;
	}

	error = "unexpected error";
	log = d_.log_;
	return false;
}
