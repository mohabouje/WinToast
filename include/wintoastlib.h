#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H
#include <windows.h>
#include <SDKDDKVer.h>

#include <Shobjidl.h>
#include <wrl/implements.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <shlobj.h>
#include <roapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <winstring.h>
#include <string.h>
#include <vector>

using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;


typedef  ITypedEventHandler<ToastNotification*, ::IInspectable *>                                       ToastActivatedEventHandler;
typedef  ITypedEventHandler<ToastNotification*, ToastDismissedEventArgs *>                              ToastDismissedEventHandler;
typedef  ITypedEventHandler<ToastNotification*, ToastFailedEventArgs *>                                 ToastFailedEventHandler;
typedef Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler>     WinToastTemplateHandler;

#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"
#pragma comment(lib,"shlwapi")


class WinToasTemplate;
class WinToastStringWrapper;
namespace WinToastLib {


    namespace DllImporter {
        typedef HRESULT(FAR STDAPICALLTYPE *f_SetCurrentProcessExplicitAppUserModelID)(__in PCWSTR AppID);
        typedef HRESULT(FAR STDAPICALLTYPE *f_PropVariantToString)(_In_ REFPROPVARIANT propvar, _Out_writes_(cch) PWSTR psz, _In_ UINT cch);
        typedef HRESULT(FAR STDAPICALLTYPE *f_RoGetActivationFactory)(_In_ HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory);
        typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsCreateStringReference)(_In_reads_opt_(length + 1) PCWSTR sourceString, UINT32 length, _Out_ HSTRING_HEADER * hstringHeader, _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING * string);
        typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsDeleteString)(_In_opt_ HSTRING string);

        f_SetCurrentProcessExplicitAppUserModelID    SetCurrentProcessExplicitAppUserModelID;
        f_PropVariantToString                        PropVariantToString;
        f_RoGetActivationFactory                     RoGetActivationFactory;
        f_WindowsCreateStringReference               WindowsCreateStringReference;
        f_WindowsDeleteString                        WindowsDeleteString;


        template<class T>
        _Check_return_ __inline HRESULT _1_GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ T** factory) {
            return RoGetActivationFactory(activatableClassId, IID_INS_ARGS(factory));
        }

        template<typename T>
        inline HRESULT Wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _Inout_ Details::ComPtrRef<T> factory) throw() {
            return _1_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
        }

        inline HRESULT initialize();
    }


    class WinToastStringWrapper {
    public:
        WinToastStringWrapper(_In_reads_(length) PCWSTR stringRef, _In_ UINT32 length) throw() {
            HRESULT hr = DllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
            if (!SUCCEEDED(hr)) {
                RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
            }
        }

        WinToastStringWrapper(_In_ const std::wstring &stringRef) throw()
        {
            HRESULT hr = DllImporter::WindowsCreateStringReference(stringRef.c_str(), static_cast<UINT32>(stringRef.length()), &_header, &_hstring);

            if (FAILED(hr)) {
                RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
            }
        }
        ~WinToastStringWrapper() {
            DllImporter::WindowsDeleteString(_hstring);
        }


        template <size_t N>
        WinToastStringWrapper(_In_reads_(N) wchar_t const (&stringRef)[N]) throw() {
            UINT32 length = N - 1;
            HRESULT hr = DllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
            if (!SUCCEEDED(hr)) {
                RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
            }
        }

        template <size_t _>
        WinToastStringWrapper(_In_reads_(_) wchar_t(&stringRef)[_]) throw() {
            UINT32 length;
            HRESULT hr = SizeTToUInt32(wcslen(stringRef), &length);
            if (!SUCCEEDED(hr)) {
                RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
            }

            DllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
        }

        HSTRING Get() const throw() {
            return _hstring;
        }

    private:
        HSTRING _hstring;
        HSTRING_HEADER _header;

    };


    namespace Util {
        inline HRESULT defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
            DWORD written = GetModuleFileNameEx(GetCurrentProcess(), nullptr, path, nSize);
            std::wcout << "Default executable path: " << path << std::endl;
            return (written > 0) ? S_OK : E_FAIL;
        }


        inline HRESULT defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
            DWORD written = GetEnvironmentVariable(L"APPDATA", path, nSize);
            HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
            if (SUCCEEDED(hr)) {
                errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
                hr = (result == 0) ? S_OK : E_INVALIDARG;
                std::wcout << "Default shell link path: " << path << std::endl;
            }
            return hr;
        }

        inline HRESULT defaultShellLinkPath(const std::wstring& appname, _In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) {
            HRESULT hr = defaultShellLinksDirectory(path, nSize);
            if (SUCCEEDED(hr)) {
                const std::wstring appLink(appname + DEFAULT_LINK_FORMAT);
                errno_t result = wcscat_s(path, nSize, appLink.c_str());
                hr = (result == 0) ? S_OK : E_INVALIDARG;
                std::wcout << "Default shell link file path: " << path << std::endl;
            }
            return hr;
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

        inline HRESULT setEventHandlers(_In_ IToastNotification* notification, _In_ WinToastTemplateHandler* eventHandler) {
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

    class WinToastHandler : public WinToastTemplateHandler{
    public:
        enum WinToastDismissalReason {
            UserCanceled = ToastDismissalReason::ToastDismissalReason_UserCanceled,
            ApplicationHidden = ToastDismissalReason::ToastDismissalReason_ApplicationHidden,
            TimedOut = ToastDismissalReason::ToastDismissalReason_TimedOut
        };

        WinToastHandler(_In_ HWND hToActivate, _In_ HWND hEdit);
        WinToastHandler() {}
        ~WinToastHandler();
        virtual void toastActivated() const;
        virtual void toastDismissed(WinToastDismissalReason state) const;
        virtual void toastFailed() const;

    private:
        IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IInspectable *inspectable);
        IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastDismissedEventArgs *e);
        IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastFailedEventArgs *e);
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv);
    protected:
        ULONG _ref;
        HWND _hToActivate;
        HWND _hEdit;
    };

    class WinToastTemplate
    {
    public:
        enum WinToastTemplateType {
            ImageWithOneLine = ToastTemplateType::ToastTemplateType_ToastImageAndText01,
            ImageWithTwoLines = ToastTemplateType::ToastTemplateType_ToastImageAndText02,
            ImageWithThreeLines = ToastTemplateType::ToastTemplateType_ToastImageAndText03,
            ImageWithFourLines = ToastTemplateType::ToastTemplateType_ToastImageAndText04,
            TextOneLine = ToastTemplateType::ToastTemplateType_ToastText01,
            TextTwoLines = ToastTemplateType::ToastTemplateType_ToastText02,
            TextThreeLines = ToastTemplateType::ToastTemplateType_ToastText03,
            TextFourLines = ToastTemplateType::ToastTemplateType_ToastText04,
            UnknownTemplate = -1
        };

        WinToastTemplate(const WinToastTemplateType& type = ImageWithTwoLines);
        ~WinToastTemplate();
        virtual WinToastHandler* handler() const;

        int                                 textFieldsCount() const { return _textFieldsCount; }
        bool                                hasImage() const { return _hasImage; }
        std::vector<std::wstring>			textFields() const { return _textFields; }
        std::wstring                        textField(int pos) const { return _textFields[pos]; }
        std::wstring                        imagePath() const { return _imagePath; }
        WinToastTemplateType                type() const { return _type; }
        void                                setTextField(const std::wstring& txt, int pos);
        void                                setImagePath(const std::wstring& imgPath);
    private:
        int                                 _textFieldsCount;
        bool                                _hasImage;
        std::vector<std::wstring>			_textFields;
        std::wstring                        _imagePath;
        WinToastTemplateType                _type;
        void initComponentsFromType();
    };

    class WinToast {
    public:
        static WinToast* instance();
        static bool isCompatible();

        bool                initialize();
        bool                isInitialized() const { return _isInitialized; }
        bool                showToast(WinToastTemplate& toast);
        std::wstring        appName() const;
        std::wstring        appUserModelId() const;
        void                setAppUserModelId(_In_ const std::wstring& appName);
        void                setAppName(_In_ const std::wstring& appName);
    private:
        bool											_isInitialized;
        std::wstring                                    _appName;
        std::wstring                                    _aumi;
        ComPtr<IXmlDocument>                            _xmlDocument;
        ComPtr<IToastNotificationManagerStatics>        _notificationManager;
        ComPtr<IToastNotifier>                          _notifier;
        ComPtr<IToastNotificationFactory>               _notificationFactory;
        ComPtr<IToastNotification>                      _notification;
        static WinToast*								_instance;

        WinToast(void);
        IXmlDocument*							xmlDocument() const { return _xmlDocument.Get(); }
        IToastNotifier*							notifier() const { return _notifier.Get(); }
        IToastNotificationFactory*				notificationFactory() const { return _notificationFactory.Get(); }
        IToastNotificationManagerStatics*		notificationManager() const { return _notificationManager.Get(); }
        IToastNotification*						notification() const { return _notification.Get(); }

        HRESULT     validateShellLink();
        HRESULT		createShellLink();
        HRESULT		setImageField(_In_ const std::wstring& path);
        HRESULT     setTextField(_In_ const std::wstring& text, int pos);
    };



}

#endif // WINTOASTLIB_H
