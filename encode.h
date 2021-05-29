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
		/// <summary>Base32 encoding/decoding class.</summary>
		class leccore_api base32 {
		public:
			/// <summary>Get the default base32 encoding alphabet
			/// used by the <see cref="base32"></see> class.</summary>
			/// <returns>The default alphabet.</returns>
			[[nodiscard]]
			static const std::string default_alphabet();
			
			/// <summary>Encode to base32.</summary>
			/// <param name="input">The data to be encoded.</param>
			/// <param name="alphabet">The alphabet to use for the encoding.</param>
			/// <returns>The encoded result.</returns>
			[[nodiscard]]
			static std::string encode(const std::string& input,
				std::string alphabet = default_alphabet());

			/// <summary>Decode from base32.</summary>
			/// <param name="input">The data to be decoded.</param>
			/// <param name="alphabet">The alphabet to use for the decoding.</param>
			/// <returns>The decoded result.</returns>
			[[nodiscard]]
			static std::string decode(const std::string& input,
				std::string alphabet = default_alphabet());
		};

		/// <summary>Base64 encoding/decoding class.</summary>
		class leccore_api base64 {
		public:
			/// <summary>Get the default base64 encoding alphabet
			/// used by the <see cref="base64"></see> class.</summary>
			/// <returns>The default alphabet.</returns>
			[[nodiscard]]
			static const std::string default_alphabet();

			/// <summary>Encode to base64.</summary>
			/// <param name="input">The data to be encoded.</param>
			/// <param name="alphabet">The alphabet to use for the encoding.</param>
			/// <returns>The encoded result.</returns>
			[[nodiscard]]
			static std::string encode(const std::string& input,
				std::string alphabet = default_alphabet());

			/// <summary>Decode from base64.</summary>
			/// <param name="input">The data to be decoded.</param>
			/// <param name="alphabet">The alphabet to use for the decoding.</param>
			/// <returns>The decoded result.</returns>
			[[nodiscard]]
			static std::string decode(const std::string& input,
				std::string alphabet = default_alphabet());
		};
	}
}
