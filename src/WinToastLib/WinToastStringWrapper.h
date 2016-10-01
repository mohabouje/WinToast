#pragma once
#include "WinToastDllImporter.h"
#include <intsafe.h>
class WinToastStringWrapper {
public:
	WinToastStringWrapper(_In_reads_(length) PCWSTR stringRef, _In_ UINT32 length) throw() {
		HRESULT hr = WinToastDllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
		if (!SUCCEEDED(hr)) {
			RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
		}
	}

	WinToastStringWrapper(_In_ const std::wstring &stringRef) throw() 
	{
		HRESULT hr = WinToastDllImporter::WindowsCreateStringReference(stringRef.c_str(), static_cast<UINT32>(stringRef.length()), &_header, &_hstring);

		if (FAILED(hr)) {
			RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
		}
	}
	~WinToastStringWrapper() {
		WinToastDllImporter::WindowsDeleteString(_hstring);
	}


	template <size_t N>
	WinToastStringWrapper(_In_reads_(N) wchar_t const (&stringRef)[N]) throw() {
		UINT32 length = N - 1;
		HRESULT hr = WinToastDllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
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

		WinToastDllImporter::WindowsCreateStringReference(stringRef, length, &_header, &_hstring);
	}

	HSTRING Get() const throw() {
		return _hstring;
	}

private:
	HSTRING _hstring;
	HSTRING_HEADER _header;

};
