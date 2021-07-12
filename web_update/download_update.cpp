//
// download_update.cpp - download web update implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../web_update.h"
#include "../leccore_common.h"
#include "download.h"
#include <thread>
#include <future>
#include <time.h>

using namespace liblec::leccore;

class download_update::impl {
public:
	struct download_update_result {
		bool success = false;
		std::string error;
		std::string fullpath;
	};

	std::string _url;
	std::string _directory;
	std::future<download_update_result> _fut;
	
	download_info _progress;
	mutex _progress_mutex;

	impl() :
		_progress({ 0, 0 }) {}
	~impl() {}

	class update_download_sink : public download_sink {
		FILE* _p_file;
		const std::string& _directory;
		std::string _fullpath;
		size_t _downloaded, _total;
		clock_t _last_update;

		download_update::download_info& _progress;
		mutex& _progress_mutex;

	public:
		update_download_sink() = delete;
		update_download_sink(const std::string& directory,
			download_update::download_info& progress,
			mutex& progress_mutex) :
			_p_file(nullptr),
			_directory(directory),
			_downloaded(0),
			_total(0),
			_last_update(-1),
			_progress(progress),
			_progress_mutex(progress_mutex) {}
		~update_download_sink() {
			close();
		}

		void close() {
			if (_p_file) {
				fclose(_p_file);
				_p_file = nullptr;
			}
		}

		void set_length(size_t length) override { _total = length; }
		bool set_filename(const std::string& filename, std::string& error) override {
			error.clear();

			if (_p_file) {
				error = "Update file already set";
				return false;
			}

			_fullpath = _directory.empty() ? filename : _directory + "\\" + filename;
			fopen_s(&_p_file, _fullpath.c_str(), "wb");
			if (!_p_file) {
				error = "Cannot save update file";
				return false;
			}

			return true;
		}

		bool add_chunk(const void* data, size_t len, std::string& error) override {
			if (!_p_file) {
				error = "Filename is not set";
				return false;
			}

			if (fwrite(data, len, 1, _p_file) != 1) {
				error = "Cannot save update file";
				return false;
			}

			_downloaded += len;

			// only update at most 10 times/sec so that we don't flood the UI:
			clock_t now = clock();
			if (now == -1 || _downloaded == _total ||
				((double(now - _last_update) / CLOCKS_PER_SEC) >= 0.1)) {
				auto_mutex lock(_progress_mutex);

				// update download progress
				_progress.downloaded = _downloaded;
				_progress.file_size = _total;
				_last_update = now;
			}

			return true;
		}

		std::string get_fullpath() override {
			return _fullpath;
		}
	};

	static download_update_result download_update_func(download_update::impl* p_impl) {
		download_update::impl& _d = *p_impl;

		download_update_result result;
		result.error.clear();
		result.fullpath.clear();
		result.success = false;

		if (_d._url.empty()) {
			result.error = "Download URL not specified";
			result.success = false;
			return result;
		}

		update_download_sink sink(_d._directory, _d._progress, _d._progress_mutex);

		result.success = download(_d._url, sink, true, result.error);
		
		sink.close();

		if (!result.success)
			return result;

		result.fullpath = sink.get_fullpath();
		result.success = true;
		return result;
	}
};

download_update::download_update() : _d(*new impl()) {}
download_update::~download_update() {
	if (_d._fut.valid()) {
		// to-do: add mechanism for stopping immediately
		_d._fut.get();
	}
	
	delete& _d;
}

void download_update::start(const std::string& url,
	const std::string& directory) {
	if (downloading()) {
		// allow only one instance
		return;
	}

	_d._url = url;
	_d._directory = directory;

	// run task asynchronously
	_d._fut = std::async(std::launch::async, _d.download_update_func, &_d);
	return;
}

bool download_update::downloading() {
	if (_d._fut.valid())
		return _d._fut.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool download_update::downloading(download_info& progress) {
	auto res = downloading();

	auto_mutex lock(_d._progress_mutex);
	progress = _d._progress;
	return res;
}

bool download_update::result(std::string& fullpath,
	std::string& error) {
	error.clear();
	fullpath.clear();
	
	if (downloading()) {
		error = "Task not yet complete";
		return false;
	}

	if (_d._fut.valid()) {
		auto result = _d._fut.get();
		fullpath = result.fullpath;
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
