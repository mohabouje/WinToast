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

        WinToastTemplate(_In_ const WinToastTemplateType& type = ImageAndText02);
        ~WinToastTemplate();
        int                                 textFieldsCount() const { return _textFields.size(); }
        bool                                hasImage() const { return _hasImage; }
        std::vector<std::wstring>			textFields() const { return _textFields; }
        std::wstring                        textField(_In_ const TextField& pos) const { return _textFields[pos]; }
        std::wstring                        imagePath() const { return _imagePath; }
        WinToastTemplateType                type() const { return _type; }
        void                                setTextField(_In_ const std::wstring& txt, _In_ const TextField& pos);
        void                                setImagePath(_In_ const std::wstring& imgPath);
    private:
        static int                          TextFieldsCount[WinToastTemplateTypeCount];
        bool                                _hasImage;
        std::vector<std::wstring>			_textFields;
        std::wstring                        _imagePath;
        WinToastTemplateType                _type;
        void initComponentsFromType();
    };

    class WinToast {
    public:
        WinToast(void);
        virtual ~WinToast();
        static WinToast* instance();
        static bool isCompatible();
        static std::wstring configureAUMI(_In_ const std::wstring& company,
                                                    _In_ const std::wstring& name,
                                                    _In_ const std::wstring& surname,
                                                    _In_ const std::wstring& versionInfo
                                                    );
        virtual bool        initialize();
        virtual bool        isInitialized() const { return _isInitialized; }
        virtual INT64       showToast(_In_ const WinToastTemplate& toast, _In_ IWinToastHandler* handler);
        virtual bool        hideToast(_In_ INT64 id);
        std::wstring        appName() const;
        std::wstring        appUserModelId() const;
        void                setAppUserModelId(_In_ const std::wstring& appName);
        void                setAppName(_In_ const std::wstring& appName);

    protected:
        inline IXmlDocument*							xmlDocument() const { return _xmlDocument.Get(); }
        inline IToastNotifier*							notifier() const { return _notifier.Get(); }
        inline IToastNotificationFactory*				notificationFactory() const { return _notificationFactory.Get(); }
        inline IToastNotificationManagerStatics*		notificationManager() const { return _notificationManager.Get(); }
    private:
        bool											_isInitialized;
        std::wstring                                    _appName;
        std::wstring                                    _aumi;
        std::map<INT64, ComPtr<IToastNotification>>     _buffer;
        ComPtr<IXmlDocument>                            _xmlDocument;
        ComPtr<IToastNotificationManagerStatics>        _notificationManager;
        ComPtr<IToastNotifier>                          _notifier;
        ComPtr<IToastNotificationFactory>               _notificationFactory;
        static WinToast*								_instance;

        HRESULT     validateShellLink();
        HRESULT		createShellLink();
        HRESULT		setImageField(_In_ const std::wstring& path);
        HRESULT     setTextField(_In_ const std::wstring& text, _In_ int pos);
    };
}
#endif // WINTOASTLIB_H
