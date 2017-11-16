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


enum Results {
	ToastClicked,					// user clicked on the toast
	ToastDismissed,					// user dismissed the toast
	ToastTimeOut,					// toast timed out
	ToastHided,						// application hid the toast
	ToastNotActivated,				// toast was not activated
	ToastFailed,					// toast failed
	SystemNotSupported,				// system does not support toasts
	UnhandledOption,				// unhandled option
	MultipleTextNotSupported,		// multiple texts were provided
	InitializationFailure,			// toast notification manager initialization failure
	ToastNotLaunched				// toast could not be launched
};

void print_help() {
	std::cout << "--action" << "Set the actions in buttons" << std::endl;
	std::cout << "--aumi" << "Set the App User Model Id" << std::endl;
	std::cout << "--appname" << "Set the default appname" << std::endl;
	std::cout << "--appid" << "Set the App Id" << std::endl;
	std::cout << "--expirems" << "Set the default expiration time" << std::endl;
	std::cout << "--text" << "Set the text for the notifications" << std::endl;
	std::cout << "--help" << "Print the help description" << std::endl;
}


int wmain(int argc, LPWSTR *argv)
{
	if (argc == 1) {
		print_help();
		return 0;
	}

    if (WinToast::isCompatible()) {
        std::wcerr << L"Error, your system in not supported!" << std::endl;
        return Results::SystemNotSupported;
    }

    LPWSTR appName = L"Console WinToast Example", appUserModelID = L"WinToast Console Example", text = L"Hello, world!", imagePath = NULL;
    std::vector<std::wstring> actions;
    INT64 expiration = 0;

    int i;
	for (i = 1; i < argc; i++)
        if (!wcscmp(L"--image", argv[i]))
            imagePath = argv[++i];
        else if (!wcscmp(L"--action", argv[i]))
            actions.push_back(argv[++i]);
        else if (!wcscmp(L"--expirems", argv[i]))
            expiration = wcstol(argv[++i], NULL, 10);
        else if (!wcscmp(L"--appname", argv[i]))
            appName = argv[++i];
        else if (!wcscmp(L"--aumi", argv[i]) || !wcscmp(L"--appid", argv[i]))
            appUserModelID = argv[++i];
		else if (!wcscmp(L"--text", argv[i]))
			text = argv[++i];
        else if (!wcscmp(L"--help", argv[i])) {
			print_help();
			return 0;
		} else {
            std::wcerr << L"Cannot handle multiple texts for now" << std::endl;
			return Results::MultipleTextNotSupported;
        }

    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);
    bool wasLinkCreated = false;
    if (!WinToast::instance()->initialize(&wasLinkCreated)) {
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        return Results::InitializationFailure;
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

	WinToastTemplate templ((imagePath != NULL) ? WinToastTemplate::ImageAndText01 : WinToastTemplate::Text01);
    templ.setImagePath(imagePath);
	templ.setTextField(text, WinToastTemplate::FirstLine);
    
	for (auto const &action : actions)
        templ.addAction(action);
    if (expiration)
        templ.setExpiration(expiration);

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        std::wcerr << L"Could not launch your toast notification!";
		return Results::ToastFailed;
    }

    // Give the handler a chance for 15 seconds (or the expiration plus 1 second)
    Sleep(expiration ? expiration + 1000 : 15000);

    exit(2);
}
