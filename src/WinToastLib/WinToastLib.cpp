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

void WinToast::setAppName(const wstring& appName) {
	_appName = appName;
}

wstring WinToast::appName() const {
	return _appName;
}


HRESULT WinToast::createShellLinkInPath(_In_ PCWSTR path) const {
	WCHAR exePath[MAX_PATH];
	HRESULT hr = defaultExecutablePath(exePath);
	if (SUCCEEDED(hr)) {
		ComPtr<IShellLink> shellLink;
		hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
		if (SUCCEEDED(hr)) {
			hr = shellLink->SetPath(exePath);
			if (SUCCEEDED(hr)) {
				hr = shellLink->SetArguments(L"");
				if (SUCCEEDED(hr)) {
					ComPtr<IPersistFile> persistFile;
					hr = shellLink.As(&persistFile);
					if (SUCCEEDED(hr)) {
						hr = persistFile->Save(path, TRUE);
					}
				}
			}
		}
	}
	return hr;
}

HRESULT WinToast::defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize) const {
	DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, path, nSize);
	return (written > 0) ? S_OK : E_FAIL;
}


HRESULT WinToast::defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize) const {
	DWORD written = GetEnvironmentVariable(L"APPDATA", path, nSize);
	HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
	if (SUCCEEDED(hr)) {
		errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
		hr = (result == 0) ? S_OK : E_INVALIDARG;
	}
	return hr;
}

HRESULT WinToast::defaultShellLinkPath(_In_ WCHAR* path, _In_ DWORD nSize) const {
	HRESULT hr = defaultShellLinksDirectory(path, nSize);
	if (SUCCEEDED(hr)) {
		const wstring appLink(_appName + DEFAULT_LINK_FORMAT);
		errno_t result = wcscat_s(path, nSize, appLink.c_str());
		hr = (result == 0) ? S_OK : E_INVALIDARG;
	}
	return hr;
}

HRESULT WinToast::loadAppUserModelId() {
	wchar_t slPath[MAX_PATH];
	DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, slPath, ARRAYSIZE(slPath));
	HRESULT hr = written > 0 ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		written = GetFileAttributes(slPath);
		if (written >= 0xFFFFFFF)
			return E_FAIL;
		
		ComPtr<IShellItem> shellItem;
		if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellItem)))) {
			ComPtr<IPersistFile>	persistFile;
			ComPtr<IPropertyStore>	propertyStore;
			if (SUCCEEDED(shellItem.As(&persistFile)) 
				&& SUCCEEDED(persistFile->Load(slPath, STGM_READWRITE))
				&& SUCCEEDED(shellItem.As(&propertyStore))
				) 
			{
				PROPVARIANT appIdPropVar;
				if (SUCCEEDED(propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar))) {
					WCHAR AUMI[MAX_PATH];
					if (SUCCEEDED(PropVariantToString(appIdPropVar, AUMI, MAX_PATH))
						&& AUMI == _aumi) {
						PropVariantClear(&appIdPropVar);
						CoTaskMemFree(slPath);
						return S_OK;
					}
				}

			}
		}
	}
	return E_FAIL;
}