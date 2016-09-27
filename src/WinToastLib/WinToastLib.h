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

// This class is exported from the WinToastLib.dll
class WINTOASTLIB_API WinToast {
public:
	WinToast(void);
	// TODO: add your methods here.
};

extern WINTOASTLIB_API int nWinToastLib;

WINTOASTLIB_API int fnWinToastLib(void);
