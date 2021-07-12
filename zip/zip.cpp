//
// zip.cpp - zip implementation
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

#include <Poco\Zip\Compress.h>
#include <Poco\Delegate.h>
#include <Poco\File.h>

// cater for GetAdaptersInfo used in PocoFoundation static lib
#pragma comment(lib, "iphlpapi.lib")

using namespace liblec::leccore;

class zip::impl {
public:
	std::string _filename;
	std::vector<std::string> _entries;
	compression_level _level = compression_level::normal;
	bool _add_root;

	struct zip_result {
		bool success = false;
		std::string error;
	};

	std::future<zip_result> _fut;

	impl() :
		_add_root(true) {}
	~impl() {}

	static zip_result zip_func(impl* p_impl) {
		impl& _d = *p_impl;

		zip_result result = {};

		if (_d._filename.empty()) {
			result.error = "Destination file not specified";
			result.success = false;
			return result;
		}

		if (_d._entries.empty()) {
			result.error = "Zip archive entries not specified";
			result.success = false;
			return result;
		}

		try {
			// add trailing slashes to directories (if missing)
			for (auto& it : _d._entries) {
				std::filesystem::path path(it);
				if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
					if (it[it.length() - 1] != '\\')
						it += "\\";
				}
			}

			Poco::File file(_d._filename);

			if (file.exists() && !file.canWrite()) {
				result.error = "File cannot be written to";
				result.success = false;
				return result;
			}

			std::ofstream out(_d._filename, std::ios::binary | std::ios::trunc);
			Poco::Zip::Compress compress(out, true);

			for (const auto& it : _d._entries) {
				Poco::File file(it);

				if (file.exists()) {
					Poco::Path entry(it);

					Poco::Zip::ZipCommon::CompressionLevel level;

					switch (_d._level) {
						break;
					case liblec::leccore::zip::compression_level::maximum:
						level = Poco::Zip::ZipCommon::CompressionLevel::CL_MAXIMUM;
						break;
					case liblec::leccore::zip::compression_level::fast:
						level = Poco::Zip::ZipCommon::CompressionLevel::CL_FAST;
						break;
					case liblec::leccore::zip::compression_level::superfast:
						level = Poco::Zip::ZipCommon::CompressionLevel::CL_SUPERFAST;
						break;
					case liblec::leccore::zip::compression_level::normal:
					case liblec::leccore::zip::compression_level::none:
					default:
						level = Poco::Zip::ZipCommon::CompressionLevel::CL_NORMAL;
						break;
					}

					Poco::Zip::ZipCommon::CompressionMethod method =
						_d._level == compression_level::none ?
						Poco::Zip::ZipCommon::CompressionMethod::CM_STORE : Poco::Zip::ZipCommon::CompressionMethod::CM_DEFLATE;

					if (entry.isDirectory()) {
						entry.makeDirectory();
						const bool add_root = _d._add_root ? true : _d._entries.size() > 1;
						compress.addRecursive(entry, level, !add_root);
					}
					else {
						entry.makeFile();
						compress.addFile(entry, entry, method, level);
					}
				}
			}

			compress.close();
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

zip::zip() : _d(*new impl()) {}
zip::~zip() {
	if (_d._fut.valid())
		_d._fut.get();

	delete& _d;
}

void zip::start(const std::string& filename,
	const std::vector<std::string>& entries,
	compression_level level) {
	if (zipping()) {
		// allow only one instance
		return;
	}

	_d._filename = filename;
	_d._entries = entries;
	_d._level = level;

	// run task asynchronously
	_d._fut = std::async(std::launch::async, _d.zip_func, &_d);
	return;
}

bool zip::zipping() {
	if (_d._fut.valid())
		return _d._fut.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool zip::result(std::string& error) {
	error.clear();

	if (zipping()) {
		error = "Task not yet complete";
		return false;
	}

	if (_d._fut.valid()) {
		auto result = _d._fut.get();
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
