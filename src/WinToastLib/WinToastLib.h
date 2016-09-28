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
#include <string>
using namespace std;

class WINTOASTLIB_API WinToast {
public:
	WinToast(void);
	// TODO: add your methods here.
	bool isCompatible();
	void setAppName(const string& appName);
	string appName() const;

private:
	HRESULT		loadAppUserModelId();
	HRESULT		defaultAppUserModelIdDirectory(_In_ WCHAR* path, _In_ DWORD nSize) const;
private:
	bool	_isCompatible;
	string	_appName;
	wstring	_aumi;
};

extern WINTOASTLIB_API int nWinToastLib;

WINTOASTLIB_API int fnWinToastLib(void);
