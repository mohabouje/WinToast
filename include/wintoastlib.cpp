#include "wintoastlib.h"
using namespace WinToastLib;

// Function load a function from library
template <typename Function>
HRESULT loadFunctionFromLibrary(HINSTANCE library, LPCSTR name, Function &func) {
    if (!library) return false;

    func = reinterpret_cast<Function>(GetProcAddress(library, name));
    return (func != nullptr) ? S_OK : E_FAIL;
}

WinToast::WinToast()
{

}

HRESULT DllImporter::initialize() {
    HINSTANCE LibShell32 = LoadLibrary(L"SHELL32.DLL");
    HRESULT hr = loadFunctionFromLibrary(LibShell32, "SetCurrentProcessExplicitAppUserModelID", SetCurrentProcessExplicitAppUserModelID);
    if (SUCCEEDED(hr)) {
        HINSTANCE LibPropSys = LoadLibrary(L"PROPSYS.DLL");
        hr = loadFunctionFromLibrary(LibPropSys, "PropVariantToString", PropVariantToString);
        if (SUCCEEDED(hr)) {
            HINSTANCE LibComBase = LoadLibrary(L"COMBASE.DLL");
            return SUCCEEDED(loadFunctionFromLibrary(LibComBase, "RoGetActivationFactory", RoGetActivationFactory))
                    && SUCCEEDED(loadFunctionFromLibrary(LibComBase, "WindowsCreateStringReference", WindowsCreateStringReference))
                    && SUCCEEDED(loadFunctionFromLibrary(LibComBase, "WindowsDeleteString", WindowsDeleteString));
        }
    }
}
