#pragma once
#define WIN32_LEAN_AND_MEAN            
// Windows Header Files:
#include <windows.h>
#include <SDKDDKVer.h>

#include <Shobjidl.h>
#include <wrl/implements.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <winstring.h>
#include <string.h>

typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ::IInspectable *>												ToastActivatedEventHandler;
typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ABI::Windows::UI::Notifications::ToastDismissedEventArgs *>		ToastDismissedEventHandler;
typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ABI::Windows::UI::Notifications::ToastFailedEventArgs *>			ToastFailedEventHandler;

typedef  Microsoft::WRL::Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler>														WinToastHandler;

using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;

#pragma comment(lib,"shlwapi")
