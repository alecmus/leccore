//
// aes.cpp - AES encryption/decryption implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../encrypt.h"

#include <aes.h>
#include <hex.h>
#include <modes.h>

using namespace liblec::leccore;

class aes::impl {
public:
	CryptoPP::byte key_[CryptoPP::AES::MAX_KEYLENGTH];
	CryptoPP::byte  iv_[CryptoPP::AES::BLOCKSIZE];

	impl() = delete;
	impl(std::string key,
		std::string iv) {
		memset(key_, 0x00, CryptoPP::AES::MAX_KEYLENGTH);
		memset(iv_, 0x00, CryptoPP::AES::BLOCKSIZE);

		// make key
		const size_t max_key_string_length = CryptoPP::AES::MAX_KEYLENGTH * sizeof(CryptoPP::byte);
		if (max_key_string_length < key.size())
			key = key.substr(0, max_key_string_length);	// trim
		memcpy(key_, key.c_str(), key.size());

		// make iv
		const size_t max_iv_string_length = CryptoPP::AES::BLOCKSIZE * sizeof(CryptoPP::byte);
		if (max_iv_string_length < iv.size())
			iv = iv.substr(0, max_iv_string_length);	// trim
		memcpy(iv_, iv.c_str(), iv.size());
	}
	~impl() {}
};

aes::aes(const std::string& key, const std::string& iv) : d_(*new impl(key, iv)) {}
aes::~aes() { delete& d_; }

bool aes::encrypt(const std::string& input,
	std::string& encrypted, std::string& error) {
	error.clear();
	encrypted.clear();
	try {
		CryptoPP::AES::Encryption aesEncryption(d_.key_, CryptoPP::AES::MAX_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, d_.iv_);

		CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(encrypted));
		stfEncryptor.Put(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());
		stfEncryptor.MessageEnd();
		return true;
	}
	catch (CryptoPP::Exception& e) {
		error = e.what();
		return false;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}

bool aes::decrypt(const std::string& input,
	std::string& decrypted, std::string& error) {
	error.clear();
	decrypted.clear();
	try {
		CryptoPP::AES::Decryption aesDecryption(d_.key_, CryptoPP::AES::MAX_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, d_.iv_);

		CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted));
		stfDecryptor.Put(reinterpret_cast<const unsigned char*>(input.c_str()), input.size());
		stfDecryptor.MessageEnd();
		return true;
	}
	catch (CryptoPP::Exception& e) {
		error = e.what();
		return false;
	}
	catch (const std::exception& e) {
		error = e.what();
		return false;
	}
}
