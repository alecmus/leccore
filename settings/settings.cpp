//
// settings.cpp - settings implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../settings.h"
#include "../leccore_common.h"
#include "../error/win_error.h"
#include "../app_version_info.h"
#include "../encode.h"
#include "../hash.h"
#include "../encrypt.h"

using namespace liblec::leccore;

class settings::impl {
public:
	const registry::scope scope_;
	const std::string key_;
	const std::string iv_;
	const bool encrypted_;
	const int salt_length_ = 4;
	std::string registry_path_;

	impl(registry::scope settings_scope,
		const std::string& key,
		const std::string& iv) :
		scope_(settings_scope),
		key_(key),
		iv_(iv),
		encrypted_(!key_.empty() && !iv_.empty()) {}
	~impl() {}

	bool encrypt_string(const std::string& plain,
		std::string& encrypted,
		std::string& error) {
		const std::string salt = hash::random_string(salt_length_);
		std::string encoded = base64::encode(salt + plain);
		aes enc(key_, iv_);
		return enc.encrypt(encoded, encrypted, error);
	}

	bool decrypt_string(const std::string& encrypted,
		std::string& decrypted,
		std::string& error) {
		std::string encoded;
		aes enc(key_, iv_);
		if (!enc.decrypt(encrypted, encoded, error))
			return false;

		base64 base64_encoder;
		decrypted = base64_encoder.decode(encoded);
		decrypted = decrypted.substr(salt_length_);
		return true;
	}
};

settings::settings(registry::scope settings_scope) : settings(settings_scope, "", "") {}
settings::settings(registry::scope settings_scope,
	const std::string& key, const std::string& iv) : d_(*new impl(settings_scope, key, iv)) {}
settings::~settings() { delete& d_; }

bool settings::get_registry_path(std::string& registry_path, std::string& error) {
	if (d_.registry_path_.empty()) {
		app_version_info ver_info;

		std::string company_name, app_name;
		if (!ver_info.get_company_name(company_name, error))
			return false;

		if (!ver_info.get_app_name(app_name, error))
			return false;

		d_.registry_path_ = "Software\\" +
			company_name + "\\" +
			app_name;
	}
	registry_path = d_.registry_path_;
	return true;
}

void settings::set_registry_path(const std::string& registry_path) {
	d_.registry_path_ = registry_path;
}

bool settings::write_value(const std::string& branch,
	const std::string& value_name, const std::string& value, std::string& error) {
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string branch_;

	if (!branch.empty())
		branch_ += "\\" + branch;

	const std::string sub_key = registry_path + branch_;

	if (d_.encrypted_) {
		std::string data_encrypted;
		if (!d_.encrypt_string(value, data_encrypted, error))
			return false;

		registry reg(d_.scope_);
		return reg.do_write_binary(sub_key, value_name, data_encrypted, error);
	}
	else {
		registry reg(d_.scope_);
		return reg.do_write(sub_key, value_name, value, error);
	}
}

bool settings::read_value(const std::string& branch,
	const std::string& value_name, std::string& value, std::string& error) {
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string branch_;

	if (!branch.empty())
		branch_ += "\\" + branch;

	const std::string sub_key = registry_path + branch_;

	if (d_.encrypted_) {
		std::string data_encrypted;
		registry reg(d_.scope_);
		if (!reg.do_read_binary(sub_key, value_name, data_encrypted, error))
			return false;

		return d_.decrypt_string(data_encrypted, value, error);
	}
	else {
		registry reg(d_.scope_);
		return reg.do_read(sub_key, value_name, value, error);
	}
}

bool liblec::leccore::settings::delete_value(const std::string& branch,
	const std::string& value_name, std::string& error) {
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string branch_;

	if (!branch.empty())
		branch_ += "\\" + branch;

	const std::string sub_key = registry_path + branch_;

	registry reg(d_.scope_);
	return reg.do_delete(sub_key, value_name, error);
}

bool settings::delete_recursive(const std::string& branch,
	std::string& error) {
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string branch_;

	if (!branch.empty())
		branch_ += "\\" + branch;

	const std::string sub_key = registry_path + branch_;

	registry reg(d_.scope_);
	reg.do_recursive_delete(sub_key);
	return true;
}
