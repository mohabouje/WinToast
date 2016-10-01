#ifdef WINTOASTLIB_EXPORTS
#define WINTOASTLIB_API __declspec(dllexport)
#else
#define WINTOASTLIB_API __declspec(dllimport)
#endif

#include "WinToastTemplate.h"
#include "stdafx.h"
class WINTOASTLIB_API WinToast {
public:
	static WinToast* instance();
	static bool isCompatible();

	bool initialize();
	bool isInitialized() const { return _isInitialized; }
	bool showToast(WinToastTemplate& toast);
	wstring appName() const;
	wstring appUserModelId() const;
    void setAppUserModelId(_In_ const std::wstring& appName);
    void setAppName(_In_ const std::wstring& appName);
private:
	bool											_isInitialized;
	wstring											_appName;
	wstring											_aumi;
	WinToastTemplate::WinToastTemplateType			_template;
	ComPtr<IXmlDocument>                            _xmlDocument;
	ComPtr<IToastNotificationManagerStatics>        _notificationManager;
	ComPtr<IToastNotifier>                          _notifier;
	ComPtr<IToastNotificationFactory>               _notificationFactory;
	ComPtr<IToastNotification>                      _notification;
	static WinToast*								_instance;
	static wstring									ToastTag;
	static wstring									ImageTag;
	static wstring									TextTag;
	static wstring									SrcTag;

	WinToast(void);
	IXmlDocument*							xmlDocument() const { return _xmlDocument.Get(); }
	IToastNotifier*							notifier() const { return _notifier.Get(); }
	IToastNotificationFactory*				notificationFactory() const { return _notificationFactory.Get(); }
	IToastNotificationManagerStatics*		notificationManager() const { return _notificationManager.Get(); }
	IToastNotification*						notification() const { return _notification.Get(); }

	HRESULT		initAppUserModelId();
	HRESULT		loadAppUserModelId();
	HRESULT		setImageField(_In_ const wstring& path);
	HRESULT     setTextField(_In_ const wstring& text, int pos);
};


