#include "wintoastlib.h"
#include <string>
#include <windows.h>

using namespace WinToastLib;


/* Explaine
    This used to Handle events that occure on users behaves 
*/
class CustomHandler : public IWinToastHandler {
public:

    // When user clicks the notification
    void toastActivated() const {
        std::wcout << L"The user clicked in this toast" << std::endl;
        exit(0);
    }

    // When user clicks the button in notiification
    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
        exit(16 + actionIndex);
    }

    // When user closes the notification
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

    // When error occure
    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
        exit(5);
    }
};

// ######################################################################################


/* Explaine
    Here we return code foreach case, to can do any handiling in the future based on the returning code
*/
enum Results {
    ToastClicked,             // user clicked on the toast
    ToastDismissed,           // user dismissed the toast
    ToastTimeOut,             // toast timed out
    ToastHided,               // application hid the toast
    ToastNotActivated,        // toast was not activated
    ToastFailed,              // toast failed
    SystemNotSupported,       // system does not support toasts
    UnhandledOption,          // unhandled option
    MultipleTextNotSupported, // multiple texts were provided
    InitializationFailure,    // toast notification manager initialization failure
    ToastNotLaunched          // toast could not be launched
};


// ######################################################################################


int basicModel() {
    
    /* Explaine
        Here we put all data to our notification properties
    */
    std::wstring appName = L"This is Model 1";
    std::wstring appUserModelID = L"Example 1";
    std::wstring text1 = L"This is a Test notification";
    std::wstring text2 = L"Hellow forom C++ I'm the potato who ate the cotato";
    std::wstring text3 = L"Meow";
    std::wstring imagePath = L"C:/Users/pc/Downloads/New FCI Hashira/FCIH/FCAIHHPNgLogo.png";
    std::wstring bannerPath = L"C:/Users/pc/Downloads/New FCI Hashira/Registeration opens (4).png";
    std::wstring audioPath = L"C:/Users/pc/Downloads/videoplayback.m4a";
    std::wstring attribute = L"Via SMS";
    std::vector<std::wstring> actions;
    INT64 expiration = 70000;

    actions.push_back(L"Cancel");
    actions.push_back(L"Mute");
    actions.push_back(L"Answer");


    // ######################################################################################


    /* Explaine
        Here we Set the notification settings for the instance
    */

    WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Loop; // set how notification audio should behave
    
    /* here we can choose the scenario */
    //WinToastTemplate::Scenario scenarioOption = WinToastTemplate::Scenario::Default; // for calls
    //WinToastTemplate::Scenario scenarioOption = WinToastTemplate::Scenario::Alarm; // for alarms
    //WinToastTemplate::Scenario scenarioOption = WinToastTemplate::Scenario::Reminder; // for Reminder
    WinToastTemplate::Scenario scenarioOption = WinToastTemplate::Scenario::IncomingCall;

    /* here we can choose the image crop options */
    //WinToastTemplate::CropHint cropOption = WinToastTemplate::CropHint::Circle; // circle 50%;
    WinToastTemplate::CropHint cropOption = WinToastTemplate::CropHint::Square; // square


    WinToast::instance()->setAppName(appName); // adding the instance name 
    WinToast::instance()->setAppUserModelId(appUserModelID); // adding the instance id 


    if (!WinToast::instance()->initialize()) { // Initialize toast instance, and if error happened return error
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        return Results::InitializationFailure; // if failed will return failure code
    }


    // ######################################################################################


    /* Explaine
        Initialize template from the same instance, this templte is used onece thin killed 
    */

    WinToastTemplate templ( WinToastTemplate::ImageAndText04); // decide our template type
    templ.setTextField(text1, WinToastTemplate::FirstLine);
    templ.setTextField(text2, WinToastTemplate::SecondLine);
    templ.setTextField(text3, WinToastTemplate::ThirdLine);
    

    // audio options are sounds differ based on Scenario
    templ.setAudioOption(audioOption);


    // image
    templ.setImagePath(imagePath); // to set image as icon no crop
    templ.setImagePath(imagePath, cropOption); // to set how image will be cropped

    // banner
    templ.setHeroImagePath(bannerPath); // to set image as banner
    templ.setHeroImagePath(bannerPath, true);
    templ.setAttributionText(attribute);


    templ.setExpiration(expiration);
    templ.setScenario(scenarioOption);

    for (auto const& action : actions) {
        templ.addAction(action);
    }


    // ######################################################################################


    // Try to send the instance, If failed it will return the Failing Code
    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        std::wcerr << L"Could not launch your toast notification!";
        return Results::ToastFailed;
    }

    // Give the handler a chance for 7 seconds and in expiration, then the notifcation is killed
    Sleep((DWORD)expiration + 1500);

    return 2;
}


int model1() {
    return 0;
}
int model2() {
    return 0;
}



int wmain(int argc, LPWSTR* argv) {
    basicModel();    
    exit(2); // same as reutrn 2
}