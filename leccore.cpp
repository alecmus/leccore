//
// leccore.h - leccore implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "leccore.h"
#include "versioninfo.h"

#include <Windows.h>
#include <strsafe.h>	// for StringCchPrintfA

#include <thread>
#include <chrono>

#include <sstream>
#include <unordered_map>

// crypto++
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

// DllMain function
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	default:
		break;
	}
	return TRUE;
}

std::string liblec::leccore::version() {
	return leccorename + std::string(" ") + leccoreversion + std::string(", ") + leccoredate;
}

namespace nmReadableSize {
	/*
	** adapted from wxWidgets whose license is as follows:
	**
	** Name:        src / common / filename.cpp
	** Purpose:     wxFileName - encapsulates a file path
	** Author:      Robert Roebling, Vadim Zeitlin
	** Modified by:
	** Created:     28.12.2000
	** RCS-ID:      $Id$
	** Copyright:   (c) 2000 Robert Roebling
	** Licence:     wxWindows licence
	*/

	// size conventions
	enum SizeConvention {
		SIZE_CONV_TRADITIONAL,  // 1024 bytes = 1KB
		SIZE_CONV_SI            // 1000 bytes = 1KB
	};

	std::string GetHumanReadableSize(const long double& dSize,
		const std::string& nullsize,
		int precision,
		SizeConvention conv) {
		// deal with trivial case first
		if (dSize == 0)
			return nullsize;

		// depending on the convention used the multiplier may be either 1000 or
		// 1024 and the binary infix may be empty (for "KB") or "i" (for "KiB")
		long double multiplier = 1024.;

		switch (conv) {
		case SIZE_CONV_TRADITIONAL:
			// nothing to do, this corresponds to the default values of both
			// the multiplier and infix string
			break;

		case SIZE_CONV_SI:
			multiplier = 1000;
			break;
		}

		const long double kiloByteSize = multiplier;
		const long double megaByteSize = multiplier * kiloByteSize;
		const long double gigaByteSize = multiplier * megaByteSize;
		const long double teraByteSize = multiplier * gigaByteSize;

		const long double bytesize = dSize;

		size_t const cchDest = 256;
		char pszDest[cchDest];

		if (bytesize < kiloByteSize)
			StringCchPrintfA(pszDest, cchDest, "%.*fB", 0, dSize);
		else if (bytesize < megaByteSize)
			StringCchPrintfA(pszDest, cchDest, "%.*fKB", precision, bytesize / kiloByteSize);
		else if (bytesize < gigaByteSize)
			StringCchPrintfA(pszDest, cchDest, "%.*fMB", precision, bytesize / megaByteSize);
		else if (bytesize < teraByteSize)
			StringCchPrintfA(pszDest, cchDest, "%.*fGB", precision, bytesize / gigaByteSize);
		else
			StringCchPrintfA(pszDest, cchDest, "%.*fTB", precision, bytesize / teraByteSize);

		return pszDest;
	}
}

std::string liblec::leccore::format_size(unsigned long long size, unsigned short precision) {
	return nmReadableSize::GetHumanReadableSize(static_cast<long double>(size), "0B", precision,
		nmReadableSize::SIZE_CONV_TRADITIONAL);
}

void liblec::leccore::sleep(unsigned long long milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

liblec::leccore::password_quality_specs liblec::leccore::password_quality(const std::string& password) {
	// returns the number of duplicates in a given string
	auto number_of_duplicates = [](const std::string& str) {
		// K = character, T = occurences
		std::unordered_map<char, int> pairs;
		for (const auto& character : str)
			pairs[character]++;

		int duplicates = 0;
		for (const auto& [character, occurences] : pairs)
			duplicates += (occurences - 1);

		return duplicates;
	};

	password_quality_specs quality;
	quality.strength = 0.f;

	if (password.empty()) {
		quality.issues.push_back("No password");
		quality.issues_summary = "No password";
		return quality;
	}

	// number of items of each type
	float hits_lower = 0;
	float hits_upper = 0;
	float hits_digit = 0;
	float hits_special = 0;

	for (const auto& character : password) {
		if (islower(character))
			hits_lower++;
		else
			if (isupper(character))
				hits_upper++;
			else
				if (isdigit(character))
					hits_digit++;
				else
					hits_special++;
	}

	// check for duplicate characters
	int duplicates = number_of_duplicates(password);

	// add strength at different proportions for different elements
	quality.strength = quality.strength +
		hits_lower * 3.f +
		hits_upper * 3.f +
		hits_digit * 2.f +	// digits have the least weight
		hits_special * 5.f;	// special characters have the greatest weight

	// add strength according to complexity of mixing
	const float mix_factor =
		(hits_lower * hits_upper) +
		(hits_lower * hits_digit) +
		(hits_lower * hits_special) +
		(hits_upper * hits_digit) +
		(hits_upper * hits_special) +
		(hits_digit * hits_special);

	quality.strength = quality.strength + mix_factor - duplicates * 2.5f;

	// impose limits
	quality.strength = max(quality.strength, 0.f);
	quality.strength = min(quality.strength, 100.f);

	// issues
	if (hits_lower < 3) {
		if (hits_lower == 0)
			quality.issues.push_back("No lowercase characters");
		else
			quality.issues.push_back("Few lowercase characters");
	}

	if (hits_upper < 3) {
		if (hits_upper == 0)
			quality.issues.push_back("No uppercase characters");
		else
			quality.issues.push_back("Few uppercase characters");
	}

	if (hits_special < 3) {
		if (hits_special == 0)
			quality.issues.push_back("No special characters");
		else
			quality.issues.push_back("Few special characters");
	}

	if (hits_digit < 3) {
		if (hits_digit == 0)
			quality.issues.push_back("No digits");
		else
			quality.issues.push_back("Few digits");
	}

	if (duplicates > 2)
		quality.issues.push_back("Duplicate characters");

	// issues summary
	for (auto issue : quality.issues) {
		if (quality.issues_summary.empty())
			quality.issues_summary = issue;
		else {
			if (!issue.empty())
				issue[0] = tolower(issue[0]);

			quality.issues_summary += ", " + issue;
		}
	}

	return quality;
}

std::string liblec::leccore::round_off::to_string(const double& d, int precision) {
	std::stringstream ss;
	ss << std::fixed;
	ss.precision(precision);
	ss << d;
	return ss.str();
}

double liblec::leccore::round_off::to_double(const double& d, int precision) {
	int y = (int)d;
	double z = d - (double)y;
	double m = pow(10.0, (double)precision);
	double q = z * m;
	double r = round(q);

	return static_cast<double>(y) + (1.0 / m) * r;
}

float liblec::leccore::round_off::to_float(const float& f, int precision) {
	int y = (int)f;
	float z = f - (float)y;
	float m = pow(10.f, (float)precision);
	float q = z * m;
	float r = round(q);

	return static_cast<float>(y) + (1.f / m) * r;
}
