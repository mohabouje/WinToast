#include <Windows.h>
#include <Shobjidl.h>
#include <shellapi.h>

#include <roapi.h>
#include <wrl/client.h>
#include <wrl/implements.h>
#include <windows.ui.notifications.h>


#include <Shlwapi.h>
#include <strsafe.h>
#include <windowsx.h>
#include <WtsApi32.h>

#include <SDKDDKVer.h>

#include <sal.h>
#include <Psapi.h>
#include <strsafe.h>
#include <propvarutil.h>
#include <ObjBase.h>
#include <propvarutil.h>
#include <functiondiscoverykeys.h>
#include <intsafe.h>
#include <guiddef.h>
#include <sstream>
#include <iostream>
#include <winstring.h>

// Required library: available from Windows 2000. No problem with Windows XP & 7
#pragma comment(lib,"shlwapi")

typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ::IInspectable *>												ToastActivatedEventHandler;
typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ABI::Windows::UI::Notifications::ToastDismissedEventArgs *>		ToastDismissedEventHandler;
typedef  ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification*, ABI::Windows::UI::Notifications::ToastFailedEventArgs *>			ToastFailedEventHandler;


using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;

