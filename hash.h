//
// hash.h - hash interface
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#pragma once

#if defined(LECCORE_EXPORTS)
#include "leccore.h"
#else
#include <liblec/leccore.h>
#endif

#include <string>
#include <vector>
#include <map>

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

		/// <summary>File hashing class.</summary>
		class leccore_api hash_file {
		public:
			hash_file();
			~hash_file();

			/// <summary>Hashing algorithm.</summary>
			enum class algorithm {
				/// <summary>SHA512 hash.</summary>
				sha256,

				/// <summary>SHA512 hash.</summary>
				sha512,
			};

			/// <summary>Hash results. Key is the algorithm and value is the hash.</summary>
			/// <remarks>Allows running multiple hashes in one sweep, which is far much more efficient
			/// than running one hash at a time because in that case the file data has to be read again.
			/// </remarks>
			using hash_results = std::map<algorithm, std::string>;

			
			/// <summary>Start hashing.</summary>
			/// <param name="fullpath">The full path to the file, including the extension.</param>
			/// <param name="algorithms">The list of algorithms to use.</param>
			/// <remarks>This method returns almost immediately. The actual hashing is executed
			/// on a different thread. To check the status of the hashing use the <see cref="hashing"></see>
			/// method.</remarks>
			void start(const std::string& fullpath,
				const std::vector<algorithm>& algorithms);

			/// <summary>Check whether hashing is still in progress.</summary>
			/// <returns>Returns true if the hashing process is still underway and false otherwise.</returns>
			/// <remarks>After calling <see cref="start"></see> call this method in a loop or a timer, depending on
			/// your kind of app, then call <see cref="result"></see> once it returns false.</remarks>
			bool hashing();

			/// <summary>Get the results of the hashing operation.</summary>
			/// <param name="results">The results of the hashing operation as defined in
			/// the <see cref="hash_results"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if the hashing was successful, else false. When false the
			/// error information is written back to <see cref="error"></see>.</returns>
			bool result(hash_results& results,
				std::string& error);

		private:
			class impl;
			impl& _d;

			// Copying an object of this class is not allowed
			hash_file(const hash_file&) = delete;
			hash_file& operator=(const hash_file&) = delete;
		};
	}
}
