#include "stdafx.h"

#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"

typedef HRESULT(FAR STDAPICALLTYPE *f_windowsCreateStringReference)(_In_reads_opt_(length + 1) PCWSTR sourceString, UINT32 length, _Out_ HSTRING_HEADER * hstringHeader, _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING * string);
f_windowsCreateStringReference windowsCreateStringReference = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_windowsDeleteString)(_In_opt_ HSTRING string);
f_windowsDeleteString windowsDeleteString = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_setCurrentProcessExplicitAppUserModelID)(__in PCWSTR AppID);
f_setCurrentProcessExplicitAppUserModelID setCurrentProcessExplicitAppUserModelID = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_roGetActivationFactory)(_In_ HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory);
f_roGetActivationFactory roGetActivationFactory = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_propVariantToString)(_In_ REFPROPVARIANT propvar, _Out_writes_(cch) PWSTR psz, _In_ UINT cch);
f_propVariantToString propVariantToString = 0;

template<class T>
_Check_return_ __inline HRESULT Wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ T** factory) {
	return roGetActivationFactory(activatableClassId, IID_INS_ARGS(factory));
}

template<typename T>
inline HRESULT wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _Inout_ Details::ComPtrRef<T> factory) throw() {
	return Wrap_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
}

template <typename TFunction>
inline HRESULT tryLoadFunction(HINSTANCE dll, LPCSTR name, TFunction &func) {
	if (!dll) return false;

	func = (TFunction)GetProcAddress(dll, name);
	return (!!func) ? S_OK : E_FAIL;
}



static const int RequiredStaticLibrariesCount = 4;
static const std::wstring RequiredStaticLibraries[RequiredStaticLibrariesCount] = { L"PROPSYS.DLL", L"api-ms-win-core-winrt-string-l1-1-0.dll", L"COMBASE.DLL", L"SHELL32.DLL" };
extern WINTOASTLIB_API inline HRESULT isRequiredLibrariesAvailables() {
	for (int i = 0; i < RequiredStaticLibrariesCount; i++) {
		if (LoadLibrary(RequiredStaticLibraries[i].c_str()) == NULL)
			return E_FAIL;
	}
	return S_OK;
}


extern WINTOASTLIB_API inline HRESULT setupRequiredLibraries() {
	HINSTANCE procId = LoadLibrary(RequiredStaticLibraries[0].c_str());
	if (SUCCEEDED(tryLoadFunction(procId, "PropVariantToString", propVariantToString))) {
		procId = LoadLibrary(RequiredStaticLibraries[1].c_str());
		if (SUCCEEDED(tryLoadFunction(procId, "WindowsCreateStringReference", windowsCreateStringReference))
			&& SUCCEEDED(tryLoadFunction(procId, "WindowsDeleteString", windowsDeleteString))) {
			procId = LoadLibrary(RequiredStaticLibraries[2].c_str());
			if (SUCCEEDED(tryLoadFunction(procId, "SetCurrentProcessExplicitAppUserModelID", setCurrentProcessExplicitAppUserModelID))) {
				procId = LoadLibrary(RequiredStaticLibraries[3].c_str());
				if (SUCCEEDED(tryLoadFunction(procId, "RoGetActivationFactory", roGetActivationFactory)))
					return false;
			}
		}
	}
	return E_FAIL;
}


extern WINTOASTLIB_API inline HRESULT defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
	DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, path, nSize);
	return (written > 0) ? S_OK : E_FAIL;
}


extern WINTOASTLIB_API inline HRESULT defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
	DWORD written = GetEnvironmentVariable(L"APPDATA", path, nSize);
	HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
	if (SUCCEEDED(hr)) {
		errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
		hr = (result == 0) ? S_OK : E_INVALIDARG;
	}
	return hr;
}

extern WINTOASTLIB_API inline HRESULT defaultShellLinkPath(const std::wstring& appname, _In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
	HRESULT hr = defaultShellLinksDirectory(path, nSize);
	if (SUCCEEDED(hr)) {
		const wstring appLink(appname + DEFAULT_LINK_FORMAT);
		errno_t result = wcscat_s(path, nSize, appLink.c_str());
		hr = (result == 0) ? S_OK : E_INVALIDARG;
	}
	return hr;
}


extern WINTOASTLIB_API inline HSTRING	loadStringReference(_In_ std::wstring data) {
	HSTRING_HEADER hstringHeader;
	HSTRING string;
	windowsCreateStringReference(data.c_str(), static_cast<UINT32>(data.length()), &hstringHeader, &string);
	return string;
}

extern WINTOASTLIB_API inline HRESULT getNodeListByTag(const std::wstring tag, ComPtr<IXmlNodeList>& nodeList, IXmlDocument *xml) {
	return xml->GetElementsByTagName(loadStringReference(tag), &nodeList);
}

extern WINTOASTLIB_API inline HRESULT getNodeFromNodeList(ComPtr<IXmlNodeList>& nodeList, ComPtr<IXmlNode>& node, int pos) {
	ComPtr<IXmlNode> textNode;
	return nodeList->Item(pos, &node);
}

extern WINTOASTLIB_API inline HRESULT setNodeStringValue(const HSTRING &string, IXmlNode *node, IXmlDocument *xml) {
	ComPtr<IXmlText> textNode;
	HRESULT hr = xml->CreateTextNode(string, &textNode);
	if (SUCCEEDED(hr)) {
		ComPtr<IXmlNode> stringNode;
		hr = textNode.As(&stringNode);
		if (SUCCEEDED(hr)) {
			ComPtr<IXmlNode> appendedChild;
			hr = node->AppendChild(stringNode.Get(), &appendedChild);
		}
	}
	return hr;
}

extern WINTOASTLIB_API inline HRESULT setEventHandlers(_In_ IToastNotification* notification, _In_ WinToastHandler* eventHandler) {
	EventRegistrationToken activatedToken, dismissedToken, failedToken;
	HRESULT hr = notification->add_Activated(eventHandler, &activatedToken);
	if (SUCCEEDED(hr)) {
		hr = notification->add_Dismissed(eventHandler, &dismissedToken);
		if (SUCCEEDED(hr)) {
			hr = notification->add_Failed(eventHandler, &failedToken);
		}
	}
	return hr;
}

extern WINTOASTLIB_API inline HRESULT createShellLinkInPath(_In_ PCWSTR path) {
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