// WinToastLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WinToastLib.h"
#include "helper.h"

std::wstring WinToast::ToastTag = L"toast";
std::wstring WinToast::ImageTag = L"image";
std::wstring WinToast::TextTag = L"text";
std::wstring WinToast::SrcTag = L"src";


// This is an example of an exported variable
WINTOASTLIB_API int nWinToastLib=0;

// This is an example of an exported function.
WINTOASTLIB_API int fnWinToastLib(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see WinToastLib.h for the class definition
WinToast::WinToast() : _isCompatible(false), _template(WinToastTemplate::UnknownTemplate)
{
	setTemplate(WinToastTemplate::ImageWithOneLine);
	return;
}

bool WinToast::initialize() {
	if (_aumi.empty()) {
		wcout << L"Error: App User Model Id is empty!";
		return false;
	}
	HRESULT hr = loadAppUserModelId();
	if (FAILED(hr)) {
		WCHAR shellPath[MAX_PATH];
		hr = defaultShellLinkPath(shellPath);
		if (SUCCEEDED(hr)) {
			hr = createShellLinkInPath(shellPath);
			if (SUCCEEDED(hr)) {
				hr = initAppUserModelId();
			}
		}
	}
	return hr == S_OK;
}

void WinToast::setAppName(_In_ const wstring& appName) {
	_appName = appName;
}

wstring WinToast::appName() const {
	return _appName;
}

wstring WinToast::appUserModelId() const {
	return _aumi;
}
void WinToast::setAppUserModelId(_In_ const wstring& aumi) {
	_aumi = aumi;
}


HRESULT	WinToast::initAppUserModelId() {
	WCHAR	slPath[MAX_PATH];
	HRESULT hr = defaultShellLinkPath(slPath);
	if (SUCCEEDED(hr)) {
		ComPtr<IPropertyStore> propertyStore;
		hr = SHGetPropertyStoreFromParsingName(slPath, nullptr, GPS_READWRITE, IID_PPV_ARGS(&propertyStore));
		if (SUCCEEDED(hr)) {
			PROPVARIANT appIdPropVar;
			hr = InitPropVariantFromString(_aumi.c_str(), &appIdPropVar);
			if (SUCCEEDED(hr)) {
				hr = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
				if (SUCCEEDED(hr)) {
					hr = propertyStore->Commit();
				}
				PropVariantClear(&appIdPropVar);
			}
		}
	}
	return hr;
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
						CoTaskMemFree(exePath);
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
					if (SUCCEEDED(propVariantToString(appIdPropVar, AUMI, MAX_PATH))
						&& AUMI == _aumi) {
						PropVariantClear(&appIdPropVar);
						CoTaskMemFree(slPath);
						hr = S_OK;
					}
				}

			}
		}
	}
	return hr;
}

void WinToast::setTemplate(_In_ const WinToastTemplate& templ) {
	if (_template != templ) {
		_template = templ;
		notificationManager()->GetTemplateContent(ToastTemplateType(templ), &_xmlDocument);
	}
}



HRESULT WinToast::setTextField(_In_ const wstring& text, int pos) {
	ComPtr<IXmlNodeList> nodeList;
	HRESULT hr = getNodeListByTag(TextTag, nodeList, xmlDocument());
	if (SUCCEEDED(hr)) {
		ComPtr<IXmlNode> node;
		hr = getNodeFromNodeList(nodeList, node, pos);
		if (SUCCEEDED(hr)) {
			hr = setNodeStringValue(loadStringReference(text), node.Get(), xmlDocument());
		}
	}
	return hr;
}


HRESULT WinToast::setImageField(_In_ const wstring& path)  {
	wchar_t imagePath[MAX_PATH];
	HRESULT hr = StringCchCat(imagePath, MAX_PATH, path.c_str());
	if (SUCCEEDED(hr)) {
		ComPtr<IXmlNodeList> nodeList;
		HRESULT hr = getNodeListByTag(ImageTag, nodeList, xmlDocument());
		if (SUCCEEDED(hr)) {
			ComPtr<IXmlNode> node;
			hr = getNodeFromNodeList(nodeList, node, 0);
			if (SUCCEEDED(hr))  {
				ComPtr<IXmlNamedNodeMap> attributes;
				hr = node->get_Attributes(&attributes);
				if (SUCCEEDED(hr)) {
					ComPtr<IXmlNode> editedNode;
					hr = attributes->GetNamedItem(loadStringReference(SrcTag), &editedNode);
					if (SUCCEEDED(hr)) {
						setNodeStringValue(loadStringReference(imagePath), editedNode.Get(), xmlDocument());
					}
				}
			}
		}
	}
	return hr;
}




