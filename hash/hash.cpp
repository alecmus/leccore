//
// hash.cpp - hash implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "..\hash.h"
#include "sha256.h"
#include "sha512.h"

std::string liblec::leccore::hash::sha256(const std::string& input) {
    unsigned char digest[SHA256::DIGEST_SIZE];
    memset(digest, 0, SHA256::DIGEST_SIZE);

    SHA256 ctx = SHA256();
    ctx.init();
    ctx.update((unsigned char*)input.c_str(), static_cast<unsigned int>(input.length()));
    ctx.final(digest);

    char buf[2 * SHA256::DIGEST_SIZE + 1];
    buf[2 * SHA256::DIGEST_SIZE] = 0;
    for (int i = 0; i < SHA256::DIGEST_SIZE; i++)
        sprintf_s(buf + i * 2, 2 + 1, "%02x", digest[i]);
    return std::string(buf);
}

std::string liblec::leccore::hash::sha512(const std::string& input) {
    unsigned char digest[SHA512::DIGEST_SIZE];
    memset(digest, 0, SHA512::DIGEST_SIZE);

    SHA512 ctx = SHA512();
    ctx.init();
    ctx.update((unsigned char*)input.c_str(), static_cast<unsigned int>(input.length()));
    ctx.final(digest);

    char buf[2 * SHA512::DIGEST_SIZE + 1];
    buf[2 * SHA512::DIGEST_SIZE] = 0;
    for (int i = 0; i < SHA512::DIGEST_SIZE; i++)
        sprintf_s(buf + i * 2, 2 + 1, "%02x", digest[i]);
    return std::string(buf);
}
