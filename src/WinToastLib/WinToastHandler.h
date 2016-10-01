#pragma once
class  WinToastHandler : public Microsoft::WRL::Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler>{
public:
	WinToastHandler(_In_ HWND hToActivate, _In_ HWND hEdit);
	WinToastHandler() {}
	~WinToastHandler();
	virtual IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IInspectable *inspectable);
	virtual IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastDismissedEventArgs *e);
	virtual IFACEMETHODIMP Invoke(_In_ IToastNotification *toast, _In_ IToastFailedEventArgs *e);
	virtual IFACEMETHODIMP_(ULONG) AddRef();
	virtual IFACEMETHODIMP_(ULONG) Release();
	virtual IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv);

protected:
	ULONG _ref;
	HWND _hToActivate;
	HWND _hEdit;
};

