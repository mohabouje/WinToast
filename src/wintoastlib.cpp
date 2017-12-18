#include "wintoastlib.h"
#include <memory>
#include <assert.h>

#pragma comment(lib,"shlwapi")
#pragma comment(lib,"user32")

#ifdef NDEBUG
    #define DEBUG_MSG(str) do { } while ( false )
 #else
    #define DEBUG_MSG(str) do { std::wcout << str << std::endl; } while( false )
#endif

// Thanks: https://stackoverflow.com/a/36545162/4297146

typedef LONG NTSTATUS, *PNTSTATUS;

#define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

RTL_OSVERSIONINFOW GetRealOSVersion() {
	HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
	if (hMod) {
		RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
		if (fxPtr != nullptr) {
			RTL_OSVERSIONINFOW rovi = { 0 };
			rovi.dwOSVersionInfoSize = sizeof(rovi);
			if (STATUS_SUCCESS == fxPtr(&rovi)) {
				return rovi;
			}
		}
	}
	RTL_OSVERSIONINFOW rovi = { 0 };
	return rovi;
}

// Quickstart: Handling toast activations from Win32 apps in Windows 10
// https://blogs.msdn.microsoft.com/tiles_and_toasts/2015/10/16/quickstart-handling-toast-activations-from-win32-apps-in-windows-10/

using namespace WinToastLib;
namespace DllImporter {

    // Function load a function from library
    template <typename Function>
	HRESULT loadFunctionFromLibrary(HINSTANCE library, LPCSTR name, Function &func) {
		if (!library) {
			return E_INVALIDARG;
		}
        func = reinterpret_cast<Function>(GetProcAddress(library, name));
        return (func != nullptr) ? S_OK : E_FAIL;
    }

    typedef HRESULT(FAR STDAPICALLTYPE *f_SetCurrentProcessExplicitAppUserModelID)(__in PCWSTR AppID);
    typedef HRESULT(FAR STDAPICALLTYPE *f_PropVariantToString)(_In_ REFPROPVARIANT propvar, _Out_writes_(cch) PWSTR psz, _In_ UINT cch);
    typedef HRESULT(FAR STDAPICALLTYPE *f_RoGetActivationFactory)(_In_ HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory);
    typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsCreateStringReference)(_In_reads_opt_(length + 1) PCWSTR sourceString, UINT32 length, _Out_ HSTRING_HEADER * hstringHeader, _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING * string);
    typedef PCWSTR(FAR STDAPICALLTYPE *f_WindowsGetStringRawBuffer)(_In_ HSTRING string, _Out_ UINT32 *length);
    typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsDeleteString)(_In_opt_ HSTRING string);

    static f_SetCurrentProcessExplicitAppUserModelID    SetCurrentProcessExplicitAppUserModelID;
    static f_PropVariantToString                        PropVariantToString;
    static f_RoGetActivationFactory                     RoGetActivationFactory;
    static f_WindowsCreateStringReference               WindowsCreateStringReference;
    static f_WindowsGetStringRawBuffer                  WindowsGetStringRawBuffer;
    static f_WindowsDeleteString                        WindowsDeleteString;


    template<class T>
    _Check_return_ __inline HRESULT _1_GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ T** factory) {
        return RoGetActivationFactory(activatableClassId, IID_INS_ARGS(factory));
    }

    template<typename T>
    inline HRESULT Wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _Inout_ Details::ComPtrRef<T> factory) throw() {
        return _1_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
    }

    inline HRESULT initialize() {
        HINSTANCE LibShell32 = LoadLibraryW(L"SHELL32.DLL");
        HRESULT hr = loadFunctionFromLibrary(LibShell32, "SetCurrentProcessExplicitAppUserModelID", SetCurrentProcessExplicitAppUserModelID);
        if (SUCCEEDED(hr)) {
            HINSTANCE LibPropSys = LoadLibraryW(L"PROPSYS.DLL");
            hr = loadFunctionFromLibrary(LibPropSys, "PropVariantToString", PropVariantToString);
            if (SUCCEEDED(hr)) {
                HINSTANCE LibComBase = LoadLibraryW(L"COMBASE.DLL");
                const bool succeded = SUCCEEDED(loadFunctionFromLibrary(LibComBase, "RoGetActivationFactory", RoGetActivationFactory))
										&& SUCCEEDED(loadFunctionFromLibrary(LibComBase, "WindowsCreateStringReference", WindowsCreateStringReference))
										&& SUCCEEDED(loadFunctionFromLibrary(LibComBase, "WindowsGetStringRawBuffer", WindowsGetStringRawBuffer))
										&& SUCCEEDED(loadFunctionFromLibrary(LibComBase, "WindowsDeleteString", WindowsDeleteString));
				return succeded ? S_OK : E_FAIL;
            }
        }
        return hr;
    }
}

class WinToastStringWrapper {
public:
    WinToastStringWrapper(_In_reads_(length) PCWSTR stringRef, _In_ UINT32 length) throw() {
        HRESULT hr = DllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
        if (!SUCCEEDED(hr)) {
            RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
        }
    }
    WinToastStringWrapper(_In_ const std::wstring &stringRef) throw() {
        HRESULT hr = DllImporter::WindowsCreateStringReference(stringRef.c_str(), static_cast<UINT32>(stringRef.length()), &_header, &_hstring);
        if (FAILED(hr)) {
            RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
        }
    }
    ~WinToastStringWrapper() {
        DllImporter::WindowsDeleteString(_hstring);
    }
    inline HSTRING Get() const throw() { return _hstring; }
private:
    HSTRING _hstring;
    HSTRING_HEADER _header;

};

class MyDateTime : public IReference<DateTime>
{
protected:
    DateTime _dateTime;

public:
    static INT64 Now() {
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        return ((((INT64)now.dwHighDateTime) << 32) | now.dwLowDateTime);
    }

    MyDateTime(DateTime dateTime) : _dateTime(dateTime) {}

    MyDateTime(INT64 millisecondsFromNow) {
        _dateTime.UniversalTime = Now() + millisecondsFromNow * 10000;
    }

    operator INT64() {
        return _dateTime.UniversalTime;
    }

    HRESULT STDMETHODCALLTYPE get_Value(DateTime *dateTime) {
        *dateTime = _dateTime;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) {
        if (!ppvObject) {
            return E_POINTER;
        }
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IReference<DateTime>)) {
            *ppvObject = static_cast<IUnknown*>(static_cast<IReference<DateTime>*>(this));
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE Release() {
        return 1;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return 2;
    }

    HRESULT STDMETHODCALLTYPE GetIids(ULONG*, IID**) {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING*) {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel*) {
        return E_NOTIMPL;
    }
};

namespace Util {
    inline HRESULT defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
        DWORD written = GetModuleFileNameExW(GetCurrentProcess(), nullptr, path, nSize);
        DEBUG_MSG("Default executable path: " << path);
        return (written > 0) ? S_OK : E_FAIL;
    }


    inline HRESULT defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
        DWORD written = GetEnvironmentVariableW(L"APPDATA", path, nSize);
        HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
        if (SUCCEEDED(hr)) {
            errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
            hr = (result == 0) ? S_OK : E_INVALIDARG;
            DEBUG_MSG("Default shell link path: " << path);
        }
        return hr;
    }

    inline HRESULT defaultShellLinkPath(const std::wstring& appname, _In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
        HRESULT hr = defaultShellLinksDirectory(path, nSize);
        if (SUCCEEDED(hr)) {
            const std::wstring appLink(appname + DEFAULT_LINK_FORMAT);
            errno_t result = wcscat_s(path, nSize, appLink.c_str());
            hr = (result == 0) ? S_OK : E_INVALIDARG;
            DEBUG_MSG("Default shell link file path: " << path);
        }
        return hr;
    }


    inline PCWSTR AsString(ComPtr<IXmlDocument> &xmlDocument) {
        HSTRING xml;
        ComPtr<IXmlNodeSerializer> ser;
        HRESULT hr = xmlDocument.As<IXmlNodeSerializer>(&ser);
        hr = ser->GetXml(&xml);
        if (SUCCEEDED(hr))
            return DllImporter::WindowsGetStringRawBuffer(xml, NULL);
        return NULL;
    }

    inline PCWSTR AsString(HSTRING hstring) {
		return DllImporter::WindowsGetStringRawBuffer(hstring, NULL);
    }

    inline HRESULT setNodeStringValue(const std::wstring& string, IXmlNode *node, IXmlDocument *xml) {
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

    inline HRESULT setEventHandlers(_In_ IToastNotification* notification, _In_ std::shared_ptr<IWinToastHandler> eventHandler, _In_ INT64 expirationTime) {
        EventRegistrationToken activatedToken, dismissedToken, failedToken;
        HRESULT hr = notification->add_Activated(
                    Callback < Implements < RuntimeClassFlags<ClassicCom>,
                    ITypedEventHandler<ToastNotification*, IInspectable* >> >(
                    [eventHandler](IToastNotification*, IInspectable* inspectable)
                {
                    IToastActivatedEventArgs *activatedEventArgs;
                    HRESULT hr = inspectable->QueryInterface(&activatedEventArgs);
                    if (SUCCEEDED(hr)) {
                        HSTRING argumentsHandle;
                        hr = activatedEventArgs->get_Arguments(&argumentsHandle);
                        if (SUCCEEDED(hr)) {
                            PCWSTR arguments = Util::AsString(argumentsHandle);
                            if (arguments && *arguments) {
                                eventHandler->toastActivated((int)wcstol(arguments, NULL, 10));
                                return S_OK;
                            }
                        }
                    }
                    eventHandler->toastActivated();
                    return S_OK;
                }).Get(), &activatedToken);

        if (SUCCEEDED(hr)) {
            hr = notification->add_Dismissed(Callback < Implements < RuntimeClassFlags<ClassicCom>,
                     ITypedEventHandler<ToastNotification*, ToastDismissedEventArgs* >> >(
                     [eventHandler, expirationTime](IToastNotification*, IToastDismissedEventArgs* e)
                 {
                     ToastDismissalReason reason;
                     if (SUCCEEDED(e->get_Reason(&reason)))
                     {
                         if (reason == ToastDismissalReason_UserCanceled && expirationTime && MyDateTime::Now() >= expirationTime)
                            reason = ToastDismissalReason_TimedOut;
                         eventHandler->toastDismissed(static_cast<IWinToastHandler::WinToastDismissalReason>(reason));
                     }
                     return S_OK;
                 }).Get(), &dismissedToken);
            if (SUCCEEDED(hr)) {
                hr = notification->add_Failed(Callback < Implements < RuntimeClassFlags<ClassicCom>,
                    ITypedEventHandler<ToastNotification*, ToastFailedEventArgs* >> >(
                    [eventHandler](IToastNotification*, IToastFailedEventArgs*)
                {
                    eventHandler->toastFailed();
                    return S_OK;
                }).Get(), &failedToken);
            }
        }
        return hr;
    }
}

WinToast* WinToast::instance() {
    static WinToast instance;
    return &instance;
}

WinToast::WinToast() :
    _isInitialized(false),
    _hasCoInitialized(false)
{

	if (!isCompatible()) {
		DEBUG_MSG(L"Warning: Your system is not compatible with this library ");
	}
}

WinToast::~WinToast() {
	_xmlDocument.Reset();
	_notificationManager.Reset();
	_notifier.Reset();
	_notificationFactory.Reset();
	_buffer.clear();
	_aumi.clear();
	_appName.clear();
    if (_hasCoInitialized) {
        CoUninitialize();
    }
}

void WinToast::setAppName(_In_ const std::wstring& appName) {
    _appName = appName;
}




void WinToast::setAppUserModelId(_In_ const std::wstring& aumi) {
    _aumi = aumi;
    DEBUG_MSG(L"Default App User Model Id: " << _aumi.c_str());
}

bool WinToast::isCompatible() {
	DllImporter::initialize();
	return !((DllImporter::SetCurrentProcessExplicitAppUserModelID == nullptr)
		|| (DllImporter::PropVariantToString == nullptr)
		|| (DllImporter::RoGetActivationFactory == nullptr)
		|| (DllImporter::WindowsCreateStringReference == nullptr)
		|| (DllImporter::WindowsDeleteString == nullptr));
}

bool WinToastLib::WinToast::supportActions() {
	RTL_OSVERSIONINFOW tmp = GetRealOSVersion();
	return tmp.dwMajorVersion > 6;

}
std::wstring WinToast::configureAUMI(_In_ const std::wstring &companyName,
                                               _In_ const std::wstring &productName,
                                               _In_ const std::wstring &subProduct,
                                               _In_ const std::wstring &versionInformation)
{
    std::wstring aumi = companyName;
    aumi += L"." + productName;
    if (subProduct.length() > 0) {
        aumi += L"." + subProduct;
        if (versionInformation.length() > 0) {
            aumi += L"." + versionInformation;
        }
    }

    if (aumi.length() > SCHAR_MAX) {
        DEBUG_MSG("Error: max size allowed for AUMI: 128 characters.");
    }
    return aumi;
}


enum WinToast::ShortcutResult WinToast::createShortcut() {
    if (_aumi.empty() || _appName.empty()) {
        DEBUG_MSG(L"Error: App User Model Id or Appname is empty!");
        return SHORTCUT_MISSING_PARAMETERS;
    }

    if (!isCompatible()) {
        DEBUG_MSG(L"Your OS is not compatible with this library! =(");
        return SHORTCUT_INCOMPATIBLE_OS;
    }

    if (!_hasCoInitialized) {
        HRESULT initHr = CoInitializeEx(NULL, COINIT::COINIT_MULTITHREADED);
        if (initHr != RPC_E_CHANGED_MODE) {
            if (FAILED(initHr) && initHr != S_FALSE) {
                DEBUG_MSG(L"Error on COM library initialization!");
                return SHORTCUT_COM_INIT_FAILURE;
            }
            else {
                _hasCoInitialized = true;
            }
        }
    }

    bool wasChanged;
    HRESULT hr = validateShellLinkHelper(wasChanged);
    if (SUCCEEDED(hr))
        return wasChanged ? SHORTCUT_WAS_CHANGED : SHORTCUT_UNCHANGED;

    hr = createShellLinkHelper();
    if (SUCCEEDED(hr))
        return SHORTCUT_WAS_CREATED;
    return SHORTCUT_CREATE_FAILED;
}

bool WinToast::initialize() {
    _isInitialized = false;

    if (createShortcut() < 0)
        return false;

    if (FAILED(DllImporter::SetCurrentProcessExplicitAppUserModelID(_aumi.c_str()))) {
        DEBUG_MSG(L"Error while attaching the AUMI to the current proccess =(");
        return false;
    }

    HRESULT hr = DllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(), &_notificationManager);
    if (SUCCEEDED(hr)) {
        hr = _notificationManager->CreateToastNotifierWithId(WinToastStringWrapper(_aumi).Get(), &_notifier);
        if (SUCCEEDED(hr)) {
            hr = DllImporter::Wrap_GetActivationFactory(WinToastStringWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(), &_notificationFactory);
            if (SUCCEEDED(hr)) {
                _isInitialized = true;
                return true;
            }
        }
    }

    return false;
}

HRESULT	WinToast::validateShellLinkHelper(_Out_ bool& wasChanged) {
	WCHAR	path[MAX_PATH] = { L'\0' };
    Util::defaultShellLinkPath(_appName, path);
    // Check if the file exist
    DWORD attr = GetFileAttributesW(path);
    if (attr >= 0xFFFFFFF) {
        DEBUG_MSG("Error, shell link not found. Try to create a new one in: " << path);
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
            hr = persistFile->Load(path, STGM_READWRITE);
            if (SUCCEEDED(hr)) {
                ComPtr<IPropertyStore> propertyStore;
                hr = shellLink.As(&propertyStore);
                if (SUCCEEDED(hr)) {
                    PROPVARIANT appIdPropVar;
                    hr = propertyStore->GetValue(PKEY_AppUserModel_ID, &appIdPropVar);
                    if (SUCCEEDED(hr)) {
                        WCHAR AUMI[MAX_PATH];
                        hr = DllImporter::PropVariantToString(appIdPropVar, AUMI, MAX_PATH);
                        wasChanged = false;
                        if (FAILED(hr) || _aumi != AUMI) {
                            // AUMI Changed for the same app, let's update the current value! =)
                            wasChanged = true;
                            PropVariantClear(&appIdPropVar);
                            hr = InitPropVariantFromString(_aumi.c_str(), &appIdPropVar);
                            if (SUCCEEDED(hr)) {
                                hr = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
                                if (SUCCEEDED(hr)) {
                                    hr = propertyStore->Commit();
                                    if (SUCCEEDED(hr) && SUCCEEDED(persistFile->IsDirty())) {
                                        hr = persistFile->Save(path, TRUE);
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
    return hr;
}



HRESULT	WinToast::createShellLinkHelper() {
	WCHAR   exePath[MAX_PATH]{L'\0'};
	WCHAR	slPath[MAX_PATH]{L'\0'};
    Util::defaultShellLinkPath(_appName, slPath);
    Util::defaultExecutablePath(exePath);
    ComPtr<IShellLinkW> shellLink;
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
                            PropVariantClear(&appIdPropVar);
                        }
                    }
                }
            }
        }
    }
    return hr;
}

INT64 WinToast::showToast(_In_ const WinToastTemplate& toast, _In_  IWinToastHandler* handler)  {
    INT64 id = -1;
    if (!isInitialized()) {
        DEBUG_MSG("Error when launching the toast. WinToast is not initialized =(");
        return id;
    }
    if (!handler) {
        DEBUG_MSG("Error when launching the toast. handler cannot be null.");
        return id;
    }

    HRESULT hr = _notificationManager->GetTemplateContent(ToastTemplateType(toast.type()), &_xmlDocument);
    if (SUCCEEDED(hr)) {
        const int fieldsCount = toast.textFieldsCount();
        for (int i = 0; i < fieldsCount && SUCCEEDED(hr); i++) {
            hr = setTextFieldHelper(toast.textField(WinToastTemplate::TextField(i)), i);
        }
		bool modernActions = supportActions();
		if (!modernActions) DEBUG_MSG("Modern Actions not supported in this os version");
		if (SUCCEEDED(hr) && modernActions) {
            const int actionsCount = toast.actionsCount();
            WCHAR buf[12];
            for (int i = 0; i < actionsCount && SUCCEEDED(hr); i++) {
                _snwprintf_s(buf, sizeof(buf) / sizeof(*buf), _TRUNCATE, L"%d", i);
                hr = addActionHelper(toast.actionLabel(i), buf);
            }
            DEBUG_MSG("xml: " << Util::AsString(_xmlDocument));
        }
        if (SUCCEEDED(hr)) {
            hr = toast.hasImage() ? setImageFieldHelper(toast.imagePath()) : hr;
            if (SUCCEEDED(hr)) {
                ComPtr<IToastNotification> notification;
                hr = _notificationFactory->CreateToastNotification(_xmlDocument.Get(), &notification);
                if (SUCCEEDED(hr)) {
                    INT64 expiration = 0, relativeExpiration = toast.expiration();
                    if (relativeExpiration > 0) {
                        MyDateTime expirationDateTime(relativeExpiration);
                        expiration = expirationDateTime;
                        hr = notification->put_ExpirationTime(&expirationDateTime);
                    }
                    if (SUCCEEDED(hr)) {
                        hr = Util::setEventHandlers(notification.Get(), std::shared_ptr<IWinToastHandler>(handler), expiration);
                    }
                    if (SUCCEEDED(hr)) {
                        GUID guid;
                        hr = CoCreateGuid(&guid);
                        if (SUCCEEDED(hr)) {
                            id = guid.Data1;
                            _buffer[id] = notification;
                            hr = _notifier->Show(notification.Get());
                        }
                    }
                }
            }
        }
    }
    return FAILED(hr) ? -1 : id;
}

bool WinToast::hideToast(_In_ INT64 id) {
    if (!isInitialized()) {
        DEBUG_MSG("Error when hiding the toast. WinToast is not initialized.");
        return false;
    }
    const bool find = _buffer.find(id) != _buffer.end();
    _notifier->Hide(_buffer[id].Get());
    return find;
}

void WinToast::clear() {
    auto end = _buffer.end();
    for (auto it = _buffer.begin(); it != end; ++it) {
        _notifier->Hide(it->second.Get());
    }
    _buffer.clear();
}


HRESULT WinToast::setTextFieldHelper(_In_ const std::wstring& text, _In_ int pos) {
    ComPtr<IXmlNodeList> nodeList;
    HRESULT hr = _xmlDocument->GetElementsByTagName(WinToastStringWrapper(L"text").Get(), &nodeList);
    if (SUCCEEDED(hr)) {
        ComPtr<IXmlNode> node;
        hr = nodeList->Item(pos, &node);
        if (SUCCEEDED(hr)) {
            hr = Util::setNodeStringValue(text, node.Get(), _xmlDocument.Get());
        }
    }
    return hr;
}


HRESULT WinToast::setImageFieldHelper(_In_ const std::wstring& path)  {
    wchar_t imagePath[MAX_PATH] = L"file:///";
    HRESULT hr = StringCchCatW(imagePath, MAX_PATH, path.c_str());
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
                        Util::setNodeStringValue(imagePath, editedNode.Get(), _xmlDocument.Get());
                    }
                }
            }
        }
    }
    return hr;
}

HRESULT WinToast::addActionHelper(_In_ const std::wstring& content, _In_ const std::wstring& arguments) {
	ComPtr<IXmlNodeList> nodeList;
	HRESULT hr = _xmlDocument->GetElementsByTagName(WinToastStringWrapper(L"actions").Get(), &nodeList);
	if (FAILED(hr))
		return hr;
	UINT32 length;
	hr = nodeList->get_Length(&length);
	if (FAILED(hr))
		return hr;
	ComPtr<IXmlNode> actionsNode;
	if (length > 0)
		hr = nodeList->Item(0, &actionsNode);
	else {
		hr = _xmlDocument->GetElementsByTagName(WinToastStringWrapper(L"toast").Get(), &nodeList);
		if (FAILED(hr))
			return hr;
		hr = nodeList->get_Length(&length);
		if (FAILED(hr))
			return hr;
		ComPtr<IXmlNode> toastNode;
		hr = nodeList->Item(0, &toastNode);
		if (FAILED(hr))
			return hr;
		ComPtr<IXmlElement> toastElement;
		hr = toastNode.As(&toastElement);
		if (SUCCEEDED(hr))
                    hr = toastElement->SetAttribute(WinToastStringWrapper(L"template").Get(), WinToastStringWrapper(L"ToastGeneric").Get());
		if (SUCCEEDED(hr))
                    hr = toastElement->SetAttribute(WinToastStringWrapper(L"duration").Get(), WinToastStringWrapper(L"long").Get());
		if (FAILED(hr))
			return hr;
		ComPtr<IXmlElement> actionsElement;
		hr = _xmlDocument->CreateElement(WinToastStringWrapper(L"actions").Get(), &actionsElement);
		if (FAILED(hr))
			return hr;
		hr = actionsElement.As(&actionsNode);
		if (FAILED(hr))
			return hr;
		ComPtr<IXmlNode> appendedChild;
		hr = toastNode->AppendChild(actionsNode.Get(), &appendedChild);
	}
	if (FAILED(hr))
		return hr;
	ComPtr<IXmlElement> actionElement;
	hr = _xmlDocument->CreateElement(WinToastStringWrapper(L"action").Get(), &actionElement);
	if (SUCCEEDED(hr))
		hr = actionElement->SetAttribute(WinToastStringWrapper(L"content").Get(), WinToastStringWrapper(content).Get());
	if (SUCCEEDED(hr))
		hr = actionElement->SetAttribute(WinToastStringWrapper(L"arguments").Get(), WinToastStringWrapper(arguments).Get());
	if (FAILED(hr))
		return hr;
	ComPtr<IXmlNode> actionNode;
	hr = actionElement.As(&actionNode);
	if (FAILED(hr))
		return hr;
	ComPtr<IXmlNode> appendedChild;
	hr = actionsNode->AppendChild(actionNode.Get(), &appendedChild);
	return hr;
}

WinToastTemplate::WinToastTemplate(_In_ WinToastTemplateType type) : _type(type) {
    static const int TextFieldsCount[] = { 1, 2, 2, 3, 1, 2, 2, 3};
    _textFields = std::vector<std::wstring>(TextFieldsCount[_type], L"");
}

WinToastTemplate::~WinToastTemplate() {
    _textFields.clear();
}

void WinToastTemplate::setTextField(_In_ const std::wstring& txt, _In_ WinToastTemplate::TextField pos) {
    _textFields[pos] = txt;
}

void WinToastTemplate::setImagePath(_In_ const std::wstring& imgPath) {
    _imagePath = imgPath;
}

void WinToastLib::WinToastTemplate::addAction(const std::wstring & label)
{
	_actions.push_back(label);
}
