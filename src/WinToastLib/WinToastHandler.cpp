#include "stdafx.h"
#include "WinToastHandler.h"


WinToastHandler::WinToastHandler(_In_ HWND hToActivate, _In_ HWND hEdit) :
_ref(1),
_hToActivate(hToActivate),
_hEdit(hEdit)
{
}

WinToastHandler::~WinToastHandler()
{
}

IFACEMETHODIMP WinToastHandler::QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv) {
	if (IsEqualIID(riid, IID_IUnknown))
		*ppv = static_cast<IUnknown*>(static_cast<ToastActivatedEventHandler*>(this));
	else if (IsEqualIID(riid, __uuidof(ToastActivatedEventHandler)))
		*ppv = static_cast<ToastActivatedEventHandler*>(this);
	else if (IsEqualIID(riid, __uuidof(ToastActivatedEventHandler)))
		*ppv = static_cast<ToastDismissedEventHandler*>(this);
	else if (IsEqualIID(riid, __uuidof(ToastFailedEventHandler)))
		*ppv = static_cast<ToastFailedEventHandler*>(this);
	else *ppv = nullptr;

	if (*ppv) {
		reinterpret_cast<IUnknown*>(*ppv)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

IFACEMETHODIMP_(ULONG) WinToastHandler::AddRef()
{
	return InterlockedIncrement(&_ref);
}

IFACEMETHODIMP_(ULONG) WinToastHandler::Release()
{
	ULONG l = InterlockedDecrement(&_ref);
	if (l == 0) {
		delete this;
	}
	return l;
}

IFACEMETHODIMP WinToastHandler::Invoke(_In_ IToastNotification *toast, _In_ IToastFailedEventArgs *e) {
	BOOL succeeded = SetForegroundWindow(_hToActivate);
	if (succeeded)
	{
		LRESULT result = SendMessage(_hEdit, WM_SETTEXT, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(L"The user clicked on the toast."));
		succeeded = result ? TRUE : FALSE;
	}
	return succeeded ? S_OK : E_FAIL;
}

IFACEMETHODIMP WinToastHandler::Invoke(_In_ IToastNotification *toast, _In_ IInspectable * instpectable) {
	LRESULT succeeded = SendMessage(_hEdit, WM_SETTEXT, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(L"The toast encountered an error."));
	return S_OK;
}

IFACEMETHODIMP WinToastHandler::Invoke(_In_ IToastNotification *toast, _In_ IToastDismissedEventArgs *e)  {
	ToastDismissalReason tdr;
	HRESULT hr = e->get_Reason(&tdr);
	if (SUCCEEDED(hr))
	{
		wchar_t *outputText;
		switch (tdr)
		{
		case ToastDismissalReason_ApplicationHidden:
			outputText = L"The application hid the toast using ToastNotifier.hide()";
			break;
		case ToastDismissalReason_UserCanceled:
			outputText = L"The user dismissed this toast";
			break;
		case ToastDismissalReason_TimedOut:
			outputText = L"The toast has timed out";
			break;
		default:
			outputText = L"Toast not activated";
			break;
		}

		LRESULT succeeded = SendMessage(_hEdit, WM_SETTEXT, reinterpret_cast<WPARAM>(nullptr), reinterpret_cast<LPARAM>(outputText));
		hr = succeeded ? S_OK : E_FAIL;
	}
	return hr;
}



