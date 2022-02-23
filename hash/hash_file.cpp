//
// hash_file.cpp - file hash implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
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
	std::string _fullpath;
	std::vector<algorithm> _algorithms;

	struct do_hash_result {
		bool success = false;
		std::string error;
		std::map<algorithm, std::string> hashes;
	};

	std::future<do_hash_result> _fut;

	impl() {}
	~impl() {}

	static do_hash_result hash_func(hash_file::impl* p_impl) {
		hash_file::impl& _d = *p_impl;

		do_hash_result result;
		result.error.clear();
		result.success = false;
		result.hashes.clear();

		if (_d._fullpath.empty()) {
			result.error = "File path not specified";
			result.success = false;
			return result;
		}

		if (_d._algorithms.empty()) {
			result.error = "Algorithms not specified";
			result.success = false;
			return result;
		}

		try {
			CryptoPP::ChannelSwitch cs;
			std::map<algorithm, CryptoPP::HashFilter> filters;

			CryptoPP::SHA256 sha256_hash;
			CryptoPP::SHA512 sha512_hash;

			for (const auto& algo : _d._algorithms)	{
				result.hashes[algo] = "";

				if (algo == algorithm::sha256) {
					filters.try_emplace(algo, sha256_hash,
						new CryptoPP::HexEncoder(new CryptoPP::StringSink(result.hashes.at(algo)), false));
					cs.AddDefaultRoute(filters.at(algo));
				}
				if (algo == algorithm::sha512) {
					filters.try_emplace(algo, sha512_hash,
						new CryptoPP::HexEncoder(new CryptoPP::StringSink(result.hashes.at(algo)), false));
					cs.AddDefaultRoute(filters.at(algo));
				}
			}

			CryptoPP::FileSource fs(_d._fullpath.c_str(), true, new CryptoPP::Redirector(cs));

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

hash_file::hash_file() : _d(*new impl()) {}
liblec::leccore::hash_file::~hash_file() {
	if (_d._fut.valid())
		_d._fut.get();

	delete& _d;
}

void hash_file::start(const std::string& fullpath,
	const std::vector<algorithm>& algorithms) {
	if (hashing()) {
		// allow only one instance
		return;
	}

	_d._fullpath = fullpath;
	_d._algorithms = algorithms;

	// run task asynchronously
	_d._fut = std::async(std::launch::async, _d.hash_func, &_d);
	return;
}

bool hash_file::hashing() {
	if (_d._fut.valid())
		return _d._fut.wait_for(std::chrono::seconds{ 0 }) != std::future_status::ready;
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

	if (_d._fut.valid()) {
		auto result = _d._fut.get();
		results = result.hashes;
		error = result.error;
		return result.success;
	}

	error = "unexpected error";
	return false;
}
