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
		std::string full_path;
	};

	std::string url_;
	std::string directory_;
	std::future<download_update_result> fut_;
	
	download_info progress_;
	mutex progress_mutex_;

	impl() :
		progress_({ 0, 0 }) {}
	~impl() {}

	class update_download_sink : public download_sink {
		FILE* p_file_;
		const std::string& directory_;
		std::string full_path_;
		size_t downloaded_, total_;
		clock_t last_update_;

		download_update::download_info& progress_;
		mutex& progress_mutex_;

	public:
		update_download_sink() = delete;
		update_download_sink(const std::string& directory,
			download_update::download_info& progress,
			mutex& progress_mutex) :
			p_file_(nullptr),
			directory_(directory),
			downloaded_(0),
			total_(0),
			last_update_(-1),
			progress_(progress),
			progress_mutex_(progress_mutex) {}
		~update_download_sink() {
			close();
		}

		void close() {
			if (p_file_) {
				fclose(p_file_);
				p_file_ = nullptr;
			}
		}

		void set_length(size_t length) override { total_ = length; }
		bool set_filename(const std::string& filename, std::string& error) override {
			error.clear();

			if (p_file_) {
				error = "Update file already set";
				return false;
			}

			full_path_ = directory_.empty() ? filename : directory_ + "\\" + filename;
			fopen_s(&p_file_, full_path_.c_str(), "wb");
			if (!p_file_) {
				error = "Cannot save update file";
				return false;
			}

			return true;
		}

		bool add_chunk(const void* data, size_t len, std::string& error) override {
			if (!p_file_) {
				error = "Filename is not set";
				return false;
			}

			if (fwrite(data, len, 1, p_file_) != 1) {
				error = "Cannot save update file";
				return false;
			}

			downloaded_ += len;

			// only update at most 10 times/sec so that we don't flood the UI:
			clock_t now = clock();
			if (now == -1 || downloaded_ == total_ ||
				((double(now - last_update_) / CLOCKS_PER_SEC) >= 0.1)) {
				auto_mutex lock(progress_mutex_);

				// update download progress
				progress_.downloaded = downloaded_;
				progress_.file_size = total_;
				last_update_ = now;
			}

			return true;
		}

		std::string get_fullpath() override {
			return full_path_;
		}
	};

	static download_update_result download_update_func(download_update::impl* p_impl) {
		download_update::impl& d_ = *p_impl;

		download_update_result result;
		result.error.clear();
		result.full_path.clear();
		result.success = false;

		if (d_.url_.empty()) {
			result.error = "Download URL not specified";
			result.success = false;
			return result;
		}

		update_download_sink sink(d_.directory_, d_.progress_, d_.progress_mutex_);

		result.success = download(d_.url_, sink, true, result.error);
		
		sink.close();

		if (!result.success)
			return result;

		result.full_path = sink.get_fullpath();
		result.success = true;
		return result;
	}
};

download_update::download_update() : d_(*new impl()) {}
download_update::~download_update() {
	if (d_.fut_.valid())
		d_.fut_.get();
	
	delete& d_;
}

void download_update::start(const std::string& url,
	const std::string& directory) {
	if (downloading()) {
		// allow only one instance
		return;
	}

	d_.url_ = url;
	d_.directory_ = directory;

	// run task asynchronously
	d_.fut_ = std::async(std::launch::async, d_.download_update_func, &d_);
	return;
}

bool download_update::downloading() {
	if (d_.fut_.valid())
		return d_.fut_.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool download_update::downloading(download_info& progress) {
	auto res = downloading();

	auto_mutex lock(d_.progress_mutex_);
	progress = d_.progress_;
	return res;
}

bool download_update::result(std::string& full_path,
	std::string& error) {
	error.clear();
	full_path.clear();
	
	if (downloading()) {
		error = "Task not yet complete";
		return false;
	}

	if (d_.fut_.valid()) {
		auto result = d_.fut_.get();
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
