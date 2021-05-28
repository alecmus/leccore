//
// base64.cpp - base64 encoding/decoding implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../encode.h"
#include <cryptlib.h>
#include <base64.h>

const std::string liblec::leccore::base64::default_alphabet() {
    return std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
}

std::string liblec::leccore::base64::encode(const std::string& input,
    std::string alphabet) {
    std::string encoded;
    try {
        if (alphabet.length() != 64)
            alphabet = default_alphabet();

        const CryptoPP::AlgorithmParameters encoder_params =
            CryptoPP::MakeParameters(CryptoPP::Name::EncodingLookupArray(),
                (const CryptoPP::byte*)alphabet.c_str());

        CryptoPP::Base64Encoder* p_encoder =
            new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded));
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

std::string liblec::leccore::base64::decode(const std::string& input,
    std::string alphabet) {
    std::string decoded;
    try {
        if (alphabet.length() != 64)
            alphabet = default_alphabet();

        int decoder_lookup[256];
        CryptoPP::Base64Decoder::InitializeDecodingLookupArray(decoder_lookup,
            (const CryptoPP::byte*)alphabet.c_str(), 64, false);
        const CryptoPP::AlgorithmParameters decoder_params =
            CryptoPP::MakeParameters(CryptoPP::Name::DecodingLookupArray(),
                (const int*)decoder_lookup);

        CryptoPP::Base64Decoder* p_decoder =
            new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded));
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
