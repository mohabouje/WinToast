#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H
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

using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;


typedef  ITypedEventHandler<ToastNotification*, ::IInspectable *>                                       ToastActivatedEventHandler;
typedef  ITypedEventHandler<ToastNotification*, ToastDismissedEventArgs *>                              ToastDismissedEventHandler;
typedef  ITypedEventHandler<ToastNotification*, ToastFailedEventArgs *>                                 ToastFailedEventHandler;

typedef Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler>     WinToastTemplateHandler;


#pragma comment(lib,"shlwapi")



namespace WinToastLib {

    class WinToast
    {
    public:
        WinToast();
    };

}

#endif // WINTOASTLIB_H
