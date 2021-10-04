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
		/// <summary>PC information class.</summary>
		class leccore_api pc_info {
		public:
			/// <summary>PC details.</summary>
			using pc_details = struct {
				/// <summary>The user-defined name of the Computer, e.g. "John Doe's Shop Laptop".</summary>
				std::string name;

				/// <summary>The manufacturer of the PC, e.g. Dell.</summary>
				std::string manufacturer;

				/// <summary>The model of the computer, e.g. Latitude E7250.</summary>
				std::string model;

				/// <summary>The system type, e.g. x64-based PC.</summary>
				std::string system_type;

				/// <summary>The serial number of the BIOS. Uniquely identifies the PC.</summary>
				std::string bios_serial_number;

				/// <summary>The serial number of the PC's motherboard.</summary>
				std::string motherboard_serial_number;
			};

			/// <summary>Operating System info type.</summary>
			using os_info = struct {
				/// <summary>The name of the OS, e.g. Microsoft Windows 10 Home Single Language.</summary>
				std::string name;

				/// <summary>The architecture of the OS, e.g. 64-bit.</summary>
				std::string architecture;

				/// <summary>The version number of the OS, e.g. 10.0.18363.</summary>
				std::string version;
			};

			/// <summary>Central Processing unit info type.</summary>
			using cpu_info = struct {
				/// <summary>The name of the CPU, e.g. Intel(R) Core(TM) i5-5300U CPU @ 2.30GHz.</summary>
				std::string name;

				/// <summary>The status of the CPU.</summary>
				std::string status;

				/// <summary>The manufacturer of the CPU, e.g. AMD.</summary>
				std::string manufacturer;

				/// <summary>The number of cores in the CPU, e.g. 2.</summary>
				int cores;

				/// <summary>The number of logical processors in the CPU, e.g. 4.</summary>
				int logical_processors;

				/// <summary>The base speed of the CPU in GHz, e.g. 2.29GHz.</summary>
				double base_speed;
			};

			/// <summary>Graphics Processing Unit type.</summary>
			using gpu_info = struct {
				/// <summary>The name of the GPU, e.g. Intel(R) UHD 620.</summary>
				std::string name;

				/// <summary>The status of the GPU.</summary>
				std::string status;

				/// <summary>The size of the dedicated video memory, in bytes.</summary>
				unsigned long long dedicated_vram;

				/// <summary>The size of the total available graphics memory, in bytes.</summary>
				unsigned long long total_graphics_memory;
			};

			/// <summary>Monitor video mode type.</summary>
			using video_mode = struct {
				/// <summary>The current horizontal resolution of the display,
				/// in pixels, e.g. 1920.</summary>
				int horizontal_resolution;

				/// <summary>The current vertical resolution of the display,
				/// in pixels, e.g. 1080.</summary>
				int vertical_resolution;

				/// <summary>The resolution name, e.g. Full HD.</summary>
				std::string resolution_name;

				/// <summary>The pixel clock rate, in Hz.</summary>
				unsigned long long pixel_clock_rate;

				/// <summary>The refresh rate of the display, in Hz.</summary>
				double refresh_rate;

				/// <summary>The physical size of the display in inches.</summary>
				/// <remarks>This is the length of the diagonal across the screen.</remarks>
				double physical_size;
			};

			/// <summary>Monitor information type.</summary>
			using monitor_info = struct {
				/// <summary>The instance name of the monitor.</summary>
				std::string instance_name;

				/// <summary>The manufacturer of the monitor, e.g. Lenovo.</summary>
				std::string manufacturer;

				/// <summary>The product code ID.</summary>
				std::string product_code_id;

				/// <summary>The manufacture year.</summary>
				short year_of_manufacture;

				/// <summary>The manufacture week.</summary>
				short week_of_manufacture;

				/// <summary>The supported video modes, as defined in the <see cref="video_mode"></see> type.</summary>
				std::vector<video_mode> supported_modes;
			};

			/// <summary>Random Access Memory chip type.</summary>
			using ram_chip = struct {
				/// <summary>The tag of the RAM chip.</summary>
				std::string tag;

				/// <summary>The memory type, e.g. DDR3.</summary>
				std::string type;

				/// <summary>The form factor of the RAM, e.g. SODIMM</summary>
				std::string form_factor;

				/// <summary>The part number of the RAM chip. Uniquely identifies the
				/// product.</summary>
				std::string part_number;

				/// <summary>The status of the RAM chip.</summary>
				std::string status;

				/// <summary>The manufacturer of the RAM chip, e.g. Samsung.</summary>
				std::string manufacturer;

				/// <summary>The capacity of the RAM chip, in bytes.</summary>
				unsigned long long capacity;

				/// <summary>The speed of the RAM chip, in MHz.</summary>
				int speed;
			};

			/// <summary>Overall PC Random Access Memory type.</summary>
			using ram_info = struct {
				/// <summary>The overall size of the RAM that the PC has, in bytes.</summary>
				unsigned long long size;

				/// <summary>The overall speed of the system memory, in MHz.</summary>
				int speed;

				/// <summary>The ram chips installed within the PC as defined by the
				/// <see cref="ram_chip"></see> type.</summary>
				std::vector<ram_chip> ram_chips;
			};

			/// <summary>PC Drive type.</summary>
			using drive_info = struct {
				/// <summary>The index of the drive, e.g. the system drive is normally index 0.</summary>
				unsigned int index;

				/// <summary>The device ID of the drive.</summary>
				std::string device_id;

				/// <summary>The drive model. Uniquely identifies the product.</summary>
				std::string model;

				/// <summary>The drive's serial number. Uniquely identifies the drive.</summary>
				std::string serial_number;

				/// <summary>The storage type, e.g. HDD.</summary>
				std::string storage_type;

				/// <summary>The bus type, e.g. SATA.</summary>
				std::string bus_type;

				/// <summary>The media type, e.g. External hard disk media.</summary>
				std::string media_type;

				/// <summary>The status of the drive.</summary>
				std::string status;

				/// <summary>The size of the drive, in bytes.</summary>
				unsigned long long size;
			};

			/// <summary>Battery status.</summary>
			enum class battery_status {
				/// <summary>Indicates that the battery is currently charging.</summary>
				charging,
				
				/// <summary>Indicates that the battery is currently discharging.</summary>
				discharging,
				
				/// <summary>Indicates that the battery is currently idle.</summary>
				ac_line
			};

			/// <summary>Battery type.</summary>
			using battery_info = struct battery_info {
				/// <summary>The name of the battery, e.g. DELL GV7HC59.</summary>
				std::string name;

				/// <summary>The battery's manufacturer, e.g. LGC-LGC3.6.</summary>
				std::string manufacturer;

				/// <summary>The battery's serial number. Uniquely identifies the battery.</summary>
				std::string serial_number;

				/// <summary>The battery's unique ID.</summary>
				std::string unique_id;

				/// <summary>The battery's designed capacity, in mWh.</summary>
				int designed_capacity;

				/// <summary>The battery's current fully charged capacity, in mWh. When the
				/// battery is brand new this value is usually equal to or just below the
				/// designed capacity as defined in <see cref="designed_capacity"></see>.</summary>
				int fully_charged_capacity;

				/// <summary>The battery's current health as a percentage. The higher the better.
				/// A new battery is typically in the 90s and a battery nearing the end of it's usable
				/// life is typically below the 30s.</summary>
				double health;

				/// <summary>The battery's current capacity, in mWh. When the battery is full this value
				/// is typically equal to or just above the fully charged capacity as defined
				/// in <see cref="fully_charged_capacity"></see>. A value of -1 indicates that the
				/// current capacity is unknown.</summary>
				int current_capacity;

				/// <summary>The current battery level as a percentage.</summary>
				double level;

				/// <summary>The battery's current voltage, in mV. -1 means unknown.</summary>
				int current_voltage;

				/// <summary>The battery's current charge rate, in mW. A positive value indicates that the
				/// battery is currently charging, a negative value means it's discharging and 0 means it's
				/// idle.</summary>
				int current_charge_rate;

				/// <summary>The status of the battery as defined in <see cref="battery_status"></see>.</summary>
				battery_status status;

				/// <summary>Equality operator useful for checking if any battery information has
				/// changed since the last query.</summary>
				/// <param name="param">The <see cref="battery_info"></see> to compare to.</param>
				/// <returns>Returns true if there are no changes, else false.</returns>
				bool operator==(const pc_info::battery_info& param) {
					return name == param.name &&
						manufacturer == param.manufacturer &&
						serial_number == param.serial_number &&
						unique_id == param.unique_id &&
						designed_capacity == param.designed_capacity &&
						fully_charged_capacity == param.fully_charged_capacity &&
						health == param.health &&
						current_capacity == param.current_capacity &&
						level == param.level &&
						current_voltage == param.current_voltage &&
						current_charge_rate == param.current_charge_rate &&
						status == param.status;
				}

				/// <summary>In-equality operator. The opposite of <see cref="operator=="></see>.</summary>
				/// <param name="param">The <see cref="battery_info"></see> to compare to.</param>
				/// <returns>Returns true if there are changes, else false.</returns>
				bool operator!=(const pc_info::battery_info& param) {
					return !operator==(param);
				}
			};

			/// <summary>The overall power status of the PC.</summary>
			enum class power_status {
				/// <summary>The battery level is high.</summary>
				high,
				
				/// <summary>The battery level is medium.</summary>
				medium,
				
				/// <summary>The battery level is low.</summary>
				low,
				
				/// <summary>The battery level is critical.</summary>
				critical,

				/// <summary>Battery is charging, and its charge level is high.</summary>
				high_charging,
				
				/// <summary>Battery is charging, and its charge level is low.</summary>
				low_charging,
				
				/// <summary>Battery is charging, and its charge level is critical.</summary>
				critical_charging,

				/// <summary>Battery is charging, and its charge level is medium.</summary>
				charging,
				
				/// <summary>There is no system battery.</summary>
				no_battery,
				
				/// <summary>The power status is unknown.</summary>
				unknown
			};

			/// <summary>PC power type.</summary>
			using power_info = struct {
				/// <summary>Whether the PC is using AC power.</summary>
				bool ac;

				/// <summary>The power status of the PC as defined in
				/// <see cref="power_status"></see>.</summary>
				power_status status;

				/// <summary>The overall battery level as a percentage. -1 means unknown.</summary>
				int level;

				/// <summary>The estimated battery lifetime remaining.</summary>
				std::string lifetime_remaining;

				/// <summary>The batteries currently installed within the PC, as defined
				/// in the <see cref="battery_info"></see> type.</summary>
				std::vector<battery_info> batteries;
			};

			pc_info();
			~pc_info();

			/// <summary>Get PC details.</summary>
			/// <param name="info">The PC's details as defined in the
			/// <see cref="pc_details"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool pc(pc_details& info,
				std::string& error);

			/// <summary>Get information about the PC's Operating System.</summary>
			/// <param name="info">The OS information as defined by the
			/// <see cref="os_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool os(os_info& info,
				std::string& error);

			/// <summary>Get information about the PC's Central Processing Units.</summary>
			/// <param name="info">A list of CPUs as defined by the
			/// <see cref="cpu_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool cpu(std::vector<cpu_info>& info,
				std::string& error);


			/// <summary>Get information about the PC's Graphics Processing
			/// Units.</summary>
			/// <param name="info">A list of GPUs as defined by the
			/// <see cref="gpu_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool gpu(std::vector<gpu_info>& info,
				std::string& error);

			/// <summary>Get information about the PC's monitors.</summary>
			/// <param name="info">A list of monitors as defined by the
			/// <see cref="monitor_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool monitor(std::vector<monitor_info>& info,
				std::string& error);

			/// <summary>Get PC Random Access Memory information.</summary>
			/// <param name="info">The RAM info as defined by the
			/// <see cref="ram_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool ram(ram_info& info,
				std::string& error);

			/// <summary>Get PC drive information.</summary>
			/// <param name="info">A list of drives connected to the PC
			/// as defined by the <see cref="drive_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool drives(std::vector<drive_info>& info,
				std::string& error);

			/// <summary>Get PC power information.</summary>
			/// <param name="info">The PC power information as defined in the
			/// <see cref="power_info"></see> type.</param>
			/// <param name="error">Error information.</param>
			/// <returns>Returns true if successful, else false.</returns>
			bool power(power_info& info,
				std::string& error);

			/// <summary>Convert <see cref="battery_status"></see> to a string.</summary>
			/// <param name="status">The battery status.</param>
			/// <returns>The string.</returns>
			static std::string to_string(battery_status status);

			/// <summary>Convert <see cref="power_status"></see> to a string.</summary>
			/// <param name="flag">The power status.</param>
			/// <returns>The string.</returns>
			static std::string to_string(power_status flag);

		private:
			class impl;
			impl& _d;

			// Copying an object of this class is not allowed
			pc_info(const pc_info&) = delete;
			pc_info& operator=(const pc_info&) = delete;
		};
	}
}
