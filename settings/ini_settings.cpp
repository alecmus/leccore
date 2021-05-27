//
// ini_settings.cpp - ini settings implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
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
	const std::string file_name_;
	const std::string key_;
	const std::string iv_;
	const bool encrypted_;
	const int salt_length_ = 4;
	std::string ini_path_;
	bool ini_path_set_;

	impl(const std::string& file_name,
		const std::string& key,
		const std::string& iv) :
		file_name_(file_name.empty() ? "config.ini" : file_name),
		key_(key),
		iv_(iv),
		encrypted_(!key_.empty() && !iv_.empty()),
		ini_path_set_(false) {}
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

	static bool erase_path(boost::property_tree::ptree& pt,
		const boost::property_tree::ptree::key_type& key_path,
		std::string& error) {
		try {
			boost::property_tree::ptree::path_type path(key_path);
			boost::property_tree::ptree::key_type parent_path;
			boost::property_tree::ptree::key_type sub_key;

			while (!path.single()) {
				sub_key = path.reduce();
				parent_path = parent_path.empty() ? sub_key :
					parent_path + path.separator() + sub_key;
				pt = pt.get_child(sub_key);
			}

			sub_key = path.reduce();
			pt.erase(sub_key);
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
	const std::string& iv) : d_(*new impl(file_name, key, iv)) {}
ini_settings::~ini_settings() { delete& d_; }

bool ini_settings::get_ini_path(std::string& ini_path, std::string& error) {
	if (!d_.ini_path_set_ && d_.ini_path_.empty()) {
		app_version_info ver_info;

		std::string company_name, app_name;
		if (!ver_info.get_company_name(company_name, error))
			return false;

		if (!ver_info.get_app_name(app_name, error))
			return false;

		auto get_app_data = [](std::string& app_data_folder, std::string& error)->bool {
			app_data_folder.clear();
			CHAR szPath[MAX_PATH];
			HRESULT h_res = SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
			
			if (FAILED(h_res)) {
				error = convert_string(_com_error(h_res).ErrorMessage());
				return false;
			}
			
			app_data_folder = szPath;
			return true;
		};

		// get appdata folder
		std::string app_data_folder;
		if (!get_app_data(app_data_folder, error))
			return false;

		d_.ini_path_ = app_data_folder + "\\" +
			company_name + "\\" +
			app_name;
	}
	ini_path = d_.ini_path_;
	return true;
}

void ini_settings::set_ini_path(const std::string& ini_path) {
	d_.ini_path_ = ini_path;
	d_.ini_path_set_ = true;
}

bool ini_settings::write_value(const std::string& branch,
	const std::string& value_name, const std::string& value, std::string& error) {
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		d_.file_name_ : ini_path + "\\" + d_.file_name_;

	std::string path_ = value_name;

	if (!branch.empty())
		path_ = branch + "." + path_;

	try {
		if (!ini_path.empty()) {
			std::filesystem::path path(ini_path);
			std::filesystem::create_directories(path);
		}

		boost::property_tree::ptree pt;

		std::filesystem::path file(full_ini_file_path);
		if (std::filesystem::is_regular_file(file))
			boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);

		if (d_.encrypted_) {
			std::string data_encrypted;
			if (!d_.encrypt_string(value, data_encrypted, error))
				return false;

			std::string encoded = base32::encode(data_encrypted);
			pt.put(path_, encoded);
			boost::property_tree::write_ini(full_ini_file_path, pt);
			return true;

		}
		else {
			pt.put(path_, value);
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
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		d_.file_name_ : ini_path + "\\" + d_.file_name_;

	std::string path_ = value_name;

	if (!branch.empty())
		path_ = branch + "." + path_;

	try {
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);

		if (d_.encrypted_) {
			std::string encoded;
			try { encoded = pt.get<std::string>(path_); }
			catch (const std::exception&) {
				return true;
			}

			std::string data_encrypted = base32::decode(encoded);
			return d_.decrypt_string(data_encrypted, value, error);
		}
		else {
			try {
				value = pt.get<std::string>(path_);
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
	if (value_name.empty()) {
		error = "Value name not specified";
		return false;
	}

	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		d_.file_name_ : ini_path + "\\" + d_.file_name_;

	std::string path_ = value_name;

	if (!branch.empty())
		path_ = branch + "." + path_;

	try {
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);
		if (!impl::erase_path(pt, path_, error))
			return false;

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
	std::string ini_path;
	if (!get_ini_path(ini_path, error))
		return false;

	const std::string full_ini_file_path = ini_path.empty() ?
		d_.file_name_ : ini_path + "\\" + d_.file_name_;

	std::string path_ = branch;

	try {
		boost::property_tree::ptree pt;

		if (!path_.empty()) {
			boost::property_tree::ini_parser::read_ini(full_ini_file_path, pt);
			if (!impl::erase_path(pt, path_, error))
				return false;
		}

		boost::property_tree::write_ini(full_ini_file_path, pt);
		return true;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}
