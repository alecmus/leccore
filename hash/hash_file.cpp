//
// hash_file.cpp - file hash implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../hash.h"
#include <thread>
#include <future>
#include <cryptlib.h>
#include <sha.h>
#include <hex.h>
#include <files.h>
#include <filters.h>
#include <channels.h>

using namespace liblec::leccore;

class hash_file::impl {
public:
	std::string fullpath_;
	std::vector<algorithm> algorithms_;

	struct do_hash_result {
		bool success = false;
		std::string error;
		std::map<algorithm, std::string> hashes;
	};

	std::future<do_hash_result> fut_;

	impl() {}
	~impl() {}

	static do_hash_result hash_func(hash_file::impl* p_impl) {
		hash_file::impl& d_ = *p_impl;

		do_hash_result result;
		result.error.clear();
		result.success = false;
		result.hashes.clear();

		if (d_.fullpath_.empty()) {
			result.error = "File path not specified";
			result.success = false;
			return result;
		}

		if (d_.algorithms_.empty()) {
			result.error = "Algorithms not specified";
			result.success = false;
			return result;
		}

		try {
			CryptoPP::ChannelSwitch cs;
			std::map<algorithm, CryptoPP::HashFilter> filters;

			CryptoPP::SHA256 sha256_hash;
			CryptoPP::SHA512 sha512_hash;

			for (const auto& algo : d_.algorithms_)	{
				result.hashes[algo] = "";

				if (algo == algorithm::sha256) {
					filters.try_emplace(algo, sha256_hash,
						new CryptoPP::HexEncoder(new CryptoPP::StringSink(result.hashes.at(algo))));
					cs.AddDefaultRoute(filters.at(algo));
				}
				if (algo == algorithm::sha512) {
					filters.try_emplace(algo, sha512_hash,
						new CryptoPP::HexEncoder(new CryptoPP::StringSink(result.hashes.at(algo))));
					cs.AddDefaultRoute(filters.at(algo));
				}
			}

			CryptoPP::FileSource fs(d_.fullpath_.c_str(), true, new CryptoPP::Redirector(cs));

			result.success = true;
			return result;
		}
		catch (const CryptoPP::Exception& e) {
			result.error = e.what();
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

hash_file::hash_file() : d_(*new impl()) {}
liblec::leccore::hash_file::~hash_file() {
	if (d_.fut_.valid())
		d_.fut_.get();

	delete& d_;
}

void hash_file::start(const std::string& fullpath,
	const std::vector<algorithm>& algorithms) {
	if (hashing()) {
		// allow only one instance
		return;
	}

	d_.fullpath_ = fullpath;
	d_.algorithms_ = algorithms;

	// run task asynchronously
	d_.fut_ = std::async(std::launch::async, d_.hash_func, &d_);
	return;
}

bool hash_file::hashing() {
	if (d_.fut_.valid())
		return d_.fut_.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
	else
		return false;
}

bool hash_file::result(hash_results& results,
	std::string& error) {
	error.clear();
	results.clear();

	if (hashing()) {
		error = "Task not yet complete";
		return false;
	}

	if (d_.fut_.valid()) {
		auto result = d_.fut_.get();
		results = result.hashes;
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
