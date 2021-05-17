//
// get_wmi_data.cpp - get wmi (windows management instrumentation) data implementation
//
// leccore library, part of the liblec library
// Copyright (c) 2019 Alec Musasa (alecmus at live dot com)
//
// Released under the CC-BY-NC 2.0 license. For full details see the file
// LICENSE.txt or go to https://github.com/alecmus/liblec/blob/master/LICENSE.md
//

#include "get_wmi_data.h"
#include "../leccore_common.h"
#include <sstream>
#include <WbemIdl.h>
#pragma comment(lib, "wbemuuid.lib")

bool liblec::leccore::get_wmi_data(
	bstr_t path,
	bstr_t class_name,
	std::vector<bstr_t> properties,
	std::map<std::string, std::vector<std::any>>& wmi_data,	// key is the property name
	std::string& error) {
	wmi_data.clear();

	// 1. Obtain the initial locator to WMI
	IWbemLocator* p_loc = nullptr;

	HRESULT h_res = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&p_loc);

	if (FAILED(h_res)) {
		error = convert_string(_com_error(h_res).ErrorMessage());
		return false;
	}

	// 2. Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices* p_svc = nullptr;

	h_res = p_loc->ConnectServer(
		path,   // Object path of WMI namespace
		NULL,   // User name. NULL = current user
		NULL,   // User password. NULL = current
		0,      // Locale. NULL indicates current
		NULL,   // Security flags.
		0,      // Authority (for example, Kerberos)
		0,      // Context object 
		&p_svc  // pointer to IWbemServices proxy
	);

	if (FAILED(h_res)) {
		p_loc->Release();

		error = convert_string(_com_error(h_res).ErrorMessage());
		return false;
	}

	// 3. Set security levels on the proxy
	h_res = CoSetProxyBlanket(
		p_svc,                       // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(h_res)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(h_res).ErrorMessage());
		return false;
	}

	// 4. Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* p_enumerator = nullptr;
	h_res = p_svc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM ") + class_name,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&p_enumerator);

	if (FAILED(h_res)) {
		p_svc->Release();
		p_loc->Release();

		error = convert_string(_com_error(h_res).ErrorMessage());
		return false;
	}

	// 5. Get the data from the query in step 4
	IWbemClassObject* p_class_object;
	ULONG u_return = 0;

	while (p_enumerator) {
		HRESULT hr = p_enumerator->Next(WBEM_INFINITE, 1,
			&p_class_object, &u_return);

		if (0 == u_return)
			break;

		for (const auto& property : properties) {
			VARIANT vtProp;

			// Get the value of the Name property
			CIMTYPE type;
			hr = p_class_object->Get(property, 0, &vtProp, &type, 0);

			if (FAILED(hr)) {
				VariantClear(&vtProp);

				p_svc->Release();
				p_loc->Release();

				wmi_data.clear();
				error = convert_string(_com_error(h_res).ErrorMessage());
				return false;
			}

			switch (type) {
			case CIM_STRING:
				wmi_data[std::string(property)].push_back(vtProp.bstrVal ? convert_string(vtProp.bstrVal) : "");
				break;

			case CIM_UINT16:
				wmi_data[std::string(property)].push_back(vtProp.intVal);
				break;

			case CIM_UINT32:
				wmi_data[std::string(property)].push_back(vtProp.intVal);
				break;

			case CIM_UINT64: {
				std::string val = vtProp.bstrVal ? convert_string(vtProp.bstrVal) : "";
				std::stringstream ss;
				ss << val;
				unsigned long long l;
				ss >> l;
				wmi_data[std::string(property)].push_back(l);
			} break;

			default:
				break;
			}

			VariantClear(&vtProp);
		}

		p_class_object->Release();
	}

	// cleanup
	p_svc->Release();
	p_loc->Release();
	p_enumerator->Release();

	for (auto& [property, values] : wmi_data) {
		if (property == "SerialNumber") {
			for (auto& it : values) {
				auto data_str = std::any_cast<std::string>(it);

				for (int i = 0; i < (int)data_str.length(); i++)
					data_str[i] = toupper(data_str[i]);

				if (data_str.find("O.E.M.") != data_str.npos)
					it = std::string();	// TO BE FILLED BY O.E.M.

				// check if BIOS serial is less than three characters in length
				const char* buf = class_name;
				std::string class_name_ = std::string(buf ? buf : "", class_name.length());
				if (class_name_ == "Win32_Bios" && data_str.length() < 3)
					it = std::string();
			}
		}
	}

	return true;
}
