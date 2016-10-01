#include "stdafx.h"
#ifdef WINTOASTLIB_EXPORTS
#define WINTOASTLIB_API __declspec(dllexport)
#else
#define WINTOASTLIB_API __declspec(dllimport)
#endif


typedef HRESULT(FAR STDAPICALLTYPE *f_windowsCreateStringReference)(_In_reads_opt_(length + 1) PCWSTR sourceString, UINT32 length, _Out_ HSTRING_HEADER * hstringHeader, _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING * string);
f_windowsCreateStringReference windowsCreateStringReference = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_windowsDeleteString)(_In_opt_ HSTRING string);
f_windowsDeleteString windowsDeleteString = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_setCurrentProcessExplicitAppUserModelID)(__in PCWSTR AppID);
f_setCurrentProcessExplicitAppUserModelID setCurrentProcessExplicitAppUserModelID = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_roGetActivationFactory)(_In_ HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory);
f_roGetActivationFactory roGetActivationFactory = 0;

typedef HRESULT(FAR STDAPICALLTYPE *f_propVariantToString)(_In_ REFPROPVARIANT propvar, _Out_writes_(cch) PWSTR psz, _In_ UINT cch);
f_propVariantToString propVariantToString = 0;

template<class T>
_Check_return_ __inline HRESULT Wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ T** factory) {
	return roGetActivationFactory(activatableClassId, IID_INS_ARGS(factory));
}

template<typename T>
inline HRESULT wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _Inout_ Details::ComPtrRef<T> factory) throw() {
	return Wrap_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
}

template <typename TFunction>
inline HRESULT tryLoadFunction(HINSTANCE dll, LPCSTR name, TFunction &func) {
	if (!dll) return false;

	func = (TFunction)GetProcAddress(dll, name);
	return (!!func) ? S_OK : E_FAIL;
}



static const int RequiredStaticLibrariesCount = 4;
static const std::wstring RequiredStaticLibraries[RequiredStaticLibrariesCount] = { L"PROPSYS.DLL", L"api-ms-win-core-winrt-string-l1-1-0.dll", L"SHELL32.DLL", L"COMBASE.DLL" };
extern WINTOASTLIB_API inline HRESULT isRequiredLibrariesAvailables() {
	for (int i = 0; i < RequiredStaticLibrariesCount; i++) {
		if (LoadLibrary(RequiredStaticLibraries[i].c_str()) == NULL)
			return E_FAIL;
	}
	return S_OK;
}


extern WINTOASTLIB_API inline HRESULT setupRequiredLibraries() {
	HINSTANCE procId = LoadLibrary(RequiredStaticLibraries[0].c_str());
	if (SUCCEEDED(tryLoadFunction(procId, "PropVariantToString", propVariantToString))) {
		procId = LoadLibrary(RequiredStaticLibraries[1].c_str());
		if (SUCCEEDED(tryLoadFunction(procId, "WindowsCreateStringReference", windowsCreateStringReference))
			&& SUCCEEDED(tryLoadFunction(procId, "WindowsDeleteString", windowsDeleteString))) {
			procId = LoadLibrary(RequiredStaticLibraries[2].c_str());
			if (SUCCEEDED(tryLoadFunction(procId, "SetCurrentProcessExplicitAppUserModelID", setCurrentProcessExplicitAppUserModelID))) {
				procId = LoadLibrary(RequiredStaticLibraries[3].c_str());
				if (SUCCEEDED(tryLoadFunction(procId, "RoGetActivationFactory", roGetActivationFactory)))
					return false;
			}
		}
	}
	return E_FAIL;
}