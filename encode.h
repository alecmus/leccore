//
// encode.h - encoding/decoding interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#pragma once

#if defined(LECCORE_EXPORTS)
#include "leccore.h"
#else
#include <liblec/leccore.h>
#endif

#include <string>

namespace liblec {
	namespace leccore {
		class leccore_api base32 {
		public:
			static const std::string default_alphabet();

			static std::string encode(const std::string& input,
				std::string alphabet = default_alphabet());
			static std::string decode(const std::string& input,
				std::string alphabet = default_alphabet());
		};

		class leccore_api base64 {
		public:
			static const std::string default_alphabet();

			static std::string encode(const std::string& input,
				std::string alphabet = default_alphabet());
			static std::string decode(const std::string& input,
				std::string alphabet = default_alphabet());
		};
	}
}
