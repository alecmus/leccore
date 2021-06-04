//
// check_update.cpp - check web update implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../web_update.h"
#include "download.h"
#include "appcast.h"
#include <thread>
#include <future>

using namespace liblec::leccore;

class check_update::impl {
public:
	struct check_update_result {
		bool success = false;
		std::string error;
		update_info details;
	};

	const std::string appcast_url_;
	std::future<check_update_result> fut_;

	impl(const std::string& appcast_url) :
		appcast_url_(appcast_url) {}
	~impl() {}

	class string_download_sink : public download_sink {
	public:
		std::string data;

		void set_length(size_t) override {}

		bool set_filename(const std::string&, std::string& error) override {
			error.clear();
			return true;
		}

		bool add_chunk(const void* data, size_t len, std::string& error) override {
			error.clear();
			this->data.append(reinterpret_cast<const char*>(data), len);
			return true;
		}

		std::string get_fullpath() override {
			return std::string();
		}
	};

	static check_update_result check_update_func(check_update::impl* p_impl) {
		check_update::impl& d_ = *p_impl;

		check_update_result result;
		result.error.clear();
		result.success = false;
		result.details = {};

		if (d_.appcast_url_.empty()) {
			result.error = "Appcast URL not specified";
			result.success = false;
			return result;
		}

		string_download_sink appcast_xml;
		if (!download(d_.appcast_url_, appcast_xml, true, result.error)) {
			result.success = false;
			return result;
		}

		if (!load_appcast(appcast_xml.data, result.details, result.error)) {
			result.success = false;
			return result;
		}

		result.success = true;
		return result;
	}
};

check_update::check_update(const std::string& appcast_url) :
	d_(*new impl(appcast_url)) {}
check_update::~check_update() {
	if (d_.fut_.valid())
		d_.fut_.get();

	delete& d_;
}

void check_update::start() {
	if (checking()) {
		// allow only one instance
		return;
	}

	// run task asynchronously
	d_.fut_ = std::async(std::launch::async, d_.check_update_func, &d_);
	return;
}

bool check_update::checking() {
	if (d_.fut_.valid())
		return d_.fut_.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool check_update::result(update_info& details, std::string& error) {
	error.clear();
	details = {};

	if (checking()) {
		error = "Task not yet complete";
		return false;
	}

	if (d_.fut_.valid()) {
		auto result = d_.fut_.get();
		details = result.details;
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
