//
// database.h - database interface
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

#include <any>
#include <map>
#include <string>
#include <vector>

namespace liblec {
	namespace leccore {
		namespace database {
			/// <summary>Database blob type.</summary>
			using blob = struct {
				/// <summary>The raw blob data.</summary>
				std::string data;
			};

			/// <summary>Database row; a list of (column, value) pairs. The map's "key" is
			/// the column name and it's "value" is the value under that column for the given row, e.g.
			/// { { "Name", "John" }, { "Surname", "Doe" }, { "Height", 172 }, { "Image", _blob_data_ } }
			/// </summary>
			using row = std::map<std::string, std::any>;

			/// <summary>Table type.</summary>
			using table = struct {
				/// <summary>The name of the table, e.g. "Members"</summary>
				std::string name;

				/// <summary>The list of the table's columns, e.g. { "Name", "Surname", "Height", "Image" }.</summary>
				std::vector<std::string> columns;

				/// <summary>The table data. A list of rows as defined in the <see cref="row"></see> type.</summary>
				std::vector<row> data;
			};

			/// <summary>Helper class for extracting values from a std::any. Strictly added to
			/// enable more terse code and make the code more readable.</summary>
			/// <remarks>If the std::any actually contains a different data type than what you expect
			/// the methods in this class will throw, so make sure to use a try-catch block in the calling
			/// code.</remarks>
			class leccore_api get {
			public:
				/// <summary>Extract integer value.</summary>
				/// <param name="value">The std::any containing the value.</param>
				/// <returns>The integer.</returns>
				static int integer(const std::any& value);

				/// <summary>Extract double value.</summary>
				/// <param name="value">The std::any containing the value.</param>
				/// <returns>The double.</returns>
				static double real(const std::any& value);

				/// <summary>Extract text.</summary>
				/// <param name="value">The std::any containing the text.</param>
				/// <returns>The text.</returns>
				static std::string text(const std::any& value);

				/// <summary>Extract blob data as defined in the <see cref="blob"></see> type.</summary>
				/// <param name="value">The std::any containing the blob data.</param>
				/// <returns>The blob.</returns>
				static blob blob(const std::any& value);
			};

			/// <summary>Database connection class.</summary>
			class leccore_api connection {
			public:
				/// <summary>Constructor.</summary>
				/// <param name="type">The database connection type, e.g. "sqlcipher".</param>
				/// <param name="connection_string">The connection string,
				/// e.g. "C:\ProgramData\com.github.alecmus\PC Info\data.db".</param>
				/// <param name="password">The database password. This must be private. Use an
				/// empty string to use a plain database.</param>
				connection(const std::string& type,
					const std::string& connection_string,
					const std::string& password);
				~connection();

				/// <summary>Check whether there is an existing connection to the database.</summary>
				/// <returns>True if the connection exists, else false.</returns>
				bool connected();

				/// <summary>Connect to the database.</summary>
				/// <param name="error">Error information.</param>
				/// <returns>Returns true if successful, else false.</returns>
				bool connect(std::string& error);

				/// <summary>Disconnect from the database.</summary>
				/// <param name="error">Error information.</param>
				/// <returns>Returns true if successful, else false.</returns>
				bool disconnect(std::string& error);

				/// <summary>Execute an sql statement.</summary>
				/// <param name="sql">The query (supports placeholders for binding),
				/// e.g. INSERT INTO Members VALUES(?, ?, ?, ?);</param>
				/// <param name="values">The values to bind to the <see cref="sql"></see> statement, e.g.
				/// { "John", "Doe", 172, blob{ jpg_file_data } }</param>
				/// <param name="error">Error information.</param>
				/// <returns>Returns true if successful, else false.</returns>
				/// <remarks>The use of placeholders is optional. You can make the sql statement yourself
				/// and leave the <see cref="values"></see> parameter empty but this is not recommended
				/// because of sql injection attacks and issues like the proper handling of special
				/// characters, which quickly becomes tedious and is error prone.</remarks>
				bool execute(const std::string& sql, const std::vector<std::any>& values, std::string& error);

				/// <summary>Execute an sql query.</summary>
				/// <param name="sql">The query, e.g. "SELECT Name, Surname FROM Members WHERE UniqueID = ?;"</param>
				/// <param name="values">The values to bind to the <see cref="sql"></see> statement, e.g.
				/// { "T432FJ" }</param>
				/// <param name="results">The results of the query as defined in the
				/// <see cref="table"></see> type.</param>
				/// <param name="error">Error information.</param>
				/// <returns>Returns true if successful, else false.</returns>
				bool execute_query(const std::string& sql, const std::vector<std::any>& values, table& results, std::string& error);

			private:
				class impl;
				impl& _d;

				// Default constructor and copying an object of this class are not allowed
				connection() = delete;
				connection(const connection&) = delete;
				connection& operator=(const connection&) = delete;
			};
		}
	}
}
