#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H
#include <windows.h>
#include <SDKDDKVer.h>

#include <Shobjidl.h>
#include <wrl/implements.h>
#include <windows.ui.notifications.h>
#include <strsafe.h>
#include <Psapi.h>
#include <shlobj.h>
#include <roapi.h>
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
    namespace DllImporter {
        // Available in SHELL32.DLL
        typedef HRESULT(FAR STDAPICALLTYPE *f_SetCurrentProcessExplicitAppUserModelID)(__in PCWSTR AppID);
        extern f_SetCurrentProcessExplicitAppUserModelID SetCurrentProcessExplicitAppUserModelID;

        // Available in PROPSYS.DLL
        typedef HRESULT(FAR STDAPICALLTYPE *f_PropVariantToString)(_In_ REFPROPVARIANT propvar, _Out_writes_(cch) PWSTR psz, _In_ UINT cch);
        extern f_PropVariantToString PropVariantToString;

        // Available COMBASE.DLL
        typedef HRESULT(FAR STDAPICALLTYPE *f_RoGetActivationFactory)(_In_ HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory);
        extern f_RoGetActivationFactory RoGetActivationFactory;

        typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsCreateStringReference)(_In_reads_opt_(length + 1) PCWSTR sourceString, UINT32 length, _Out_ HSTRING_HEADER * hstringHeader, _Outptr_result_maybenull_ _Result_nullonfailure_ HSTRING * string);
        extern f_WindowsCreateStringReference WindowsCreateStringReference;

        typedef HRESULT(FAR STDAPICALLTYPE *f_WindowsDeleteString)(_In_opt_ HSTRING string);
        extern f_WindowsDeleteString WindowsDeleteString;


        template<class T>
        _Check_return_ __inline HRESULT _1_GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ T** factory) {
            return RoGetActivationFactory(activatableClassId, IID_INS_ARGS(factory));
        }

        template<typename T>
        inline HRESULT Wrap_GetActivationFactory(_In_ HSTRING activatableClassId, _Inout_ Details::ComPtrRef<T> factory) throw() {
            return _1_GetActivationFactory(activatableClassId, factory.ReleaseAndGetAddressOf());
        }

        inline HRESULT initialize();
    }


    class WinToast
    {
    public:
        WinToast();
    };
}

#endif // WINTOASTLIB_H
