#include "stdafx.h"
#include "WinToastLibExporter.h"
#include "WinToastDllImporter.h"
#include "WinToastStringWrapper.h"
#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"

namespace WinToastUtil {

	extern WINTOASTLIB_API inline HRESULT defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
		DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, path, nSize);
		wcout << "Default executable path: " << path << endl;
		return (written > 0) ? S_OK : E_FAIL;
	}


	extern WINTOASTLIB_API inline HRESULT defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
		DWORD written = GetEnvironmentVariable(L"APPDATA", path, nSize);
		HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
		if (SUCCEEDED(hr)) {
			errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
			hr = (result == 0) ? S_OK : E_INVALIDARG;
			wcout << "Default shell link path: " << path << endl;
		}
		return hr;
	}

	extern WINTOASTLIB_API inline HRESULT defaultShellLinkPath(const std::wstring& appname, _In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
		HRESULT hr = defaultShellLinksDirectory(path, nSize);
		if (SUCCEEDED(hr)) {
			const wstring appLink(appname + DEFAULT_LINK_FORMAT);
			errno_t result = wcscat_s(path, nSize, appLink.c_str());
			hr = (result == 0) ? S_OK : E_INVALIDARG;
			wcout << "Default shell link file path: " << path << endl;
		}
		return hr;
	}


	extern WINTOASTLIB_API inline HRESULT setNodeStringValue(const std::wstring& string, IXmlNode *node, IXmlDocument *xml) {
		ComPtr<IXmlText> textNode;
		HRESULT hr = xml->CreateTextNode( WinToastStringWrapper(string).Get(), &textNode);
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

}
