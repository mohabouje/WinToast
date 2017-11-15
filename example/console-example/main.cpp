#include "wintoastlib.h"

using namespace WinToastLib;

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this toast" << std::endl;
        exit(0);
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
        exit(16 + actionIndex);
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"The user dismissed this toast" << std::endl;
            exit(1);
            break;
        case TimedOut:
            std::wcout << L"The toast has timed out" << std::endl;
            exit(2);
            break;
        case ApplicationHidden:
            std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
            exit(3);
            break;
        default:
            std::wcout << L"Toast not activated" << std::endl;
            exit(4);
            break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
        exit(5);
    }
};

int wmain(int argc, LPWSTR *argv)
{
    if (!WinToast::isCompatible()) {
        std::wcerr << L"Error, your system in not supported!" << std::endl;
        return 6;
    }

    LPWSTR appName = L"Console WinToast Example", appUserModelID = L"WinToast Console Example", text = L"Hello, world!", imagePath = NULL;
    std::vector<std::wstring> actions;
    INT64 expiration = 0;

    int i;
    for (i = 1; i < argc; i++)
        if (!wcscmp(L"-image", argv[i]))
            imagePath = argv[++i];
        else if (!wcscmp(L"-action", argv[i]))
            actions.push_back(argv[++i]);
        else if (!wcscmp(L"-expire-ms", argv[i]))
            expiration = wcstol(argv[++i], NULL, 10);
        else if (!wcscmp(L"-app-name", argv[i]))
            appName = argv[++i];
        else if (!wcscmp(L"-app-user-model-id", argv[i]) || !wcscmp(L"-app-id", argv[i]))
            appUserModelID = argv[++i];
        else if (argv[i][0] == L'-') {
            std::wcout << L"Unhandled option: " << argv[i] << std::endl;
            return 7;
        }
        else if (i + 1 == argc)
            text = argv[i];
        else {
            std::wcerr << L"Cannot handle multiple texts for now" << std::endl;
            return 8;
        }

    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);
    bool wasLinkCreated = false;
    if (!WinToast::instance()->initialize(&wasLinkCreated)) {
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        return 9;
    }

    if (wasLinkCreated) {
        WCHAR   exePath[MAX_PATH]{L'\0'};
        DWORD written = GetModuleFileNameExW(GetCurrentProcess(), nullptr, exePath, MAX_PATH);
        STARTUPINFOW si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        memset(&pi, 0, sizeof(pi));
        Sleep(3000);
        BOOL b = CreateProcessW(exePath, GetCommandLineW(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if (b) {
            DWORD code = 1;
            WaitForSingleObject(pi.hProcess, INFINITE);
            if (!GetExitCodeProcess(pi.hProcess, &code))
                std::wcerr << "Could not get exit code of child process!" << std::endl;
            CloseHandle(pi.hProcess);
            exit(code);
        }
    }

	WinToastTemplate templ((imagePath == NULL) ? WinToastTemplate::ImageAndText01 : WinToastTemplate::Text01);
    templ.setImagePath(imagePath);
	templ.setTextField(text, WinToastTemplate::FirstLine);
    
	for (auto const &action : actions)
        templ.addAction(action);
    if (expiration)
        templ.setExpiration(expiration);

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        std::wcerr << L"Could not launch your toast notification!";
        return 10;
    }

    // Give the handler a chance for 15 seconds (or the expiration plus 1 second)
    Sleep(expiration ? expiration + 1000 : 15000);

    exit(2);
}
