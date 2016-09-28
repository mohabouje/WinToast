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

#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"

#include <string>
using namespace std;

class WINTOASTLIB_API WinToast {
public:
	WinToast(void);
	// TODO: add your methods here.
	bool isCompatible();
	bool initialize();
	wstring appName() const;
	wstring appUserModelId() const;
	void setAppUserModelId(_In_ const wstring& appName);
	void setAppName(_In_ const wstring& appName);

private:
	HRESULT		loadAppUserModelId();
	HRESULT		initAppUserModelId();
	HRESULT     defaultExecutablePath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) const;
	HRESULT		defaultShellLinksDirectory(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) const;
	HRESULT		defaultShellLinkPath(_In_ WCHAR* path, _In_ DWORD nSize = MAX_PATH) const;
	HRESULT     createShellLinkInPath(_In_ PCWSTR exePath) const;

private:
	bool	_isCompatible;
	wstring	_appName;
	wstring	_aumi;
};

extern WINTOASTLIB_API int nWinToastLib;

WINTOASTLIB_API int fnWinToastLib(void);
