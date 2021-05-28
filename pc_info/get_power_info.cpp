//
// get_power_info.cpp - get power information implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#define INITGUID

#include "get_power_info.h"
#include "../leccore_common.h"
#include "../error/win_error.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include <regex>
#include <batclass.h>

#include <SetupAPI.h>
#pragma comment(lib, "SetupAPI.lib")

#define GUID_DEVCLASS_BATTERY GUID_DEVICE_BATTERY
constexpr int MAX_NUMBER_OF_BATTERIES = 100;

static void trim(std::string& s) {
	s = s.substr(s.find_first_not_of(' '), (s.find_last_not_of(' ') - s.find_first_not_of(' ')) + 1);
}

void timeConv(int iTotalSeconds,
	int& iHours, int& iMinutes, int& iSeconds) {
	iHours = 0, iMinutes = 0, iSeconds = 0;

	int iTotal = iTotalSeconds;
	iMinutes = iTotal / 60;
	iSeconds = iTotal % 60;
	iHours = iMinutes / 60;
	iMinutes = iMinutes % 60;

	return;
}

std::string formatTime(int iHours, int iMinutes) {
	std::string sFormatted;

	if (iHours > 0) {
		sFormatted = std::to_string(iHours);

		sFormatted += " hour";

		if (iHours != 1)
			sFormatted += "s";
	}

	if (iMinutes > 0 || iHours == 0) {
		if (!sFormatted.empty())
			sFormatted += " ";

		if (iMinutes < 10)
			sFormatted += "0";

		sFormatted += std::to_string(iMinutes);

		sFormatted += " minute";

		if (iMinutes != 1)
			sFormatted += "s";
	}

	return sFormatted;
}

namespace liblec {
	namespace leccore {
		/// <summary>
		/// Code adapted from https://docs.microsoft.com/en-gb/windows/desktop/Power/enumerating-battery-devices
		/// </summary>
		bool GetBatteryState(pc_info::power_info& info, std::string& error) {
			error.clear();
			SYSTEM_POWER_STATUS ps = { 0 };

			if (!GetSystemPowerStatus(&ps)) {
				error = get_last_error();
				return false;
			}

			// 1A. get ac status
			info.ac = ps.ACLineStatus == 1;
			info.status = pc_info::power_status::unknown;

			// 1B. get power status
			switch (ps.BatteryFlag) {
			case 0:
				info.status = pc_info::power_status::medium;
				break;
			case 1:
				info.status = pc_info::power_status::high;
				break;
			case 2:
				info.status = pc_info::power_status::low;
				break;
			case 4:
				info.status = pc_info::power_status::critical;
				break;
			case 8:
				info.status = pc_info::power_status::charging;
				break;
			case 128:
				info.status = pc_info::power_status::no_battery;
				break;
			case 255:
				info.status = pc_info::power_status::unknown;
				break;
			default: {
				if (ps.BatteryFlag & 8) {
					// charging
					if (ps.BatteryFlag & 1)
						info.status = pc_info::power_status::high_charging;
					else
						if (ps.BatteryFlag & 2)
							info.status = pc_info::power_status::low_charging;
						else
							if (ps.BatteryFlag & 4)
								info.status = pc_info::power_status::critical_charging;
				}
			} break;
			}

			// 1C. Get battery level
			if (ps.BatteryLifePercent) {
				if (ps.BatteryLifePercent == 255)
					info.level = -1;
				else {
					info.level = ps.BatteryLifePercent;

					info.level = largest(info.level, 0);
					info.level = smallest(info.level, 100);
				}
			}
			else
				info.level = -1;

			// 1D. Get battery lifetime
			if (ps.ACLineStatus != 1 && ps.BatteryLifeTime != 0xFFFFFFFF && ps.BatteryLifeTime != -1) {
				int iHours = 0, iMinutes = 0, iSeconds = 0;
				timeConv(ps.BatteryLifeTime, iHours, iMinutes, iSeconds);
				info.lifetime_remaining = formatTime(iHours, iMinutes);
			}

			HDEVINFO hdev = SetupDiGetClassDevs(
				&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

			if (hdev == INVALID_HANDLE_VALUE) {
				error = get_last_error();
				return false;
			}

			// Limit search to 100 batteries max
			for (int idev = 0; idev < MAX_NUMBER_OF_BATTERIES; idev++) {
				SP_DEVICE_INTERFACE_DATA did = { 0 };
				did.cbSize = sizeof(did);

				if (SetupDiEnumDeviceInterfaces(hdev, 0, &GUID_DEVCLASS_BATTERY, idev, &did)) {
					DWORD cbRequired = 0;
					SetupDiGetDeviceInterfaceDetail(hdev, &did, 0, 0, &cbRequired, 0);

					if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
						PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd =
							(PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);

						if (pdidd) {
							pdidd->cbSize = sizeof(*pdidd);

							if (SetupDiGetDeviceInterfaceDetail(hdev, &did, pdidd, cbRequired, &cbRequired, 0)) {
								// We've eumerated a battery. Let's ask it for information.
								HANDLE hBattery = CreateFile(pdidd->DevicePath, GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL, NULL);

								if (INVALID_HANDLE_VALUE != hBattery) {
									// Let's ask the battery for its tag.
									BATTERY_QUERY_INFORMATION bqi = { 0 };
									DWORD dwWait = 0, dwOut;

									if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG,
										&dwWait, sizeof(dwWait), &bqi.BatteryTag,
										sizeof(bqi.BatteryTag), &dwOut, NULL)
										&& bqi.BatteryTag) {
										// With the tag, we can now query the battery info.
										BATTERY_INFORMATION bi = { 0 };
										bqi.InformationLevel = BatteryInformation;

										if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION,
											&bqi, sizeof(bqi), &bi, sizeof(bi), &dwOut, NULL)) {

											// Only non-UPS system batteries count
											if (bi.Capabilities & BATTERY_SYSTEM_BATTERY) {
												// Finally let's get the battery info
												pc_info::battery_info battery;

												// capture the designed and fully charged capacities
												battery.designed_capacity = bi.DesignedCapacity;
												battery.fully_charged_capacity = bi.FullChargedCapacity;

												// Let's query the battery status.
												BATTERY_WAIT_STATUS bws = { 0 };
												bws.BatteryTag = bqi.BatteryTag;

												BATTERY_STATUS bs;
												if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_STATUS,
													&bws, sizeof(bws), &bs, sizeof(bs), &dwOut, NULL)) {
													// capture current capacity
													if (bs.Capacity != BATTERY_UNKNOWN_CAPACITY)
														battery.current_capacity = bs.Capacity;
													else
														battery.current_capacity = -1;

													// capture current voltage
													if (bs.Voltage != BATTERY_UNKNOWN_VOLTAGE)
														battery.current_voltage = bs.Voltage;
													else
														battery.current_voltage = -1;

													// capture current charge rate
													if (bs.Rate != BATTERY_UNKNOWN_RATE)
														battery.current_charge_rate = bs.Rate;
													else
														battery.current_charge_rate = 0;

													// capture battery status
													if (bs.PowerState & BATTERY_CHARGING)
														battery.status = pc_info::battery_status::charging;
													else
														if (bs.PowerState & BATTERY_DISCHARGING)
															battery.status = pc_info::battery_status::discharging;
														else
															if (bs.PowerState & BATTERY_POWER_ON_LINE)
																battery.status = pc_info::battery_status::ac_line;
															else
																battery.status = pc_info::battery_status::ac_line;	// default to online
												}

												// Query serial number
												{
													TCHAR c[256];	// TO-DO: there must be a better way
													bqi.InformationLevel = BatterySerialNumber;

													if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi,
														sizeof(bqi), &c, sizeof(c), &dwOut, NULL)) {
														battery.serial_number = convert_string(c);

														// trim spaces
														trim(battery.serial_number);
													}
												}

												// Query manufacture name
												{
													TCHAR c[256];	// TO-DO: there must be a better way
													bqi.InformationLevel = BatteryManufactureName;

													if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi,
														sizeof(bqi), &c, sizeof(c), &dwOut, NULL)) {
														battery.manufacturer = convert_string(c);

														// trim spaces
														trim(battery.manufacturer);
													}

												}

												// Query battery name
												{
													TCHAR c[256];	// TO-DO: there must be a better way
													bqi.InformationLevel = BatteryDeviceName;

													if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi,
														sizeof(bqi), &c, sizeof(c), &dwOut, NULL)) {
														battery.name = convert_string(c);

														// trim spaces
														trim(battery.name);
													}
												}

												// Query unique ID
												{
													TCHAR c[256];	// TO-DO: there must be a better way
													bqi.InformationLevel = BatteryUniqueID;

													if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi,
														sizeof(bqi), &c, sizeof(c), &dwOut, NULL)) {
														battery.unique_id = convert_string(c);

														// trim spaces
														trim(battery.unique_id);
													}
												}

												// Query manufacture date
												{
													BATTERY_MANUFACTURE_DATE bmd = { 0 };
													bqi.InformationLevel = BatteryManufactureDate;

													if (DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION, &bqi,
														sizeof(bqi), &bmd, sizeof(bmd), &dwOut, NULL)) {

													}
												}

												info.batteries.push_back(battery);
											}
										}
									}

									CloseHandle(hBattery);
								}
							}

							LocalFree(pdidd);
						}
					}
				}
				else if (ERROR_NO_MORE_ITEMS == GetLastError()) {
					break;  // Enumeration failed - perhaps we're out of items
				}
			}

			SetupDiDestroyDeviceInfoList(hdev);

			for (auto& it : info.batteries) {
				if (it.designed_capacity != 0 && it.designed_capacity != -1 && it.fully_charged_capacity != -1) {
					it.health = 100.0 * it.fully_charged_capacity / it.designed_capacity;

					it.health = largest(it.health, 0.0);
					it.health = smallest(it.health, 100.0);
				}

				if (it.fully_charged_capacity != 0 && it.fully_charged_capacity != -1 && it.current_capacity != -1) {
					it.level = 100.0 * it.current_capacity / it.fully_charged_capacity;

					it.level = largest(it.level, 0.0);
					it.level = smallest(it.level, 100.0);
				}
			}

			return true;
		}

		bool get_power_info(pc_info::power_info& info,
			std::string& error) {
			error.clear();
			info.ac = false;
			info.status = pc_info::power_status::unknown;
			info.level = -1;
			info.lifetime_remaining.clear();
			info.batteries.clear();

			return GetBatteryState(info, error);
		}
	}
}
