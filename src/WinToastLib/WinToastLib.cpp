// WinToastLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WinToastLib.h"
#include "util.h"

std::wstring WinToast::ToastTag = L"toast";
std::wstring WinToast::ImageTag = L"image";
std::wstring WinToast::TextTag = L"text";
std::wstring WinToast::SrcTag = L"src";
WinToast* WinToast::_instance = nullptr;
WinToast* WinToast::instance() {
	if (_instance == nullptr) {
		_instance = new WinToast();
	}
	return _instance;
}

WinToast::WinToast() : _isInitialized(false), _template(WinToastTemplate::UnknownTemplate)
{
	return;
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

bool WinToast::isCompatible() {
	return SUCCEEDED(isRequiredLibrariesAvailables());
}

bool WinToast::initialize() {
	if (_aumi.empty() || _appName.empty()) {
		wcout << L"Error: App User Model Id or Appname is empty!";
		_isInitialized = false;
		return false;
	}

	if (!isCompatible() || FAILED(setupRequiredLibraries())) {
		wcout << L"Your OS is not compatible with this library! =(";
		_isInitialized = false;
		return _isInitialized;
	}


	HRESULT hr = loadAppUserModelId();
	if (FAILED(hr)) {
		WCHAR shellPath[MAX_PATH];
		hr = defaultShellLinkPath(_appName, shellPath);
		if (SUCCEEDED(hr)) {
			hr = createShellLinkInPath(shellPath);
			if (SUCCEEDED(hr)) {
				hr = initAppUserModelId();
				if (FAILED(hr)) {
					wcout << L"Could not create your App User Model Id =(";
				}
			}
		}
	}

	hr = wrap_GetActivationFactory(loadStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager), &_notificationManager);
	if (SUCCEEDED(hr)) {
		hr = notificationManager()->CreateToastNotifierWithId(loadStringReference(_aumi), &_notifier);
		if (SUCCEEDED(hr)) {
			hr = wrap_GetActivationFactory(loadStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotification), &_notificationFactory);
		}
	}

	_isInitialized = SUCCEEDED(hr);
	return _isInitialized;
}


HRESULT	WinToast::initAppUserModelId() {
	WCHAR	slPath[MAX_PATH];
	HRESULT hr = defaultShellLinkPath(_appName, slPath);
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

bool WinToast::showToast(_In_ WinToastTemplate& toast)  {
	if (!isInitialized()) {
		wcout << "WinToastLib not initialized =(";
		return _isInitialized;
	}

	HRESULT hr = S_OK;
	if (_template != toast.type()) {
		_template = toast.type();
		hr = notificationManager()->GetTemplateContent(ToastTemplateType(_template), &_xmlDocument);
	}

	if (SUCCEEDED(hr)) {
		for (int i = 0; i < toast.textFieldsCount() && SUCCEEDED(hr); i++) {
			hr = setTextField(toast.textField(i), i);
		}
		if (SUCCEEDED(hr)) {
			hr = toast.hasImage() ? setImageField(toast.imagePath()) : hr;
			if (SUCCEEDED(hr)) {
				hr = notificationFactory()->CreateToastNotification(xmlDocument(), &_notification);
				if (SUCCEEDED(hr)) {
					hr = setEventHandlers(notification(), toast.handler());
					if (SUCCEEDED(hr)) {
						hr = notifier()->Show(notification());
					}
				}
			}
		}
	}
	return SUCCEEDED(hr);
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




