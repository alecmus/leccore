//
// pc_info.h - pc information interface
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
#include <vector>

namespace liblec {
	namespace leccore {
		class leccore_api pc_info {
		public:
			using drive_info = struct {
				unsigned int index;
				std::string model;
				std::string serial_number;
				std::string interface_type;
				std::string media_type;
				std::string status;
				unsigned long long size;
			};

			enum class battery_status { charging, discharging, ac_line, };

			struct battery_info {
				std::string name;
				std::string manufacturer;
				std::string serial_number;
				std::string unique_id;
				int designed_capacity = 0;
				int fully_charged_capacity = 0;
				double health = 0.0;
				int current_capacity = 0;							// current battery capacity in mWh, -1 means unknown
				double level = 0.0;									// current battery level as a percentage
				int current_voltage = 0;							// current battery capacity in mV, -1 means unknown
				int current_charge_rate = 0;						// current battery charge rate in mW
				battery_status status = battery_status::ac_line;	// battery status
			};

			enum class power_status { high, low, critical, charging, no_battery, unknown, };

			struct power_info {
				bool ac = false;
				power_status status;
				int level = 0;						// overall battery level as a percentage, -1 means unknown
				std::string lifetime_remaining;		// estimated lifetime remaining in hours and minutes
				std::vector<battery_info> batteries;
			};

			pc_info();
			~pc_info();

			bool bios_serial(std::string& serial,
				std::string& error);
			bool drives(std::vector<drive_info>& info,
				std::string& error);
			bool power(power_info& info,
				std::string& error);

			static std::string to_string(battery_status status);
			static std::string to_string(power_status flag);

		private:
			class impl;
			impl& d_;

			// Copying an object of this class is not allowed
			pc_info(const pc_info&) = delete;
			pc_info& operator=(const pc_info&) = delete;
		};
	}
}
