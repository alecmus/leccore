//
// pc_info.cpp - pc information implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "../pc_info.h"
#include "get_power_info.h"
#include "../leccore_common.h"
#include "get_wmi_data.h"
#include <map>
#include <any>

using namespace std;
using namespace liblec::leccore;

class pc_info::impl {
	
public:
	impl() {
		// Initialize COM
		if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED))) {
			// Set general COM security levels
			if (FAILED(CoInitializeSecurity(
				NULL,
				-1,                          // COM authentication
				NULL,                        // Authentication services
				NULL,                        // Reserved
				RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
				RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
				NULL,                        // Authentication info
				EOAC_NONE,                   // Additional capabilities 
				NULL                         // Reserved
			))) {
				// to-do: log
			}
		}
		else {
			// to-do: log
		}
	}

	~impl() {
		CoUninitialize();
	}

	bool get_info(const string& class_name,
		const vector<string>& properties,
		map<string, vector<any>>& data,
		string& error) {
		vector<bstr_t> properties_(properties.size());
		for (size_t i = 0; i < properties.size(); i++)
			properties_[i] = properties[i].c_str();

		return get_wmi_data(
			class_name.c_str(),
			properties_,
			data,
			error);
	}
};

pc_info::pc_info() :
	d_(*new impl()) {}

pc_info::~pc_info() {
	delete& d_;
}

bool pc_info::pc(pc_info::pc_details& info, std::string& error) {
	info = {};

	auto bios_serial = [&](std::string& serial,
		std::string& error)->bool {
			serial.clear();
			map<string, vector<any>> data;
			if (d_.get_info("Win32_Bios", { "SerialNumber" }, data, error)) {
				try {
					if (!data.empty() && !data.at("SerialNumber").empty())
						serial = any_cast<string>(data.at("SerialNumber")[0]);
					return true;
				}
				catch (const std::exception& e) {
					error = e.what();
					return false;
				}
			}
			else
				return false;
	};

	auto baseboard_serial = [&](std::string& serial,
		std::string& error)->bool {
			serial.clear();
			map<string, vector<any>> data;
			if (d_.get_info("Win32_BaseBoard", { "SerialNumber" }, data, error)) {
				try {
					if (!data.empty() && !data.at("SerialNumber").empty())
						serial = any_cast<string>(data.at("SerialNumber")[0]);
					return true;
				}
				catch (const std::exception& e) {
					error = e.what();
					return false;
				}
			}
			else
				return false;
	};

	if (!bios_serial(info.bios_serial_number, error)) {
		// to-do: log
		error.clear();
	}

	if (!baseboard_serial(info.motherboard_serial_number, error)) {
		// to-do: log
		error.clear();
	}

	map<string, vector<any>> data;
	if (d_.get_info("Win32_ComputerSystem", { "Name", "Manufacturer", "Model", "SystemType" }, data, error)) {
		try {
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					if (property == "Name") {
						info.name = any_cast<string>(values[0]);
					}
					if (property == "Manufacturer") {
						info.manufacturer = any_cast<string>(values[0]);
					}
					if (property == "Model") {
						info.model = any_cast<string>(values[0]);
					}
					if (property == "SystemType") {
						info.system_type = any_cast<string>(values[0]);
					}
				}
			}
			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}
	else
		return false;
}

bool pc_info::os(os_info& info,
	std::string& error) {
	info = {};
	map<string, vector<any>> data;
	if (d_.get_info("Win32_OperatingSystem", { "Caption", "OSArchitecture", "Version" }, data, error)) {
		try {
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					if (property == "Caption") {
						info.name = any_cast<string>(values[0]);
					}
					if (property == "OSArchitecture") {
						info.architecture = any_cast<string>(values[0]);
					}
					if (property == "Version") {
						info.version = any_cast<string>(values[0]);
					}
				}
			}
			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}
	else
		return false;
}

bool pc_info::cpu(std::vector<cpu_info>& info, std::string& error) {
	info.clear();
	map<string, vector<any>> data;
	if (d_.get_info("Win32_Processor",
		{ "Name", "Status", "Manufacturer", "NumberOfCores", "NumberOfLogicalProcessors", "MaxClockSpeed" }, data, error)) {
		try {
			std::map<size_t, cpu_info> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Name") {
							info_map[i].name = any_cast<string>(values[i]);
						}
						if (property == "Status") {
							info_map[i].status = any_cast<string>(values[i]);
						}
						if (property == "Manufacturer") {
							info_map[i].manufacturer = any_cast<string>(values[i]);
						}
						if (property == "NumberOfCores") {
							info_map[i].cores = any_cast<int>(values[i]);
						}
						if (property == "NumberOfLogicalProcessors") {
							info_map[i].logical_processors = any_cast<int>(values[i]);
						}
						if (property == "MaxClockSpeed") {
							const auto speed_mhz = any_cast<int>(values[i]);
							info_map[i].base_speed = round_off::tod(speed_mhz / 1000.0, 2);
						}
					}
				}
			}

			for (auto& it : info_map)
				info.push_back(it.second);

			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}
	else
		return false;
}

bool pc_info::gpu(std::vector<gpu_info>& info, std::string& error) {
	info.clear();
	map<string, vector<any>> data;
	if (d_.get_info("Win32_VideoController",
		{ "Name", "Status", "CurrentHorizontalResolution", "CurrentVerticalResolution", "CurrentRefreshRate", "AdapterRAM" },
		data, error)) {
		try {
			std::map<size_t, gpu_info> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Name") {
							info_map[i].name = any_cast<string>(values[i]);
						}
						if (property == "Status") {
							info_map[i].status = any_cast<string>(values[i]);
						}
						if (property == "CurrentHorizontalResolution") {
							info_map[i].horizontal_resolution = any_cast<int>(values[i]);
						}
						if (property == "CurrentVerticalResolution") {
							info_map[i].vertical_resolution = any_cast<int>(values[i]);
						}
						if (property == "CurrentRefreshRate") {
							info_map[i].refresh_rate = any_cast<int>(values[i]);
						}
						if (property == "AdapterRAM") {
							info_map[i].ram = any_cast<int>(values[i]);
						}
					}
				}
			}

			for (auto& it : info_map)
				info.push_back(it.second);

			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}

		return true;
	}
	else
		return false;
}

bool pc_info::ram(ram_info& info, std::string& error) {
	info = {};
	map<string, vector<any>> data;
	if (d_.get_info("Win32_PhysicalMemory",
		{ "Tag", "MemoryType", "FormFactor", "PartNumber", "Status", "Manufacturer", "Capacity", "Speed" }, data, error)) {
		try {
			std::map<size_t, ram_chip> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Tag") {
							info_map[i].tag = any_cast<string>(values[i]);
						}
						if (property == "MemoryType") {
							const auto type = any_cast<int>(values[i]);
							switch (type) {
							case 1:
								info_map[i].type = "Other";
								break;
							case 2:
								info_map[i].type = "DRAM";
								break;
							case 3:
								info_map[i].type = "Synchronous DRAM";
								break;
							case 4:
								info_map[i].type = "Cached DRAM";
								break;
							case 5:
								info_map[i].type = "EDO";
								break;
							case 6:
								info_map[i].type = "EDRAM";
								break;
							case 7:
								info_map[i].type = "VRAM";
								break;
							case 8:
								info_map[i].type = "SRAM";
								break;
							case 9:
								info_map[i].type = "RAM";
								break;
							case 10:
								info_map[i].type = "ROM";
								break;
							case 11:
								info_map[i].type = "Flash";
								break;
							case 12:
								info_map[i].type = "EEPROM";
								break;
							case 13:
								info_map[i].type = "FEPROM";
								break;
							case 14:
								info_map[i].type = "EPROM";
								break;
							case 15:
								info_map[i].type = "CDRAM";
								break;
							case 16:
								info_map[i].type = "3DRAM";
								break;
							case 17:
								info_map[i].type = "SDRAM";
								break;
							case 18:
								info_map[i].type = "SGRAM";
								break;
							case 19:
								info_map[i].type = "RDRAM";
								break;
							case 20:
								info_map[i].type = "DDR";
								break;
							case 21:	// May not be available
								info_map[i].type = "DDR2";
								break;
							case 22:	// May not be available
								info_map[i].type = "DDR2 FB-DIMM";
								break;
							case 24:	// May not be available
								info_map[i].type = "DDR3";
								break;
							case 25:
								info_map[i].type = "FBD2";
								break;
							case 26:
								info_map[i].type = "DDR4";
								break;
							case 0:
							default:
								info_map[i].type = "Unknown";
								break;
							}
						}
						if (property == "FormFactor") {
							const auto factor = any_cast<int>(values[i]);
							switch (factor) {
							case 1:
								info_map[i].form_factor = "Other";
								break;
							case 2:
								info_map[i].form_factor = "SIP";
								break;
							case 3:
								info_map[i].form_factor = "DIP";
								break;
							case 4:
								info_map[i].form_factor = "ZIP";
								break;
							case 5:
								info_map[i].form_factor = "SOJ";
								break;
							case 6:
								info_map[i].form_factor = "Proprietary";
								break;
							case 7:
								info_map[i].form_factor = "SIMM";
								break;
							case 8:
								info_map[i].form_factor = "DIMM";
								break;
							case 9:
								info_map[i].form_factor = "TSOP";
								break;
							case 10:
								info_map[i].form_factor = "PGA";
								break;
							case 11:
								info_map[i].form_factor = "RIMM";
								break;
							case 12:
								info_map[i].form_factor = "SODIMM";
								break;
							case 13:
								info_map[i].form_factor = "SRIMM";
								break;
							case 14:
								info_map[i].form_factor = "SMD";
								break;
							case 15:
								info_map[i].form_factor = "SSMP";
								break;
							case 16:
								info_map[i].form_factor = "QFP";
								break;
							case 17:
								info_map[i].form_factor = "TQFP";
								break;
							case 18:
								info_map[i].form_factor = "SOIC";
								break;
							case 19:
								info_map[i].form_factor = "LLC";
								break;
							case 20:
								info_map[i].form_factor = "PLCC";
								break;
							case 21:
								info_map[i].form_factor = "BGA";
								break;
							case 22:
								info_map[i].form_factor = "FPBGA";
								break;
							case 23:
								info_map[i].form_factor = "LGA";
								break;

							case 0:
							default:
								info_map[i].form_factor = "Unknown";
								break;
							}
						}
						if (property == "PartNumber") {
							info_map[i].part_number = any_cast<string>(values[i]);
						}
						if (property == "Status") {
							info_map[i].status = any_cast<string>(values[i]);
						}
						if (property == "Manufacturer") {
							info_map[i].manufacturer = any_cast<string>(values[i]);
						}
						if (property == "Capacity") {
							info_map[i].capacity = any_cast<unsigned long long>(values[i]);
						}
						if (property == "Speed") {
							info_map[i].speed = any_cast<int>(values[i]);
						}
					}
				}
			}

			for (auto& [index, ram_chip] : info_map) {
				info.size += ram_chip.capacity;
				info.speed += ram_chip.speed;
				info.ram_chips.push_back(ram_chip);
			}

			if (!info.ram_chips.empty())
				info.speed /= static_cast<int>(info.ram_chips.size());

			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}
	}
	else
		return false;
}

bool pc_info::drives(std::vector<pc_info::drive_info>& info,
	std::string& error) {
	info.clear();
	map<string, vector<any>> data;
	if (d_.get_info("Win32_DiskDrive",
		{ "Index", "Model", "SerialNumber", "InterfaceType", "MediaType", "Status", "Size" },
		data, error)) {
		try {
			std::map<size_t, drive_info> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Index") {
							info_map[i].index = any_cast<INT>(values[i]);
						}
						if (property == "Model") {
							info_map[i].model = any_cast<string>(values[i]);
						}
						if (property == "SerialNumber") {
							info_map[i].serial_number = any_cast<string>(values[i]);
						}
						if (property == "InterfaceType") {
							info_map[i].interface_type = any_cast<string>(values[i]);
						}
						if (property == "MediaType") {
							info_map[i].media_type = any_cast<string>(values[i]);
						}
						if (property == "Status") {
							info_map[i].status = any_cast<string>(values[i]);
						}
						if (property == "Size") {
							info_map[i].size = any_cast<unsigned long long>(values[i]);
						}
					}
				}
			}

			for (auto& it : info_map)
				info.push_back(it.second);

			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			return false;
		}

		return true;
	}
	else
		return false;
}

bool pc_info::power(power_info& info,
	std::string& error) {
	return get_power_info(info, error);
}

std::string pc_info::to_string(battery_status status) {
	std::string status_string;
	switch (status) {
	case battery_status::charging:
		status_string.assign("Charging");
		break;
	case battery_status::discharging:
		status_string.assign("Discharging");
		break;
	case battery_status::ac_line:
	default:
		status_string.assign("AC Line");
		break;
	}
	return status_string;
}

std::string pc_info::to_string(power_status flag) {
	std::string flag_string;
	switch (flag) {
	case liblec::leccore::pc_info::power_status::high:
		flag_string.assign("High");
		break;
	case liblec::leccore::pc_info::power_status::low:
		flag_string.assign("Low");
		break;
	case liblec::leccore::pc_info::power_status::critical:
		flag_string.assign("Critical");
		break;
	case liblec::leccore::pc_info::power_status::high_charging:
		flag_string.assign("High and charging");
		break;
	case liblec::leccore::pc_info::power_status::low_charging:
		flag_string.assign("Low and charging");
		break;
	case liblec::leccore::pc_info::power_status::critical_charging:
		flag_string.assign("Critical but charging");
		break;
	case liblec::leccore::pc_info::power_status::no_battery:
		flag_string.assign("No system battery");
		break;
	case liblec::leccore::pc_info::power_status::unknown:
	default:
		flag_string.assign("Unknown");
		break;
	}

	return flag_string;
}
