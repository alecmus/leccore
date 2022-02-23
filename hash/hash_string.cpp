//
// hash_string.cpp - string hashing implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../hash.h"
#include <cryptlib.h>
#include <sha.h>
#include <hex.h>
#include <osrng.h>

// for UuidToString, RpcStringFree
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")

std::string liblec::leccore::hash_string::sha256(const std::string& input) {
    try {
        CryptoPP::SHA256 hash;
        std::string output;
        CryptoPP::StringSource ss(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(output), false)));
        return output;
    }
    catch (CryptoPP::Exception&) {
        // to-do: log
    }
    catch (std::exception&) {
        // to-do: log
    }
    return std::string();
}

std::string liblec::leccore::hash_string::sha512(const std::string& input) {
    try {
        CryptoPP::SHA512 hash;
        std::string output;
        CryptoPP::StringSource ss(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(output), false)));
        return output;
    }
    catch (CryptoPP::Exception&) {
        // to-do: log
    }
    catch (std::exception&) {
        // to-do: log
    }
    return std::string();
}

std::string liblec::leccore::hash_string::random_string(int length) {
    try {
        std::string random;
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RandomNumberSource rns(rng, length, true,
            new CryptoPP::StringSink(random)
        );
        return random;
    }
    catch (CryptoPP::Exception&) {
        // to-do: log
    }
    catch (std::exception&) {
        // to-do: log
    }
    return std::string();
}

std::string liblec::leccore::hash_string::uuid() {
    std::string uuid_string;
    UUID uuid;
    UuidCreate(&uuid);
    RPC_CSTR uuidStr;
    RPC_STATUS status = UuidToStringA(&uuid, &uuidStr);
    uuid_string += reinterpret_cast<char*>(uuidStr);
    RpcStringFreeA(&uuidStr);
    return uuid_string;
}
