//
// encrypt.h - encryption/decryption interface
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
		class leccore_api aes {
		public:
			aes(const std::string& key,
				const std::string& iv);
			~aes();

			bool encrypt(const std::string& input,
				std::string& encrypted,
				std::string& error);
			bool decrypt(const std::string& input,
				std::string& decrypted,
				std::string& error);

		private:
			class impl;
			impl& d_;
		};
	}
}
