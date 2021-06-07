//
// hash.h - hash interface
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
		/// <summary>String hashing class.</summary>
		class leccore_api hash_string {
		public:
			/// <summary>SHA256 hash.</summary>
			/// <param name="input">The string that is to be hashed.</param>
			/// <returns>The SHA256 hash of <see cref="input"></see>.</returns>
			[[nodiscard]]
			static std::string sha256(const std::string& input);

			/// <summary>SHA512 hash.</summary>
			/// <param name="input">The string that is to be hashed.</param>
			/// <returns>The SHA512 hash of <see cref="input"></see>.</returns>
			[[nodiscard]]
			static std::string sha512(const std::string& input);

			/// <summary>Generate a random string.</summary>
			/// <param name="length">The length of the random string.</param>
			/// <returns>A random string of the specified <see cref="length"></see>.</returns>
			[[nodiscard]]
			static std::string random_string(int length);

			/// <summary>Make a universally unique identifier (uuid).</summary>
			/// <returns>The uuid string.</returns>
			static std::string uuid();
		};
	}
}
