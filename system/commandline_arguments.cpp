//
// commandline_arguments.cpp - command line arguments parser implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../system.h"
#include <Windows.h>

namespace liblec {
	namespace leccore {
		/*
		** win32 commandline parser, adapted from
		** http://www.codeguru.com/cpp/w-p/win32/article.php/c1427/A-Simple-Win32-CommandLine-Parser.htm
		**
		** Usage: simply declare a commandline_arguments::impl object.
		** The class inherits from a vector and will be automatically
		** populated with the commandline arguments.
		*/
		class commandline_arguments::impl : public std::vector<char*> {
		public:
			impl() {
				// Save local copy of the command line string, because
				// parse() modifies this string while parsing it.
				PSZ cmdline = GetCommandLineA();
				m_cmdline = new char[strlen(cmdline) + 1];
				if (m_cmdline) {
					strcpy_s(m_cmdline, strlen(cmdline) + 1, cmdline);
					parse();
				}
			}

			impl(const char* cc) {
				// Save local copy of the command line string, because
				// parse() modifies this string while parsing it.
				PSZ cmdline = (PSZ)cc;
				m_cmdline = new char[strlen(cmdline) + 1];
				if (m_cmdline) {
					strcpy_s(m_cmdline, strlen(cmdline) + 1, cmdline);
					parse();
				}
			}

			~impl() {
				if (m_cmdline)
					delete m_cmdline;
			}

		private:
			PSZ m_cmdline;

			////////////////////////////////////////////////////////////////////////////////
			// Parse m_cmdline into individual tokens, which are delimited by spaces. If a
			// token begins with a quote, then that token is terminated by the next quote
			// followed immediately by a space or terminator.  This allows tokens to contain
			// spaces.
			// This input string:     This "is" a ""test"" "of the parsing" alg"o"rithm.
			// Produces these tokens: This, is, a, "test", of the parsing, alg"o"rithm
			////////////////////////////////////////////////////////////////////////////////
			void parse() {
				enum {
					TERM = '\0',
					QUOTE = '\"'
				};

				bool in_quotes = false;
				PSZ pargs = m_cmdline;

				while (*pargs) {
					// skip leading whitespace
					while (isspace(*pargs))
						pargs++;

					// see if this token is quoted
					in_quotes = (*pargs == QUOTE);

					// skip leading quote
					if (in_quotes)
						pargs++;

					// store position of current token
					push_back(pargs);

					// Find next token.
					// NOTE: Args are normally terminated by whitespace, unless the
					// arg is quoted.  That's why we handle the two cases separately,
					// even though they are very similar.
					if (in_quotes) {
						// find next quote followed by a space or terminator
						while (*pargs &&
							!(*pargs == QUOTE && (isspace(pargs[1]) || pargs[1] == TERM)))
							pargs++;

						if (*pargs) {
							// terminate token
							*pargs = TERM;

							if (pargs[1]) {
								// quoted token not followed by a terminator
								// advance to next token
								pargs += 2;
							}
						}
					}
					else {
						// skip to next non-whitespace character
						while (*pargs && !isspace(*pargs))
							pargs++;

						if (*pargs && isspace(*pargs)) {
							// end of token, terminate it
							*pargs = TERM;

							// advance to next token or terminator
							pargs++;
						}
					}
				}
			}
		};

		const std::vector<std::string>& commandline_arguments::get() {
			// must be static so it persists through-out app session
			static std::vector<std::string> tokens;

			// do the parsing only once per app session
			if (!tokens.empty())
				return tokens;

			impl d_;

			tokens.reserve(d_.size());

			for (const auto& it : d_)
				tokens.push_back(it);

			return tokens;
		}

		bool commandline_arguments::contains(const std::string& token) {
			const auto& tokens = get();

			if (std::find(tokens.begin(), tokens.end(), token) != tokens.end())
				return true;
			else
				return false;
		}
	}
}
