//
// appcast.cpp - appcast implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

/*
*  Based on WinSparkle (http://winsparkle.org)
*
*  Copyright (C) 2009-2014 Vaclav Slavik
*
*  Permission is hereby granted, free of charge, to any person obtaining a
*  copy of this software and associated documentation files (the "Software"),
*  to deal in the Software without restriction, including without limitation
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,
*  and/or sell copies of the Software, and to permit persons to whom the
*  Software is furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*  DEALINGS IN THE SOFTWARE.
*
*/

#include "appcast.h"

// libexpat
#include <expat.h>

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "libexpat64d.lib")
#else
#pragma comment(lib, "libexpat64.lib")
#endif // _DEBUG

#else

#ifdef _DEBUG
#pragma comment(lib, "libexpat32d.lib")
#else
#pragma comment(lib, "libexpat32.lib")
#endif // _DEBUG

#endif // _WIN64

#include <vector>
#include <algorithm>

namespace liblec {
	namespace leccore {
#define MVAL(x) x
#define CONCAT3(a,b,c) MVAL(a)##MVAL(b)##MVAL(c)

#define NS_SPARKLE      "http://www.andymatuschak.org/xml-namespaces/sparkle"
#define NS_SEP          '#'
#define NS_SPARKLE_NAME(name) NS_SPARKLE "#" name

#define NODE_CHANNEL    "channel"
#define NODE_ITEM       "item"
#define NODE_RELNOTES   NS_SPARKLE_NAME("releaseNotesLink")
#define NODE_TITLE "title"
#define NODE_DESCRIPTION "description"
#define NODE_ENCLOSURE  "enclosure"
#define ATTR_URL        "url"
#define ATTR_VERSION    NS_SPARKLE_NAME("version")
#define ATTR_SHORTVERSION NS_SPARKLE_NAME("shortVersionString")
#define ATTR_OS         NS_SPARKLE_NAME("os")
#define OS_MARKER       "windows"

		// context data for the parser
		struct context_data {
			context_data(XML_Parser& p)
				: parser(p),
				in_channel(0), in_item(0), in_relnotes(0), in_title(0), in_description(0)
			{}

			// the parser we're using
			XML_Parser& parser;

			// is inside <channel>, <item> or <sparkle:releaseNotesLink>, <title>, or <description> respectively?
			int in_channel, in_item, in_relnotes, in_title, in_description;

			// parsed <item>s
			std::vector<check_update::update_info> items;
		};

		void XMLCALL on_start_element(void* data, const char* name, const char** attrs) {
			context_data& ctxt = *static_cast<context_data*>(data);

			if (strcmp(name, NODE_CHANNEL) == 0)
				ctxt.in_channel++;
			else if (ctxt.in_channel && strcmp(name, NODE_ITEM) == 0) {
				ctxt.in_item++;
				check_update::update_info item;
				ctxt.items.push_back(item);
			}
			else
				if (ctxt.in_item) {
					if (strcmp(name, NODE_RELNOTES) == 0)
						ctxt.in_relnotes++;
					else if (strcmp(name, NODE_TITLE) == 0)
						ctxt.in_title++;
					else if (strcmp(name, NODE_DESCRIPTION) == 0)
						ctxt.in_description++;
					else if (strcmp(name, NODE_ENCLOSURE) == 0) {
						const auto size = ctxt.items.size();
						for (int i = 0; attrs[i]; i += 2) {
							const char* name = attrs[i];
							const char* value = attrs[i + 1];

							if (strcmp(name, ATTR_URL) == 0)
								ctxt.items[size - 1].download_url = value;
							else if (strcmp(name, ATTR_VERSION) == 0)
								ctxt.items[size - 1].version = value;
							else if (strcmp(name, ATTR_OS) == 0)
								ctxt.items[size - 1].operating_system = value;
						}
					}
				}
		}

		void XMLCALL on_end_element(void* data, const char* name) {
			context_data& ctxt = *static_cast<context_data*>(data);

			if (ctxt.in_item && strcmp(name, NODE_RELNOTES) == 0)
				ctxt.in_relnotes--;
			else if (ctxt.in_item && strcmp(name, NODE_TITLE) == 0)
				ctxt.in_title--;
			else if (ctxt.in_item && strcmp(name, NODE_DESCRIPTION) == 0)
				ctxt.in_description--;
			else if (ctxt.in_channel && strcmp(name, NODE_ITEM) == 0) {
				ctxt.in_item--;
				if (ctxt.items[ctxt.items.size() - 1].operating_system == OS_MARKER) {
					// we've found os-specific <item>, no need to continue parsing
					XML_StopParser(ctxt.parser, XML_TRUE);
				}
			}
			else if (strcmp(name, NODE_CHANNEL) == 0) {
				ctxt.in_channel--;
				// we've reached the end of <channel> element,
				// so we stop parsing
				XML_StopParser(ctxt.parser, XML_TRUE);
			}
		}

		void XMLCALL on_text(void* data, const char* s, int len) {
			context_data& ctxt = *static_cast<context_data*>(data);
			const auto size = ctxt.items.size();

			if (ctxt.in_relnotes)
				ctxt.items[size - 1].release_notes.append(s, len);
			else if (ctxt.in_title)
				ctxt.items[size - 1].title.append(s, len);
			else if (ctxt.in_description)
				ctxt.items[size - 1].description.append(s, len);
		}

		bool is_windows_item(const check_update::update_info& item) {
			return item.operating_system == OS_MARKER;
		}

		bool load_appcast(const std::string& xml,
			liblec::leccore::check_update::update_info& details,
			std::string& error) {
			details = {};

			XML_Parser p = XML_ParserCreateNS(NULL, NS_SEP);
			if (!p) {
				error = "Failed to create XML parser";
				return false;
			}

			context_data ctxt(p);

			XML_SetUserData(p, &ctxt);
			XML_SetElementHandler(p, on_start_element, on_end_element);
			XML_SetCharacterDataHandler(p, on_text);

			XML_Status st = XML_Parse(p, xml.c_str(), static_cast<int>(xml.size()), XML_TRUE);

			if (st == XML_STATUS_ERROR) {
				error = std::string("XML parser error: ");
				error.append(XML_ErrorString(XML_GetErrorCode(p)));
				XML_ParserFree(p);
				return false;
			}

			XML_ParserFree(p);

			if (ctxt.items.empty()) {
				error = "Invalid appcast data";
				return false;
			}

			// Search for first <item> which has "sparkle:os=windows" attribute.
			// If none, use the first item.
			std::vector<check_update::update_info>::iterator it = std::find_if(ctxt.items.begin(), ctxt.items.end(), is_windows_item);
			if (it != ctxt.items.end())
				details = *it;
			else
				details = ctxt.items.front();

			return true;
		}
	}
}
