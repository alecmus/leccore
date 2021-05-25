//
// hash.cpp - hash implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../hash.h"
#include <cryptlib.h>
#include <sha.h>
#include <hex.h>
#include <osrng.h>

std::string liblec::leccore::hash::sha256(const std::string& input) {
    try {
        CryptoPP::SHA256 hash;
        std::string output;
        CryptoPP::StringSource ss(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(output))));
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

std::string liblec::leccore::hash::sha512(const std::string& input) {
    try {
        CryptoPP::SHA512 hash;
        std::string output;
        CryptoPP::StringSource ss(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(output))));
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

std::string liblec::leccore::hash::random_string(int length) {
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
