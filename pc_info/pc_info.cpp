//
// pc_info.cpp - pc information implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the MIT license. For full details see the
// file LICENSE.txt
//

#include "../pc_info.h"
#include "get_power_info.h"
#include "get_monitor_info.h"
#include "../leccore_common.h"
#include "get_wmi_data.h"
#include <map>
#include <any>
#include <dxgi.h>

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

	bool get_info(
		const string& path,
		const string& class_name,
		const vector<string>& properties,
		map<string, vector<any>>& data,
		string& error) {
		error.clear();
		data.clear();
		vector<bstr_t> _properties(properties.size());
		for (size_t i = 0; i < properties.size(); i++)
			_properties[i] = properties[i].c_str();

		return get_wmi_data(
			path.c_str(),
			class_name.c_str(),
			_properties,
			data,
			error);
	}
};

pc_info::pc_info() :
	_d(*new impl()) {}

pc_info::~pc_info() {
	delete& _d;
}

bool pc_info::pc(pc_info::pc_details& info, std::string& error) {
	error.clear();
	info = {};

	auto bios_serial = [&](std::string& serial,
		std::string& error)->bool {
			error.clear();
			serial.clear();
			map<string, vector<any>> data;
			if (_d.get_info("ROOT\\CIMV2", "Win32_Bios", { "SerialNumber" }, data, error)) {
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
			error.clear();
			serial.clear();
			map<string, vector<any>> data;
			if (_d.get_info("ROOT\\CIMV2", "Win32_BaseBoard", { "SerialNumber" }, data, error)) {
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
	if (_d.get_info("ROOT\\CIMV2", "Win32_ComputerSystem",
		{ "Name", "Manufacturer", "Model", "SystemType" }, data, error)) {
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
	error.clear();
	info = {};
	map<string, vector<any>> data;
	if (_d.get_info("ROOT\\CIMV2",
		"Win32_OperatingSystem", { "Caption", "OSArchitecture", "Version" }, data, error)) {
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
	error.clear();
	info.clear();
	map<string, vector<any>> data;
	if (_d.get_info("ROOT\\CIMV2",
		"Win32_Processor",
		{ "Name", "Status", "Manufacturer", "NumberOfCores", "NumberOfLogicalProcessors", "MaxClockSpeed" },
		data, error)) {
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

							if (info_map[i].status.empty())
								info_map[i].status = "OK";	// default to OK
						}
						if (property == "Manufacturer") {
							info_map[i].manufacturer = any_cast<string>(values[i]);
						}
						if (property == "NumberOfCores") {
							info_map[i].cores = any_cast<unsigned long>(values[i]);
						}
						if (property == "NumberOfLogicalProcessors") {
							info_map[i].logical_processors = any_cast<unsigned long>(values[i]);
						}
						if (property == "MaxClockSpeed") {
							const auto speed_mhz = any_cast<unsigned long>(values[i]);
							info_map[i].base_speed = round_off::to_double(speed_mhz / 1000.0, 2);
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
	error.clear();
	info.clear();
	map<string, vector<any>> data;
	if (_d.get_info("ROOT\\CIMV2", "Win32_VideoController",
		{ "Name", "Status", "AdapterRAM" },
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

							if (info_map[i].status.empty())
								info_map[i].status = "OK";	// default to OK
						}
						if (property == "AdapterRAM") {
							// something to fall-back on if we aren't able to get info from dxgi
							// and will be overwritten if dxgi info is available
							info_map[i].total_graphics_memory = any_cast<unsigned long>(values[i]);
						}
					}
				}
			}

			for (auto& it : info_map)
				info.push_back(it.second);

			struct dxgi_graphics_memory {
				unsigned long long dedicated = 0;
				unsigned long long total = 0;
			};

			auto get_dxgi_graphics_memory = []()->std::map<std::string, dxgi_graphics_memory> {
				std::map<std::string, dxgi_graphics_memory> result;

				HINSTANCE h_dxgi = LoadLibrary(L"dxgi.dll");
				if (!h_dxgi)
					return result;

				typedef HRESULT(WINAPI* LPCREATEDXGIFACTORY)(REFIID, void**);

				LPCREATEDXGIFACTORY p_create_dxgi_factory = nullptr;
				IDXGIFactory* p_dxgi_factory = nullptr;

				// We prefer the use of DXGI 1.1
				p_create_dxgi_factory = (LPCREATEDXGIFACTORY)GetProcAddress(h_dxgi, "CreateDXGIFactory1");

				if (!p_create_dxgi_factory) {
					p_create_dxgi_factory = (LPCREATEDXGIFACTORY)GetProcAddress(h_dxgi, "CreateDXGIFactory");

					if (!p_create_dxgi_factory) {
						FreeLibrary(h_dxgi);
						// dxgi.dll missing entry-point
						return result;
					}
				}

				HRESULT hr = p_create_dxgi_factory(__uuidof(IDXGIFactory), (LPVOID*)&p_dxgi_factory);

				if (SUCCEEDED(hr)) {
					for (UINT index = 0; ; ++index) {
						IDXGIAdapter* p_adapter = nullptr;
						if (FAILED(p_dxgi_factory->EnumAdapters(index, &p_adapter)))
							break;	// DXGIERR_NOT_FOUND is expected when the end of the list is hit

						DXGI_ADAPTER_DESC desc;
						memset(&desc, 0, sizeof(DXGI_ADAPTER_DESC));
						if (SUCCEEDED(p_adapter->GetDesc(&desc)))
							result.insert(std::make_pair(convert_string(desc.Description),
								dxgi_graphics_memory{ desc.DedicatedVideoMemory,
								desc.DedicatedVideoMemory + desc.DedicatedSystemMemory + desc.SharedSystemMemory }));

						safe_release(&p_adapter);
					}

					safe_release(&p_dxgi_factory);
				}

				FreeLibrary(h_dxgi);
				return result;
			};

			// get graphics memory using dxgi
			auto result = get_dxgi_graphics_memory();
			for (auto& it : info) {
				try {
					it.dedicated_vram = result.at(it.name).dedicated;

					// overwrite the less reliable WMI "AdapterRAM" fallback
					it.total_graphics_memory = result.at(it.name).total;
				}
				catch (const std::exception&) {}
			}

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

bool liblec::leccore::pc_info::monitor(std::vector<monitor_info>& info, std::string& error) {
	error.clear();
	info.clear();
	return get_monitor_info(info, error);
}

bool pc_info::ram(ram_info& info, std::string& error) {
	error.clear();
	info = {};
	map<string, vector<any>> data;
	if (_d.get_info("ROOT\\CIMV2", "Win32_PhysicalMemory",
		{ "Tag", "MemoryType", "FormFactor", "PartNumber", "Status", "Manufacturer", "Capacity", "Speed" },
		data, error)) {
		try {
			std::map<size_t, ram_chip> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Tag") {
							info_map[i].tag = any_cast<string>(values[i]);
						}
						if (property == "MemoryType") {
							const auto type = any_cast<unsigned long>(values[i]);
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
							const auto factor = any_cast<unsigned long>(values[i]);
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

							if (info_map[i].status.empty())
								info_map[i].status = "OK";	// default to OK
						}
						if (property == "Manufacturer") {
							info_map[i].manufacturer = any_cast<string>(values[i]);
						}
						if (property == "Capacity") {
							info_map[i].capacity = any_cast<unsigned long long>(values[i]);
						}
						if (property == "Speed") {
							info_map[i].speed = any_cast<unsigned long>(values[i]);
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
	error.clear();
	info.clear();

	struct extra_drive_info {
		unsigned int index;
		std::string friendly_name;
		std::string bus_type;
		std::string storage_type;
	};

	// key is device_id
	auto get_extra = [&](std::map<unsigned int, extra_drive_info>& extra_info, std::string& error) {
		map<string, vector<any>> data;
		if (_d.get_info("Root\\Microsoft\\Windows\\Storage", "MSFT_PhysicalDisk",
			{ "DeviceID", "FriendlyName", "MediaType", "BusType" },
			data, error)) {
			try {
				std::map<size_t, extra_drive_info> info_map;
				for (const auto& [property, values] : data) {
					if (!values.empty()) {
						for (size_t i = 0; i < values.size(); i++) {
							if (property == "DeviceID") {
								const auto index = any_cast<string>(values[i]);
								info_map[i].index = atoi(index.c_str());
							}
							if (property == "FriendlyName") {
								info_map[i].friendly_name = any_cast<string>(values[i]);
							}
							if (property == "MediaType") {
								const auto media_type = any_cast<unsigned long>(values[i]);
								switch (media_type) {
								case 3:
									info_map[i].storage_type = "HDD";
									break;
								case 4:
									info_map[i].storage_type = "SSD";
									break;
								case 5:
									info_map[i].storage_type = "SCM";
									break;
								case 0:
								default:
									info_map[i].storage_type = "Unspecified";
									break;
								}
							}
							if (property == "BusType") {
								const auto bus_type = any_cast<unsigned long>(values[i]);

								switch (bus_type) {
								case 1:
									info_map[i].bus_type = "SCSI";
									break;
								case 2:
									info_map[i].bus_type = "ATAPI";
									break;
								case 3:
									info_map[i].bus_type = "ATA";
									break;
								case 4:
									info_map[i].bus_type = "1394";
									break;
								case 5:
									info_map[i].bus_type = "SSA";
									break;
								case 6:
									info_map[i].bus_type = "Fibre Channel";
									break;
								case 7:
									info_map[i].bus_type = "USB";
									break;
								case 8:
									info_map[i].bus_type = "RAID";
									break;
								case 9:
									info_map[i].bus_type = "iSCSI";
									break;
								case 10:
									info_map[i].bus_type = "SAS";
									break;
								case 11:
									info_map[i].bus_type = "SATA";
									break;
								case 12:
									info_map[i].bus_type = "SD";
									break;
								case 13:
									info_map[i].bus_type = "MMC";
									break;
								case 14:
									info_map[i].bus_type = "MAX";
									break;
								case 15:
									info_map[i].bus_type = "File-Backed Virtual";
									break;
								case 16:
									info_map[i].bus_type = "Storage Spaces";
									break;
								case 17:
									info_map[i].bus_type = "NVMe";
									break;
								case 0:
								default:
									info_map[i].bus_type = "Unknown";
									break;
								}
							}
						}
					}
				}

				for (auto& it : info_map)
					extra_info[it.second.index] = it.second;

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
	};

	std::map<unsigned int, extra_drive_info> extra_info;
	if (!get_extra(extra_info, error))
		return false;

	map<string, vector<any>> data;
	if (_d.get_info("ROOT\\CIMV2", "Win32_DiskDrive",
		{ "Index", "DeviceID", "Model", "SerialNumber", "MediaType", "Status", "Size" },
		data, error)) {
		try {
			std::map<size_t, drive_info> info_map;
			for (const auto& [property, values] : data) {
				if (!values.empty()) {
					for (size_t i = 0; i < values.size(); i++) {
						if (property == "Index") {
							info_map[i].index = any_cast<unsigned long>(values[i]);
						}
						if (property == "DeviceID") {
							info_map[i].device_id = any_cast<string>(values[i]);
						}
						if (property == "Model") {
							info_map[i].model = any_cast<string>(values[i]);
						}
						if (property == "SerialNumber") {
							info_map[i].serial_number = any_cast<string>(values[i]);
						}
						if (property == "MediaType") {
							info_map[i].media_type = any_cast<string>(values[i]);
						}
						if (property == "Status") {
							info_map[i].status = any_cast<string>(values[i]);

							if (info_map[i].status.empty())
								info_map[i].status = "OK";	// default to OK
						}
						if (property == "Size") {
							info_map[i].size = any_cast<unsigned long long>(values[i]);
						}
					}
				}
			}

			for (auto& it : info_map)
				info.push_back(it.second);

			// add extra info
			try {
				// try using exact match
				for (auto& it : info) {
					it.storage_type = extra_info.at(it.index).storage_type;
					it.bus_type = extra_info.at(it.index).bus_type;
				}
			}
			catch (const std::exception&) {
				// exact match failed ... try using search match
				for (auto& [device_id, it] : extra_info) {
					try {
						for (auto& m_it : info) {
							if (m_it.device_id.find(std::to_string(it.index)) != m_it.device_id.npos) {
								m_it.storage_type = it.storage_type;
								m_it.bus_type = it.bus_type;
								break;
							}
						}
					}
					catch (const std::exception&) {
						// to-do: log
					}
				}
			}

			// replace "search" with "replace" in the string "subject"
			auto replace_string = [](std::string& s,
				const std::string& search,
				const std::string& replace) {
					size_t pos = 0;
					while ((pos = s.find(search, pos)) != std::string::npos) {
						s.replace(pos, search.length(), replace);
						pos += replace.length();
					}
					return;
			};

			// replace hard disk media when SSD storage is detected
			for (auto& it : info) {
				if (it.storage_type == "SSD") {
					// erase "hard disk " from the text
					replace_string(it.media_type, "hard disk ", "solid state ");
				}
				if (it.storage_type == "SCM") {
					// erase "hard disk " from the text
					replace_string(it.media_type, "hard disk ", "storage class memory ");
				}
			}

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
	case liblec::leccore::pc_info::power_status::medium:
		flag_string.assign("Medium");
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
	case liblec::leccore::pc_info::power_status::charging:
		flag_string.assign("Charging");
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
