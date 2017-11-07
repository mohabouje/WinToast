#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H
#include <Windows.h>
#include <sdkddkver.h>
#include <WinUser.h>
#include <ShObjIdl.h>
#include <wrl/implements.h>
#include <wrl/event.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <ShlObj.h>
#include <roapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <winstring.h>
#include <string.h>
#include <vector>
#include <map>
using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;

#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"
namespace WinToastLib {
    class IWinToastHandler {
    public:
        enum WinToastDismissalReason {
            UserCanceled = ToastDismissalReason::ToastDismissalReason_UserCanceled,
            ApplicationHidden = ToastDismissalReason::ToastDismissalReason_ApplicationHidden,
            TimedOut = ToastDismissalReason::ToastDismissalReason_TimedOut
        };
        virtual void toastActivated() const = 0;
        virtual void toastDismissed(WinToastDismissalReason state) const = 0;
        virtual void toastFailed() const = 0;
    };

    class WinToastTemplate {
    public:
        enum TextField { FirstLine = 0, SecondLine, ThirdLine };
        enum WinToastTemplateType {
            ImageAndText01 = ToastTemplateType::ToastTemplateType_ToastImageAndText01,
            ImageAndText02 = ToastTemplateType::ToastTemplateType_ToastImageAndText02,
            ImageAndText03 = ToastTemplateType::ToastTemplateType_ToastImageAndText03,
            ImageAndText04 = ToastTemplateType::ToastTemplateType_ToastImageAndText04,
            Text01 = ToastTemplateType::ToastTemplateType_ToastText01,
            Text02 = ToastTemplateType::ToastTemplateType_ToastText02,
            Text03 = ToastTemplateType::ToastTemplateType_ToastText03,
            Text04 = ToastTemplateType::ToastTemplateType_ToastText04,
            WinToastTemplateTypeCount
        };

        WinToastTemplate(_In_ WinToastTemplateType type = ImageAndText02);
        ~WinToastTemplate();
        void                                        setTextField(_In_ const std::wstring& txt, _In_ TextField pos);
        void                                        setImagePath(_In_ const std::wstring& imgPath);
        inline int                                  textFieldsCount() const { return static_cast<int>(_textFields.size()); }
        inline bool                                 hasImage() const { return _hasImage; }
        inline std::vector<std::wstring>            textFields() const { return _textFields; }
        inline std::wstring                         textField(_In_ TextField pos) const { return _textFields[pos]; }
        inline std::wstring                         imagePath() const { return _imagePath; }
        inline WinToastTemplateType                 type() const { return _type; }
    private:
        bool                                _hasImage;
        std::vector<std::wstring>			_textFields;
        std::wstring                        _imagePath;
        WinToastTemplateType                _type;
    };

    class WinToast {
    public:
        WinToast(void);
        virtual ~WinToast();
        static WinToast* instance();
        static bool             isCompatible();
        static std::wstring     configureAUMI(_In_ const std::wstring& companyName,
                                                    _In_ const std::wstring& productName,
                                                    _In_ const std::wstring& subProduct = std::wstring(),
                                                    _In_ const std::wstring& versionInformation = std::wstring()
                                                    );
        virtual bool            initialize();
        virtual bool            isInitialized() const { return _isInitialized; }
        virtual INT64           showToast(_In_ const WinToastTemplate& toast, _In_ IWinToastHandler* handler);
        virtual bool            hideToast(_In_ INT64 id);
        virtual void            clear();
        inline std::wstring     appName() const { return _appName; }
        inline std::wstring     appUserModelId() const { return _aumi; }
        void                    setAppUserModelId(_In_ const std::wstring& appName);
        void                    setAppName(_In_ const std::wstring& appName);
    protected:
        bool											_isInitialized;
        bool                                            _hasCoInitialized;
        std::wstring                                    _appName;
        std::wstring                                    _aumi;
        std::map<INT64, ComPtr<IToastNotification>>     _buffer;
        ComPtr<IXmlDocument>                            _xmlDocument;
        ComPtr<IToastNotificationManagerStatics>        _notificationManager;
        ComPtr<IToastNotifier>                          _notifier;
        ComPtr<IToastNotificationFactory>               _notificationFactory;
        static WinToast*								_instance;

        HRESULT     validateShellLinkHelper();
        HRESULT		createShellLinkHelper();
        HRESULT		setImageFieldHelper(_In_ const std::wstring& path);
        HRESULT     setTextFieldHelper(_In_ const std::wstring& text, _In_ int pos);
    };
}
#endif // WINTOASTLIB_H
