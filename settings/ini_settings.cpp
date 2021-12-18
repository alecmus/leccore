//
// ini_settings.cpp - ini settings implementation
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
#include "../encode.h"

#include <filesystem>

#include <Windows.h>
#include <ShlObj.h>	// for SHGetFolderPathA
#include <comdef.h>	// for _com_error

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace liblec::leccore;

class ini_settings::impl {
public:
	const std::string _default_file_name = "config.ini";
	const std::string _file_name;
	const std::string _key;
	const std::string _iv;
	const bool _encrypted;
	const int _salt_length = 4;
	std::string _ini_path;
	bool _ini_path_set;

	impl(const std::string& file_name,
		const std::string& key,
		const std::string& iv) :
		_file_name(file_name.empty() ? _default_file_name : file_name),
		_key(key),
		_iv(iv),
		_encrypted(!_key.empty() && !_iv.empty()),
		_ini_path_set(false) {}
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

	static bool erase_path(boost::property_tree::ptree& pt,
		const boost::property_tree::ptree::key_type& key_path,
		size_t& erased_subkeys,
		std::string& error) {
		error.clear();
		try {
			boost::property_tree::ptree* p_sub_tree = &pt;
			boost::property_tree::ptree::path_type path(key_path);
			boost::property_tree::ptree::key_type parent_path;
			boost::property_tree::ptree::key_type sub_key;

			while (!path.single()) {
				sub_key = path.reduce();
				parent_path = parent_path.empty() ? sub_key :
					parent_path + path.separator() + sub_key;
				p_sub_tree = &(p_sub_tree->get_child(sub_key));
			}

			sub_key = path.reduce();
			const auto _count = p_sub_tree->erase(sub_key);
			erased_subkeys += _count;
			if (_count == 0)
				return true;
			
			if (p_sub_tree->empty() && !parent_path.empty())
				return erase_path(pt, parent_path, erased_subkeys, error);
			
			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}
};

ini_settings::ini_settings(const std::string& file_name) : ini_settings(file_name, "", "") {}
ini_settings::ini_settings(const std::string& file_name, const std::string& key,
	const std::string& iv) : _d(*new impl(file_name, key, iv)) {}
ini_settings::~ini_settings() { delete& _d; }

bool ini_settings::get_ini_path(std::string& ini_path, std::string& error) {
	error.clear();
	ini_path.clear();
	if (!_d._ini_path_set && _d._ini_path.empty()) {
		app_version_info ver_info;

		std::string company_name, app_name;
		if (!ver_info.get_company_name(company_name, error))
			return false;

		if (!ver_info.get_app_name(app_name, error))
			return false;

		auto get_app_data = [](std::string& app_data_folder, std::string& error)->bool {
			app_data_folder.clear();
			CHAR szPath[MAX_PATH];
			HRESULT result = SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
			
			if (FAILED(result)) {
				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}
			
			app_data_folder = szPath;
			return true;
		};

		// get appdata folder
		std::string app_data_folder;
		if (!get_app_data(app_data_folder, error))
			return false;

		_d._ini_path = app_data_folder + "\\" +
			company_name + "\\" +
			app_name;
	}
	ini_path = _d._ini_path;
	return true;
}

void ini_settings::set_ini_path(const std::string& ini_path) {
	_d._ini_path = ini_path;
	_d._ini_path_set = true;
}

bool ini_settings::write_value(const std::string& branch,
	const std::string& value_name, const std::string& value, std::string& error) {
	error.clear();
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		_d._file_name : ini_path + "\\" + _d._file_name;

	std::string _path = value_name;

	if (!branch.empty())
		_path = branch + "." + _path;

	try {
		if (!ini_path.empty()) {
			std::filesystem::path path(ini_path);
			std::filesystem::create_directories(path);
		}

		boost::property_tree::ptree pt;

		std::filesystem::path file(full_ini_file_path);
		if (std::filesystem::is_regular_file(file))
			boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);

		if (_d._encrypted) {
			std::string data_encrypted;
			if (!_d.encrypt_string(value, data_encrypted, error))
				return false;

			std::string encoded = base32::encode(data_encrypted);
			pt.put(_path, encoded);
			boost::property_tree::write_ini(full_ini_file_path, pt);
			return true;
		}
		else {
			pt.put(_path, value);
			boost::property_tree::write_ini(full_ini_file_path, pt);
			return true;
		}
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}

bool ini_settings::read_value(const std::string& branch,
	const std::string& value_name, std::string& value, std::string& error) {
	error.clear();
	value.clear();
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		_d._file_name : ini_path + "\\" + _d._file_name;

	std::string _path = value_name;

	if (!branch.empty())
		_path = branch + "." + _path;

	try {
		std::filesystem::path path(full_ini_file_path);
		if (!std::filesystem::exists(path))
			return true;

		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);

		if (_d._encrypted) {
			std::string encoded;
			try { encoded = pt.get<std::string>(_path); }
			catch (const std::exception&) {
				return true;
			}

			std::string data_encrypted = base32::decode(encoded);
			return _d.decrypt_string(data_encrypted, value, error);
		}
		else {
			try {
				value = pt.get<std::string>(_path);
				return true;
			}
			catch (const std::exception&) {
				return true;
			}
		}
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}

bool liblec::leccore::ini_settings::delete_value(const std::string& branch,
	const std::string& value_name, std::string& error) {
	error.clear();
	if (value_name.empty()) {
		error = "Value name not specified";
		return false;
	}

	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		_d._file_name : ini_path + "\\" + _d._file_name;

	std::string _path = value_name;

	if (!branch.empty())
		_path = branch + "." + _path;

	try {
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);
		size_t erased_subkeys = 0;
		if (!impl::erase_path(pt, _path, erased_subkeys, error))
			return false;

		if (erased_subkeys > 0)
			boost::property_tree::write_ini(full_ini_file_path, pt);

		return true;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}

bool ini_settings::delete_recursive(const std::string& branch,
	std::string& error) {
	error.clear();
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		_d._file_name : ini_path + "\\" + _d._file_name;

	std::string _path = branch;

	try {
		boost::property_tree::ptree pt;

		size_t erased_subkeys = 0;
		if (!_path.empty()) {
			boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);
			if (!impl::erase_path(pt, _path, erased_subkeys, error))
				return false;
		}

		if (erased_subkeys > 0)
			boost::property_tree::write_ini(full_ini_file_path, pt);

		return true;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}
