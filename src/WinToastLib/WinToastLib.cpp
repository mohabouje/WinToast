// WinToastLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WinToastLib.h"
#include "WinToastUtil.h"
#include "WinToastDllImporter.h"
WinToast* WinToast::_instance = nullptr;
WinToast* WinToast::instance() {
	if (_instance == nullptr) {
		_instance = new WinToast();
	}
	return _instance;
}

WinToast::WinToast() : _isInitialized(false)
{
    WinToastDllImporter::initialize();
}

void WinToast::setAppName(_In_ const std::wstring& appName) {
	_appName = appName;
}

wstring WinToast::appName() const {
	return _appName;
}

wstring WinToast::appUserModelId() const {
	return _aumi;
}
void WinToast::setAppUserModelId(_In_ const std::wstring& aumi) {
	_aumi = aumi;
}

bool WinToast::isCompatible() {
        return !((WinToastDllImporter::SetCurrentProcessExplicitAppUserModelID == nullptr)
			|| (WinToastDllImporter::PropVariantToString == nullptr)
			|| (WinToastDllImporter::RoGetActivationFactory == nullptr)
			|| (WinToastDllImporter::WindowsCreateStringReference == nullptr)
			|| (WinToastDllImporter::WindowsDeleteString == nullptr));
}

bool WinToast::initialize() {
	if (_aumi.empty() || _appName.empty()) {
		wcout << L"Error: App User Model Id or Appname is empty!";
		_isInitialized = false;
		return false;
	}

	if (!isCompatible()) {
		wcout << L"Your OS is not compatible with this library! =(";
		_isInitialized = false;
		return _isInitialized;
	}

	// Validate the last Shell Link - Shoul have the same AUMI.
	HRESULT hr = validateShellLink();
	if (FAILED(hr)) {
		hr = createShellLink();
	}

	if (SUCCEEDED(hr)) {
		hr = WinToastDllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(), &_notificationManager);
		if (SUCCEEDED(hr)) {
			hr = notificationManager()->CreateToastNotifierWithId(WinToastStringWrapper(_aumi).Get(), &_notifier);
			if (SUCCEEDED(hr)) {
				hr = WinToastDllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(), &_notificationFactory);
			}
		}
	}

	

	_isInitialized = SUCCEEDED(hr);
	return _isInitialized;
}

HRESULT	WinToast::validateShellLink() {
	
	WCHAR	_path[MAX_PATH];
	WinToastUtil::defaultShellLinkPath(_appName, _path);
	// Check if the file exist
	DWORD attr = GetFileAttributes(_path);
	if (attr >= 0xFFFFFFF) {
		wcout << "Error, file not found: " << _path;
		return E_FAIL;
	}

	// Let's load the file as shell link to validate.
	// - Create a shell link
	// - Create a persistant file
	// - Load the path as data for the persistant file
	// - Read the property AUMI and validate with the current
	// - Review if AUMI is equal.
	ComPtr<IShellLink> shellLink;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
	if (SUCCEEDED(hr)) {
		ComPtr<IPersistFile> persistFile;
		hr = shellLink.As(&persistFile);
		if (SUCCEEDED(hr)) {
			hr = persistFile->Load(_path, STGM_READWRITE);
			if (SUCCEEDED(hr)) {
				ComPtr<IPropertyStore> propertyStore;
				hr = shellLink.As(&propertyStore);
				if (SUCCEEDED(hr)) {
					PROPVARIANT appIdPropVar;
					hr = propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar);
					if (SUCCEEDED(hr)) {
						WCHAR AUMI[MAX_PATH];
						hr = WinToastDllImporter::PropVariantToString(appIdPropVar, AUMI, MAX_PATH);
						if (SUCCEEDED(hr)) {
							hr = (_aumi == AUMI) ? S_OK : E_FAIL;
						}
					}
					PropVariantClear(&appIdPropVar);
				}
			}
		}
	}
	return hr;
}



HRESULT	WinToast::createShellLink() {
	WCHAR   exePath[MAX_PATH];
	WCHAR	slPath[MAX_PATH];
	WinToastUtil::defaultShellLinkPath(_appName, slPath);
	WinToastUtil::defaultExecutablePath(exePath);
	ComPtr<IShellLink> shellLink;
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
	if (SUCCEEDED(hr)) {
		hr = shellLink->SetPath(exePath);
		if (SUCCEEDED(hr)) {
			hr = shellLink->SetArguments(L"");
			if (SUCCEEDED(hr)) {
				hr = shellLink->SetWorkingDirectory(exePath);
				if (SUCCEEDED(hr)) {
					ComPtr<IPropertyStore> propertyStore;
					hr = shellLink.As(&propertyStore);
					if (SUCCEEDED(hr)) {
						PROPVARIANT appIdPropVar;
						hr = InitPropVariantFromString(_aumi.c_str(), &appIdPropVar);
						if (SUCCEEDED(hr)) {
							hr = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
							if (SUCCEEDED(hr)) {
								hr = propertyStore->Commit();
								if (SUCCEEDED(hr)) {
									ComPtr<IPersistFile> persistFile;
									hr = shellLink.As(&persistFile);
									if (SUCCEEDED(hr)) {
										hr = persistFile->Save(slPath, TRUE);
									}
								}
							}
						}
						PropVariantClear(&appIdPropVar);
					}
				}
			}
		}
	}
	CoTaskMemFree(exePath);
	CoTaskMemFree(slPath);
	return hr;
}



bool WinToast::showToast(_In_ WinToastTemplate& toast)  {
	if (!isInitialized()) {
		return _isInitialized;
	}

	HRESULT hr = _notificationManager->GetTemplateContent(ToastTemplateType(toast.type()), &_xmlDocument);
	if (SUCCEEDED(hr)) {
		for (int i = 0; i < toast.textFieldsCount() && SUCCEEDED(hr); i++) {
			hr = setTextField(toast.textField(i), i);
		}
		if (SUCCEEDED(hr)) {
			hr = toast.hasImage() ? setImageField(toast.imagePath()) : hr;
			if (SUCCEEDED(hr)) {
				hr = _notificationFactory->CreateToastNotification(xmlDocument(), &_notification);
				if (SUCCEEDED(hr)) {
					hr = WinToastUtil::setEventHandlers(notification(), toast.handler());
					if (SUCCEEDED(hr)) {
						hr = _notifier->Show(notification());
					}
				}
			}
		}
	} 

	return SUCCEEDED(hr);
}


HRESULT WinToast::setTextField(_In_ const wstring& text, int pos) {
	ComPtr<IXmlNodeList> nodeList;
    HRESULT hr = _xmlDocument->GetElementsByTagName(WinToastStringWrapper(L"text").Get(), &nodeList);
    if (SUCCEEDED(hr)) {
		ComPtr<IXmlNode> node;
		hr = nodeList->Item(pos, &node);
		if (SUCCEEDED(hr)) {
			hr = WinToastUtil::setNodeStringValue(text, node.Get(), xmlDocument());
		}
	}
	return hr;
}


HRESULT WinToast::setImageField(_In_ const wstring& path)  {
	wchar_t imagePath[MAX_PATH] = L"file:///";
	HRESULT hr = StringCchCat(imagePath, MAX_PATH, path.c_str());
	if (SUCCEEDED(hr)) {
		ComPtr<IXmlNodeList> nodeList;
		HRESULT hr = _xmlDocument->GetElementsByTagName(WinToastStringWrapper(L"image").Get(), &nodeList);
		if (SUCCEEDED(hr)) {
			ComPtr<IXmlNode> node;
			hr = nodeList->Item(0, &node);
			if (SUCCEEDED(hr))  {
				ComPtr<IXmlNamedNodeMap> attributes;
				hr = node->get_Attributes(&attributes);
				if (SUCCEEDED(hr)) {
					ComPtr<IXmlNode> editedNode;
					hr = attributes->GetNamedItem(WinToastStringWrapper(L"src").Get(), &editedNode);
					if (SUCCEEDED(hr)) {
						WinToastUtil::setNodeStringValue(imagePath, editedNode.Get(), xmlDocument());
					}
				}
			}
		}
	}
	return hr;
}




