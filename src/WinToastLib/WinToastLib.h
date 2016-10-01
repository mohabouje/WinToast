// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WINTOASTLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WINTOASTLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINTOASTLIB_EXPORTS
#define WINTOASTLIB_API __declspec(dllexport)
#else
#define WINTOASTLIB_API __declspec(dllimport)
#endif



#include "WinToastHandler.h"
#include <string>
using namespace std;

class WINTOASTLIB_API WinToast {
public:

	WINTOASTLIB_API enum class WinToastTemplate {
		ImageWithOneLine  =ToastTemplateType::ToastTemplateType_ToastImageAndText01,
		ImageWithTwoLines = ToastTemplateType::ToastTemplateType_ToastImageAndText02,
		ImageWithThreeLines = ToastTemplateType::ToastTemplateType_ToastImageAndText03,
		ImageWithFourLines = ToastTemplateType::ToastTemplateType_ToastImageAndText04,
		TextOneLine = ToastTemplateType::ToastTemplateType_ToastText01,
		TextTwoLines = ToastTemplateType::ToastTemplateType_ToastText02,
		TextThreeLines = ToastTemplateType::ToastTemplateType_ToastText02,
		TextFourLines = ToastTemplateType::ToastTemplateType_ToastText02,
		UnknownTemplate = -1
	};

	static WinToast* instance();
	bool isCompatible();
	bool initialize();
	wstring appName() const;
	wstring appUserModelId() const;
	WinToastTemplate currentTemplate() const { return _template; }
	void setAppUserModelId(_In_ const wstring& appName);
	void setAppName(_In_ const wstring& appName);
	void setTemplate(_In_ const WinToastTemplate& templ);

private:
	bool											_isCompatible;
	wstring											_appName;
	wstring											_aumi;
	WinToastTemplate								_template;
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
	HRESULT		setEventHandlers(_In_ ComPtr<WinToastHandler>& eventHandler);
};


