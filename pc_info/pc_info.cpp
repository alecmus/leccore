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

bool pc_info::bios_serial(std::string& serial,
	std::string& error) {
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

bool pc_info::cpu(cpu_info& info, std::string& error) {
	info = {};
	map<string, vector<any>> data;
	if (d_.get_info("Win32_Processor", { "Name", "Manufacturer", "NumberOfCores", "NumberOfLogicalProcessors" }, data, error)) {
		try {
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					if (property == "Name") {
						info.name = any_cast<string>(values[0]);
					}
					if (property == "Manufacturer") {
						info.manufacturer = any_cast<string>(values[0]);
					}
					if (property == "NumberOfCores") {
						info.cores = any_cast<int>(values[0]);
					}
					if (property == "NumberOfLogicalProcessors") {
						info.logical_processors = any_cast<int>(values[0]);
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

bool pc_info::ram(ram_info& info, std::string& error) {
	info = {};
	map<string, vector<any>> data;
	if (d_.get_info("Win32_PhysicalMemory", { "Name", "PartNumber", "Manufacturer", "Capacity" }, data, error)) {
		try {
			std::map<size_t, ram_chip> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Name") {
							info_map[i].name = any_cast<string>(values[i]);
						}
						if (property == "PartNumber") {
							info_map[i].part_number = any_cast<string>(values[i]);
						}
						if (property == "Manufacturer") {
							info_map[i].manufacturer = any_cast<string>(values[i]);
						}
						if (property == "Capacity") {
							info_map[i].capacity = any_cast<unsigned long long>(values[i]);
						}
					}
				}
			}

			for (auto& [index, ram_chip] : info_map) {
				info.size += ram_chip.capacity;
				info.ram_chips.push_back(ram_chip);
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
	case power_status::high:
		flag_string.assign("High");
		break;
	case power_status::low:
		flag_string.assign("Low");
		break;
	case power_status::critical:
		flag_string.assign("Critical");
		break;
	case power_status::charging:
		flag_string.assign("Charging");
		break;
	case power_status::no_battery:
		flag_string.assign("No system battery");
		break;
	case power_status::unknown:
	default:
		flag_string.assign("Unknown");
		break;
	}
	return flag_string;
}
