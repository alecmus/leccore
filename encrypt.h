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
		/// <summary>256bit AES encryption class.</summary>
		class leccore_api aes {
		public:
			/// <summary>Constructor.</summary>
			/// <param name="key">The key to use for encryption. This must be kept private.</param>
			/// <param name="iv">The initialization vector to use. This must be unique.</param>
			aes(const std::string& key,
				const std::string& iv);
			~aes();

			/// <summary>Encrypt data.</summary>
			/// <param name="input">The data to be encrypted.</param>
			/// <param name="encrypted">The encrypted data.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			bool encrypt(const std::string& input,
				std::string& encrypted,
				std::string& error);

			/// <summary>Decrypt data.</summary>
			/// <param name="input">The data to be decrypted.</param>
			/// <param name="decrypted">The decrypted data.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			[[nodiscard]]
			bool decrypt(const std::string& input,
				std::string& decrypted,
				std::string& error);

		private:
			class impl;
			impl& d_;

			// Default constructor and copying an object of this class are not allowed.
			aes() = delete;
			aes(const aes&) = delete;
			aes& operator==(const aes&) = delete;
		};
	}
}
