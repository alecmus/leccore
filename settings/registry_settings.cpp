//
// registry_settings.cpp - registry settings implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../settings.h"
#include "../leccore_common.h"
#include "../app_version_info.h"
#include "../encode.h"
#include "../hash.h"
#include "../encrypt.h"

using namespace liblec::leccore;

class registry_settings::impl {
public:
	const registry::scope _scope;
	const std::string _key;
	const std::string _iv;
	const bool _encrypted;
	const int _salt_length = 4;
	std::string _registry_path;

	impl(registry::scope settings_scope,
		const std::string& key,
		const std::string& iv) :
		_scope(settings_scope),
		_key(key),
		_iv(iv),
		_encrypted(!_key.empty() && !_iv.empty()) {}
	~impl() {}

	bool encrypt_string(const std::string& plain,
		std::string& encrypted,
		std::string& error) {
		error.clear();
		encrypted.clear();
		// make random salt (makes the encoded and encrypted results different for the same input)
		const std::string salt = hash_string::random_string(_salt_length);

		// encode the salt and plain text to base64
		std::string encoded = base64::encode(salt + plain);

		// encrypt the encoded text
		aes enc(_key, _iv);
		return enc.encrypt(encoded, encrypted, error);
	}

	bool decrypt_string(const std::string& encrypted,
		std::string& decrypted,
		std::string& error) {
		error.clear();
		decrypted.clear();
		// decrypt the text
		std::string encoded;
		aes enc(_key, _iv);
		if (!enc.decrypt(encrypted, encoded, error))
			return false;

		// decode the decrypted text
		base64 base64_encoder;
		decrypted = base64_encoder.decode(encoded);

		// step over the salt
		decrypted = decrypted.substr(_salt_length);
		return true;
	}
};

registry_settings::registry_settings(registry::scope settings_scope) : registry_settings(settings_scope, "", "") {}
registry_settings::registry_settings(registry::scope settings_scope,
	const std::string& key, const std::string& iv) : _d(*new impl(settings_scope, key, iv)) {}
registry_settings::~registry_settings() { delete& _d; }

bool registry_settings::get_registry_path(std::string& registry_path, std::string& error) {
	error.clear();
	registry_path.clear();
	if (_d._registry_path.empty()) {
		app_version_info ver_info;

		std::string company_name, app_name;
		if (!ver_info.get_company_name(company_name, error))
			return false;

		if (!ver_info.get_app_name(app_name, error))
			return false;

		_d._registry_path = "Software\\" +
			company_name + "\\" +
			app_name;
	}
	registry_path = _d._registry_path;
	return true;
}

void registry_settings::set_registry_path(const std::string& registry_path) {
	_d._registry_path = registry_path;
}

bool registry_settings::write_value(const std::string& branch,
	const std::string& value_name, const std::string& value, std::string& error) {
	error.clear();
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string _branch;

	if (!branch.empty())
		_branch += "\\" + branch;

	const std::string sub_key = registry_path + _branch;

	if (_d._encrypted) {
		std::string data_encrypted;
		if (!_d.encrypt_string(value, data_encrypted, error))
			return false;

		registry reg(_d._scope);
		return reg.do_write_binary(sub_key, value_name, data_encrypted, error);
	}
	else {
		registry reg(_d._scope);
		return reg.do_write(sub_key, value_name, value, error);
	}
}

bool registry_settings::read_value(const std::string& branch,
	const std::string& value_name, std::string& value, std::string& error) {
	error.clear();
	value.clear();
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string _branch;

	if (!branch.empty())
		_branch += "\\" + branch;

	const std::string sub_key = registry_path + _branch;

	if (_d._encrypted) {
		std::string data_encrypted;
		registry reg(_d._scope);
		if (!reg.do_read_binary(sub_key, value_name, data_encrypted, error))
			return false;

		return _d.decrypt_string(data_encrypted, value, error);
	}
	else {
		registry reg(_d._scope);
		return reg.do_read(sub_key, value_name, value, error);
	}
}

bool liblec::leccore::registry_settings::delete_value(const std::string& branch,
	const std::string& value_name, std::string& error) {
	error.clear();
	if (value_name.empty()) {
		error = "Value name not specified";
		return false;
	}

	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string _branch;

	if (!branch.empty())
		_branch += "\\" + branch;

	const std::string sub_key = registry_path + _branch;

	registry reg(_d._scope);
	return reg.do_delete(sub_key, value_name, error);
}

bool registry_settings::delete_recursive(const std::string& branch,
	std::string& error) {
	error.clear();
	std::string registry_path;
	if (!get_registry_path(registry_path, error))
		return false;

	std::string _branch;

	if (!branch.empty())
		_branch += "\\" + branch;

	const std::string sub_key = registry_path + _branch;

	registry reg(_d._scope);
	return reg.do_recursive_delete(sub_key, error);
}
