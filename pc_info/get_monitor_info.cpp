//
// get_monitor_info.cpp - get monitor information implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "get_monitor_info.h"
#include "../leccore_common.h"
#include <comdef.h>
#include <WbemIdl.h>
#pragma comment(lib, "wbemuuid.lib")

bool get_supported_video_modes(std::vector<pc_info::monitor_info>& info,
	std::string& error) {
	// 1. Obtain the initial locator to WMI
	IWbemLocator* p_loc = nullptr;

	HRESULT result = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&p_loc);

	if (FAILED(result)) {
		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 2. Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices* p_svc = nullptr;

	result = p_loc->ConnectServer(
		bstr_t("ROOT\\WMI"),   // Object path of WMI namespace
		NULL,   // User name. NULL = current user
		NULL,   // User password. NULL = current
		0,      // Locale. NULL indicates current
		NULL,   // Security flags.
		0,      // Authority (for example, Kerberos)
		0,      // Context object 
		&p_svc  // pointer to IWbemServices proxy
	);

	if (FAILED(result)) {
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 3. Set security levels on the proxy
	result = CoSetProxyBlanket(
		p_svc,                       // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(result)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 4. Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* p_enumerator = nullptr;
	result = p_svc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM WmiMonitorListedSupportedSourceModes"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&p_enumerator);

	if (FAILED(result)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 5. Get the data from the query in step 4
	IWbemClassObject* p_class_object;
	ULONG u_return = 0;

	while (p_enumerator) {
		HRESULT result = p_enumerator->Next(WBEM_INFINITE, 1,
			&p_class_object, &u_return);

		if (0 == u_return)
			break;

		pc_info::monitor_info this_monitor_info;

		// get InstanceName
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"InstanceName", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type == CIM_STRING) {
				this_monitor_info.instance_name = vtProp.bstrVal ? convert_string(vtProp.bstrVal) : "";
				trim(this_monitor_info.instance_name);
			}

			VariantClear(&vtProp);
		}

		// get MonitorSourceModes
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"MonitorSourceModes", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type & CIM_OBJECT) {
				// verify if it's an array
				if (V_ISARRAY(&vtProp)) {
					// get safe array
					LPSAFEARRAY pSafeArray = V_ARRAY(&vtProp);

					// determine the type of item in the array
					VARTYPE itemType;
					if (SUCCEEDED(SafeArrayGetVartype(pSafeArray, &itemType))) {
						if (itemType == VT_UNKNOWN) {
							SAFEARRAY* psa = V_ARRAY(&vtProp);

							LONG lBound, uBound;
							SafeArrayGetLBound(psa, 1, &lBound);
							SafeArrayGetUBound(psa, 1, &uBound);
							long numElems = uBound - lBound + 1;

							IUnknown** rawArray;
							SafeArrayAccessData(psa, (void**)&rawArray);

							for (LONG i = 0; i < numElems; i++) {
								IUnknown* pElem = rawArray[i];

								IWbemClassObject* p_obj = NULL;
								pElem->QueryInterface(IID_IWbemClassObject, (void**)&p_obj);

								if (p_obj != NULL) {
									ULONG horizontal_res = 0, vertical_res = 0, pixel_clock_rate = 0;
									ULONG width_mm = 0, height_mm = 0;
									double v_refresh_rate = 0.0;

									// get the value of the horizontal resolution
									{
										VARIANT vtProp;
										CIMTYPE type;
										result = p_obj->Get(L"HorizontalActivePixels", 0, &vtProp, &type, 0);

										if (FAILED(result)) {
											VariantClear(&vtProp);
											continue;
										}

										if (type == CIM_UINT16)
											horizontal_res = vtProp.ulVal;
									}

									// get the value of the vertical resolution
									{
										VARIANT vtProp;
										CIMTYPE type;
										result = p_obj->Get(L"VerticalActivePixels", 0, &vtProp, &type, 0);

										if (FAILED(result)) {
											VariantClear(&vtProp);
											continue;
										}

										if (type == CIM_UINT16)
											vertical_res = vtProp.ulVal;
									}

									// get the pixel clock rate
									{
										VARIANT vtProp;
										CIMTYPE type;
										result = p_obj->Get(L"PixelClockRate", 0, &vtProp, &type, 0);

										if (FAILED(result)) {
											VariantClear(&vtProp);
											continue;
										}

										if (type == CIM_UINT32)
											pixel_clock_rate = vtProp.ulVal;
									}

									// get the value of the horizontal image size
									{
										VARIANT vtProp;
										CIMTYPE type;
										result = p_obj->Get(L"HorizontalImageSize", 0, &vtProp, &type, 0);

										if (FAILED(result)) {
											VariantClear(&vtProp);
											continue;
										}

										if (type == CIM_UINT16)
											width_mm = vtProp.ulVal;
									}

									// get the value of the vertical image size
									{
										VARIANT vtProp;
										CIMTYPE type;
										result = p_obj->Get(L"VerticalImageSize", 0, &vtProp, &type, 0);

										if (FAILED(result)) {
											VariantClear(&vtProp);
											continue;
										}

										if (type == CIM_UINT16)
											height_mm = vtProp.ulVal;
									}

									// get the value of the vertical refresh rate
									{
										ULONG v_refresh_demoninator = 0, v_refresh_numerator = 0;

										{
											VARIANT vtProp;
											CIMTYPE type;
											result = p_obj->Get(L"VerticalRefreshRateDenominator", 0, &vtProp, &type, 0);

											if (FAILED(result)) {
												VariantClear(&vtProp);
												continue;
											}

											if (type == CIM_UINT16 || type == CIM_UINT32)
												v_refresh_demoninator = vtProp.ulVal;
										}

										{
											VARIANT vtProp;
											CIMTYPE type;
											result = p_obj->Get(L"VerticalRefreshRateNumerator", 0, &vtProp, &type, 0);

											if (FAILED(result)) {
												VariantClear(&vtProp);
												continue;
											}

											if (type == CIM_UINT16 || type == CIM_UINT32)
												v_refresh_numerator = vtProp.ulVal;
										}

										if (v_refresh_demoninator > 0)
											v_refresh_rate = (double)v_refresh_numerator / (double)v_refresh_demoninator;
									}

									auto resolution_name = [](const int horizontal_res, const int vertical_res)->std::string {
										if (horizontal_res >= 7680 && vertical_res >= 4320)
											return "8K Ultra HD";
										if (horizontal_res >= 3840 && vertical_res >= 2160)
											return "4K Ultra HD";
										if (horizontal_res >= 3456 && vertical_res >= 2160)
											return "3.5K";
										if (horizontal_res >= 2560 && vertical_res >= 1440)
											return "Quad HD";
										if (horizontal_res >= 2048 && vertical_res >= 1080)
											return "2K";
										if (horizontal_res >= 1920 && vertical_res >= 1080)
											return "Full HD";
										if (horizontal_res >= 1600 && vertical_res >= 900)
											return "HD+";
										if (horizontal_res >= 1280 && vertical_res >= 720)
											return "HD";

										return "";
									};

									auto screen_size = [](const int width_mm, const int height_mm)->double {
										double diagonal_mm = sqrt(double(width_mm) * double(width_mm) + double(height_mm) * double(height_mm));
										return diagonal_mm / 25.4;
									};

									pc_info::video_mode mode;
									mode.horizontal_resolution = horizontal_res;
									mode.vertical_resolution = vertical_res;
									mode.resolution_name = resolution_name(mode.horizontal_resolution, mode.vertical_resolution);
									mode.pixel_clock_rate = pixel_clock_rate;
									mode.refresh_rate = v_refresh_rate;
									mode.physical_size = screen_size(width_mm, height_mm);

									// capture supported mode
									this_monitor_info.supported_modes.push_back(mode);
								}
							}

							SafeArrayUnaccessData(psa);
						}
					}
				}
			}

			VariantClear(&vtProp);
		}

		p_class_object->Release();

		// add to monitor info list
		info.push_back(this_monitor_info);
	}

	// cleanup
	p_svc->Release();
	p_loc->Release();
	p_enumerator->Release();

	return true;
}

bool get_other_monitor_info(std::vector<pc_info::monitor_info>& info,
	std::string& error) {
	// 1. Obtain the initial locator to WMI
	IWbemLocator* p_loc = nullptr;

	HRESULT result = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&p_loc);

	if (FAILED(result)) {
		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 2. Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices* p_svc = nullptr;

	result = p_loc->ConnectServer(
		bstr_t("ROOT\\WMI"),   // Object path of WMI namespace
		NULL,   // User name. NULL = current user
		NULL,   // User password. NULL = current
		0,      // Locale. NULL indicates current
		NULL,   // Security flags.
		0,      // Authority (for example, Kerberos)
		0,      // Context object 
		&p_svc  // pointer to IWbemServices proxy
	);

	if (FAILED(result)) {
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 3. Set security levels on the proxy
	result = CoSetProxyBlanket(
		p_svc,                       // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(result)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 4. Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* p_enumerator = nullptr;
	result = p_svc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM WmiMonitorID"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&p_enumerator);

	if (FAILED(result)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(result).ErrorMessage());
		return false;
	}

	// 5. Get the data from the query in step 4
	IWbemClassObject* p_class_object;
	ULONG u_return = 0;

	while (p_enumerator) {
		HRESULT result = p_enumerator->Next(WBEM_INFINITE, 1,
			&p_class_object, &u_return);

		if (0 == u_return)
			break;

		pc_info::monitor_info this_monitor_info;

		// get InstanceName
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"InstanceName", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type == CIM_STRING) {
				this_monitor_info.instance_name = vtProp.bstrVal ? convert_string(vtProp.bstrVal) : "";
				trim(this_monitor_info.instance_name);
			}

			VariantClear(&vtProp);
		}

		// get manufacturer name
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"ManufacturerName", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type & CIM_FLAG_ARRAY && type & CIM_UINT16) {
				// verify if it's an array
				if (V_ISARRAY(&vtProp)) {
					// get safe array
					LPSAFEARRAY pSafeArray = V_ARRAY(&vtProp);

					// determine the type of item in the array
					VARTYPE itemType;
					if (SUCCEEDED(SafeArrayGetVartype(pSafeArray, &itemType))) {

						// verify it's the type we expect
						if (V_VT(&vtProp) == (VT_ARRAY | VT_I4)) {
							SAFEARRAY* psa = V_ARRAY(&vtProp);

							LONG lBound, uBound;
							SafeArrayGetLBound(psa, 1, &lBound);
							SafeArrayGetUBound(psa, 1, &uBound);
							long numElems = uBound - lBound + 1;

							INT* rawArray = nullptr;
							SafeArrayAccessData(psa, (void**)&rawArray);

							for (LONG i = 0; i < numElems; i++) {
								INT pElem = rawArray[i];

								if (pElem) {
									char c = pElem;
									std::string s;
									s = c;
									this_monitor_info.manufacturer += s;
								}
							}

							SafeArrayUnaccessData(psa);
						}
					}
				}
			}

			VariantClear(&vtProp);
		}

		// get product code ID
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"ProductCodeID", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type & CIM_FLAG_ARRAY && type & CIM_UINT16) {
				// verify if it's an array
				if (V_ISARRAY(&vtProp)) {
					// get safe array
					LPSAFEARRAY pSafeArray = V_ARRAY(&vtProp);

					// determine the type of item in the array
					VARTYPE itemType;
					if (SUCCEEDED(SafeArrayGetVartype(pSafeArray, &itemType))) {

						// verify it's the type we expect
						if (V_VT(&vtProp) == (VT_ARRAY | VT_I4)) {
							SAFEARRAY* psa = V_ARRAY(&vtProp);

							LONG lBound, uBound;
							SafeArrayGetLBound(psa, 1, &lBound);
							SafeArrayGetUBound(psa, 1, &uBound);
							long numElems = uBound - lBound + 1;

							INT* rawArray = nullptr;
							SafeArrayAccessData(psa, (void**)&rawArray);

							for (LONG i = 0; i < numElems; i++) {
								INT pElem = rawArray[i];

								if (pElem) {
									char c = pElem;
									std::string s;
									s = c;
									this_monitor_info.product_code_id += s;
								}
							}

							SafeArrayUnaccessData(psa);
						}
					}
				}
			}

			VariantClear(&vtProp);
		}

		// get manufacture year
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"YearOfManufacture", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type == CIM_UINT16)
				this_monitor_info.year_of_manufacture = static_cast<SHORT>(vtProp.ulVal);

			VariantClear(&vtProp);
		}

		// get manufacture week
		{
			VARIANT vtProp;

			// Get the value of the property
			CIMTYPE type;
			result = p_class_object->Get(L"WeekOfManufacture", 0, &vtProp, &type, 0);

			if (FAILED(result)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				error = convert_string(_com_error(result).ErrorMessage());
				return false;
			}

			if (type == CIM_UINT8)
				this_monitor_info.week_of_manufacture = static_cast<SHORT>(vtProp.ulVal);

			VariantClear(&vtProp);
		}

		p_class_object->Release();

		// add to monitor info list
		info.push_back(this_monitor_info);
	}

	// cleanup
	p_svc->Release();
	p_loc->Release();
	p_enumerator->Release();

	return true;
}

bool get_monitor_info(std::vector<pc_info::monitor_info>& info,
	std::string& error) {
	// get supported video modes
	std::vector<pc_info::monitor_info> video_modes_info;
	if (!get_supported_video_modes(video_modes_info, error))
		return false;

	// get other info
	std::vector<pc_info::monitor_info> other_info;
	if (!get_other_monitor_info(other_info, error))
		return false;

	// consolidate
	info = other_info;

	for (auto& it : info) {
		for (auto& m_it : video_modes_info) {
			if (m_it.instance_name == it.instance_name) {
				it.supported_modes = m_it.supported_modes;
				break;
			}
		}
	}

	return true;
}
