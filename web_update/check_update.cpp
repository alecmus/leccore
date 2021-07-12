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
#include "parse_update_xml.h"
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

	const std::string _update_xml_url;
	std::future<check_update_result> _fut;

	impl(const std::string& update_xml_url) :
		_update_xml_url(update_xml_url) {}
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
		check_update::impl& _d = *p_impl;

		check_update_result result;
		result.error.clear();
		result.success = false;
		result.details = {};

		if (_d._update_xml_url.empty()) {
			result.error = "Update XML url not specified";
			result.success = false;
			return result;
		}

		string_download_sink sink;
		if (!download(_d._update_xml_url, sink, true, result.error)) {
			result.success = false;
			return result;
		}

		if (!parse_update_xml(sink.data, result.details, result.error)) {
			result.success = false;
			return result;
		}

		result.success = true;
		return result;
	}
};

check_update::check_update(const std::string& update_xml_url) :
	_d(*new impl(update_xml_url)) {}
check_update::~check_update() {
	if (_d._fut.valid()) {
		// to-do: add mechanism for stopping immediately
		_d._fut.get();
	}

	delete& _d;
}

void check_update::start() {
	if (checking()) {
		// allow only one instance
		return;
	}

	// run task asynchronously
	_d._fut = std::async(std::launch::async, _d.check_update_func, &_d);
	return;
}

bool check_update::checking() {
	if (_d._fut.valid())
		return _d._fut.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
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

	if (_d._fut.valid()) {
		auto result = _d._fut.get();
		details = result.details;
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
