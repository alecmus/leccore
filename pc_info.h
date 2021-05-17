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
			using pc_details = struct {
				std::string name;
				std::string manufacturer;
				std::string model;
				std::string system_type;
				std::string bios_serial_number;
				std::string motherboard_serial_number;
			};

			using os_info = struct {
				std::string name;
				std::string architecture;
				std::string version;
			};

			using cpu_info = struct {
				std::string name;
				std::string status;
				std::string manufacturer;
				int cores;
				int logical_processors;
				double base_speed;			// in GHz
			};

			using gpu_info = struct {
				std::string name;
				std::string status;
				int horizontal_resolution;
				int vertical_resolution;
				int refresh_rate;			// in Hz
				int ram;					// in bytes
			};

			using ram_chip = struct {
				std::string tag;
				std::string type;
				std::string form_factor;
				std::string part_number;
				std::string status;
				std::string manufacturer;
				unsigned long long capacity;	// in bytes
				int speed;						// in MHz
			};

			using ram_info = struct {
				unsigned long long size;		// in bytes
				int speed;						// in MHz
				std::vector<ram_chip> ram_chips;
			};

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

			using battery_info = struct {
				std::string name;
				std::string manufacturer;
				std::string serial_number;
				std::string unique_id;
				int designed_capacity;
				int fully_charged_capacity;
				double health;
				int current_capacity;			// in mWh, -1 means unknown
				double level;					// current battery level as a percentage
				int current_voltage;			// in mV, -1 means unknown
				int current_charge_rate;		// in mW
				battery_status status;			// battery status
			};

			enum class power_status { high, low, critical, charging, no_battery, unknown, };

			using power_info = struct {
				bool ac;
				power_status status;
				int level;							// overall battery level as a percentage, -1 means unknown
				std::string lifetime_remaining;		// estimated lifetime remaining in hours and minutes
				std::vector<battery_info> batteries;
			};

			pc_info();
			~pc_info();

			bool pc(pc_details& info,
				std::string& error);
			bool os(os_info& info,
				std::string& error);
			bool cpu(std::vector<cpu_info>& info,
				std::string& error);
			bool gpu(std::vector<gpu_info>& info,
				std::string& error);
			bool ram(ram_info& info,
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
