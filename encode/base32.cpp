//
// base32.cpp - base32 encoding/decoding implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../encode.h"
#include <cryptlib.h>
#include <base32.h>

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "cryptlib64d.lib")
#else
#pragma comment(lib, "cryptlib64.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "cryptlib32d.lib")
#else
#pragma comment(lib, "cryptlib32.lib")
#endif

#endif

const std::string liblec::leccore::base32::default_alphabet() {
    return std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567");
}

std::string liblec::leccore::base32::encode(const std::string& input,
    std::string alphabet) {
    std::string encoded;
    try {
        if (alphabet.length() != 32)
            alphabet = default_alphabet();
        
        const CryptoPP::AlgorithmParameters encoder_params =
            CryptoPP::MakeParameters(CryptoPP::Name::EncodingLookupArray(),
                (const CryptoPP::byte*)alphabet.c_str());

        CryptoPP::Base32Encoder* p_encoder =
            new CryptoPP::Base32Encoder(new CryptoPP::StringSink(encoded));
        p_encoder->IsolatedInitialize(encoder_params);
        CryptoPP::StringSource ss(input, true, p_encoder);
    }
    catch (CryptoPP::Exception&) {
        // to-do: log
    }
    catch (const std::exception&) {
        // to-do: log
    }

    return encoded;
}

std::string liblec::leccore::base32::decode(const std::string& input,
    std::string alphabet) {
    std::string decoded;
    try {
        if (alphabet.length() != 32)
            alphabet = default_alphabet();

        int decoder_lookup[256];
        CryptoPP::Base32Decoder::InitializeDecodingLookupArray(decoder_lookup,
            (const CryptoPP::byte*)alphabet.c_str(), 32, true);
        const CryptoPP::AlgorithmParameters decoder_params =
            CryptoPP::MakeParameters(CryptoPP::Name::DecodingLookupArray(),
                (const int*)decoder_lookup);

        CryptoPP::Base32Decoder* p_decoder =
            new CryptoPP::Base32Decoder(new CryptoPP::StringSink(decoded));
        p_decoder->IsolatedInitialize(decoder_params);
        CryptoPP::StringSource ss(input, true, p_decoder);
    }
    catch (CryptoPP::Exception&) {
        // to-do: log
    }
    catch (const std::exception&) {
        // to-do: log
    }

    return decoded;
}