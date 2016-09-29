#include "stdafx.h"

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
bool tryLoadFunction(HINSTANCE dll, LPCSTR name, TFunction &func) {
	if (!dll) return false;

	func = (TFunction)GetProcAddress(dll, name);
	return !!func;
}


bool setupPropSysLib() {
	HINSTANCE procId = LoadLibrary(L"PROPSYS.DLL");
	return tryLoadFunction(procId, "PropVariantToString", propVariantToString);
}

bool setupWinRTStringLib() {

	HINSTANCE otherProcId = LoadLibrary(L"api-ms-win-core-winrt-string-l1-1-0.dll");
	if (!tryLoadFunction(otherProcId, "WindowsCreateStringReference", windowsCreateStringReference))
		return false;
	if (!tryLoadFunction(otherProcId, "WindowsDeleteString", windowsDeleteString))
		return false;
	return true;
}

bool setupCombaseLib() {
	if (!setCurrentProcessExplicitAppUserModelID)
		return false;
	if (!propVariantToString)
		return false;
	HINSTANCE procId = LoadLibrary(L"COMBASE.DLL");
	if (!tryLoadFunction(procId, "RoGetActivationFactory", roGetActivationFactory))
		return false;
	return true;
}

bool setupShell32Lib() {
	HINSTANCE procId = LoadLibrary(L"SHELL32.DLL");
	return tryLoadFunction(procId, "SetCurrentProcessExplicitAppUserModelID", setCurrentProcessExplicitAppUserModelID);
}

