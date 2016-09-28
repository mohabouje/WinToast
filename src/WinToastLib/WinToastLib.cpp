// WinToastLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WinToastLib.h"


// This is an example of an exported variable
WINTOASTLIB_API int nWinToastLib=0;

// This is an example of an exported function.
WINTOASTLIB_API int fnWinToastLib(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see WinToastLib.h for the class definition
WinToast::WinToast() : _isCompatible(false)
{
	return;
}

void WinToast::setAppName(const string& appName) {
	_appName = appName;
}

string WinToast::appName() const {
	return _appName;
}



HRESULT WinToast::loadAppUserModelID() {
	wchar_t exePath[MAX_PATH];
	DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, exePath, ARRAYSIZE(exePath));
	HRESULT hr = written > 0 ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		written = GetFileAttributes(exePath);
		if (written >= 0xFFFFFFF)
			return E_FAIL;
		
		ComPtr<IShellItem> shellItem;
		if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellItem)))) {
			ComPtr<IPersistFile>	persistFile;
			ComPtr<IPropertyStore>	propertyStore;
			if (SUCCEEDED(shellItem.As(&persistFile)) 
				&& SUCCEEDED(persistFile->Load(exePath, STGM_READWRITE))
				&& SUCCEEDED(shellItem.As(&propertyStore))
				) 
			{
				PROPVARIANT appIdPropVar;
				if (SUCCEEDED(propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar))) {
					WCHAR AUMI[MAX_PATH];
					if (SUCCEEDED(PropVariantToString(appIdPropVar, AUMI, MAX_PATH))
						&& AUMI == _aumi) {
						PropVariantClear(&appIdPropVar);
						CoTaskMemFree(exePath);
						return S_OK;
					}
				}

			}
		}
	}
	return E_FAIL;
}