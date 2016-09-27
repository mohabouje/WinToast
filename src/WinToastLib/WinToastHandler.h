#pragma once

#include "WinToastLib.h"
class WinToastHandler : public Microsoft::WRL::Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler> {
public:
	WinToastHandler(_In_ HWND hToActivate, _In_ HWND hEdit);
	~WinToastHandler();


	IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IInspectable *inspectable);
	IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastDismissedEventArgs *e);
	IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastFailedEventArgs *e);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();
	IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv);


private:
	ULONG _ref;
	HWND _hToActivate;
	HWND _hEdit;
};

