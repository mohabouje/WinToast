# WinToast

WinToast is a lightly library written in C++ which brings a complete integration of the modern **toast notifications** of **Windows 8** &  **Windows 10**. 

Toast notifications allows your app to inform the users about relevant information and timely events that they should see and take action upon inside your app, such as a new instant message, a new friend request, breaking news, or a calendar event. 

## Event Handler

WinToast implements a common interface `IWinToastHandler` to handle events:

 - **Activated**: Occurs when user activates a toast notification through a click or touch. Apps that are running subscribe to this event
 - **Dismissed**: Occurs when a toast notification leaves the screen, either by expiring or being explicitly dismissed by the user. 
	* Application Hidden:  The application hid the toast using ToastNotifier.hide.
	* User Canceled: The user dismissed the toast.
	* Timed Out: The toast has expired
 - **Failed**: Occurs when an error is caused when Windows attempts to raise a toast notification.

Users can creates their own custom handler to interact with the user actions. For example:

```cpp
class WinToastHandlerExample : public IWinToastHandler {
 public:
	WinToastHandlerExample(); 
	// Public interfaces
	void toastActivated() const override;
	void toastDismissed(WinToastDismissalReason state) const override;
	void toastFailed() const override;
 };

```
## Error Handling

There are several reasons WinToast can fail that's why the library notifies the caller about any possible failure reason. Those are the code for each failure:

| WinToastError           | Error Code | Error message                                                |
| ----------------------- | ---------- | ------------------------------------------------------------ |
| `NoError`               | 0x00       | No error. The process was executed correctly                 |
| `NotInitialized`        | 0x01       | The library has not been initialized                         |
| `SystemNotSupported`    | 0x02       | The OS does not support WinToast                             |
| `ShellLinkNotCreated`   | 0x03       | The library was not able to create a Shell Link for the app  |
| `InvalidAppUserModelID` | 0x04       | The AUMI is not a valid one                                  |
| `InvalidParameters`     | 0x05       | The parameters used to configure the library are not valid normally because an invalid AUMI or App Name |
| `NotDisplayed`          | 0x06       | The toast was created correctly but WinToast was not able to display the toast |
| `UnknownError`          | 0x07       | Unknown error                                                |

A common example of usage is to check while initializing the library or showing a toast notification the possible failure code:

```cpp
WinToast::WinToastError error;
const bool succedded = WinToast::instance()->initialize(&error);
if (!succedded) {  
    std::wcout << L"Error, could not initialize the lib. Error: " 
    << WinToast::strerror(error) << std::endl;
}
```

## Properties

### Templates

WinToast integrates all standard templates available in the [ToastTemplateType enumeration](https://msdn.microsoft.com/en-us/library/windows/apps/br208660.aspx).



| Template         |                                                  Description |                           Example                            |
| :--------------- | -----------------------------------------------------------: | :----------------------------------------------------------: |
| `ImageAndText01` | A large image and a single string wrapped across three lines of text. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601606.png) |
| `ImageAndText02` | A large image, one string of bold text on the first line, one string of regular text wrapped across the second and third lines. |   ![12](https://i-msdn.sec.s-msft.com/dynimg/IC601607.png)   |
| `ImageAndText03` | A large image, one string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601608.png) |
| `ImageAndText04` | A large image, one string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601609.png) |
| `Text01`         |            Single string wrapped across three lines of text. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601602.png) |
| `Text02`         | One string of bold text on the first line, one string of regular text wrapped across the second and third lines. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601603.png) |
| `Text03`         | One string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601604.png) |
| `Text04`         | One string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601605.png) |

Example of a `ImageAndText02` template:

```cpp
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
templ.setTextField(L"title", WinToastTemplate::FirstLine);
templ.setTextField(L"subtitle", WinToastTemplate::SecondLine);
templ.setImagePath(L"C:/example.png"); 
```

**Note:** The user can use the default system sound or specify a sound to play when a toast notification is displayed. Same behavior for the toast notification image, by default Windows try to use the app icon.

### Expiration Time

This property sets the time after which a toast notification is no longer considered current or valid and should not be displayed. Windows attempts to raise toast notifications immediately after you call Show, so this property is rarely used. 

> For Windows 8.x app, this property also causes the toast notification to be removed from the
> Action Center once the specified data and time is reached.

> Not: Default Windows behavior is to hide notification automatically after time set in Windows Ease of Access Settings.

If you need to preserve notification in Windows Action Center for longer period of time, you have to call `WinToastTemplate::setExpiration` method. 

### Actions 

WinToast provides an easy interface to add actions (buttons) to a toast notification. This feature allows the interaction with user in a different way:

```cpp
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
templ.setTextField(L"Do you think this feature is cool?", WinToastTemplate::FirstLine);
templ.setTextField(L"Ofc,it is!", WinToastTemplate::SecondLine);

std::vector<std::wstring> actions;
actions.push_back(L"Yes");
actions.push_back(L"No");
for (auto const &action : actions)	
    templ.addAction(action);
WinToast::instance()->showToast(templ, handler)
```

This code will display something like this:

!["Toast with some actions"](https://lh3.googleusercontent.com/uJE_H0aBisOZ-9GynEWgA7Hha8tHEI-i0aHrFuOFDBsPSD-IJ-qEN0Y7XY4VI5hp_5MQ9xjWbFcm)

### Atribution Text

In the latest versions of Windows, users can add/remove an attribution text (empty by default).  WinToast integrates a simple interface to change this property `WinToastTemplate::setAttributionText`:

```c++
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
templ.setTextField(L"Do you think this feature is cool?", WinToastTemplate::FirstLine);
templ.setAttributionText(L"This is an attribution text");
WinToast::instance()->showToast(templ, handler)
```

### Duration

Users can change the amount of time the toast should be displayed. This attribute can have one of the following values: 

   - *System*: default system configuration.
 - *Short*: default system short time configuration.
 - *Long*: default system long time configuration.

```c++
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
templ.setFirstLine(L"I will be displayed for a long time");
templ.setDuration(WinToastTemplate::Duration::Long);
WinToast::instance()->showToast(templ, handler)
```

### Audio

Users can modify the different behaviors of the sound. For instance, users can specify the default play mode:

- *Default*: plays the audio file just one time.
 - *Silent*: turn off the sound.
 - *Loop*: plays the given sound in a loop during the toast existence.

> WinToast allows the modification of the default audio file. There are different audio files installed by default in the system that can be used via the `WinToastTemplate::AudioSystemFile` enumeration. See more details in this [link](https://docs.microsoft.com/en-us/uwp/schemas/tiles/toastschema/element-audio).
>
> For instance, to display an alarm that will play the same sound in a loop we could do something like this:
>
> ```c++
> WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
> templ.setFirstLine(L"I am an alarm");
> templ.setDuration(WinToastTemplate::Duration::Long);
> templ.setAudioMode(WinToastTemplate::AudioOption::Loop);
> templ.setAudioPath(WinToastTemplate::AudioSystemFile::Alarm);
> WinToast::instance()->showToast(templ, handler)
> ```

## Toast configuration on Windows 10

Windows allows the configuration of the default behavior of a toast notification. This can be done in the *Ease of Access* configuration by modifying the *Other options* tab. 

The system configuration helps you to define how long you want notifications to appear for (5 seconds to 5 minutes) as turning on visual notifications for sound.

![Ease of Access configuration](https://camo.githubusercontent.com/56c8edd1a7a4a43be07ba211d9d828478fdbad39/68747470733a2f2f7777772e686f77746f6765656b2e636f6d2f77702d636f6e74656e742f75706c6f6164732f323031362f30332f656173655f6f665f6163636573732e706e67)

<div id='id8' />

## Projects using WinToast
 - [Git for Windows](https://github.com/git-for-windows/git): A fork of Git containing Windows-specific patches.
 - [QGIS](https://github.com/qgis/QGIS): QGIS is a free, open source, cross platform (lin/win/mac) geographical information system (GIS)
 - [nheko](https://github.com/mujx/nheko): Desktop client for the Matrix protocol.
 - [EDPathFinder](https://github.com/neotron/EDPathFinder): A program that creates an optimal route that passes through two or more systems in Elite.
 - [AntiExploit](https://github.com/Empier/Anti-Exploit): antiexploit utility for Windows.
 - [Zroya](https://github.com/malja/zroya): Python extension for creating native Windows notifications..
 - [PidginWinToastNotifications](https://github.com/ChristianGalla/PidginWinToastNotifications): Windows Toast Notification Plugin for Pidgin. 
 - [Dnai-Editor](https://github.com/Nicolas-Constanty/Dnai.Editor): Visual Scripting, node editor.
 - [Spectral](https://gitlab.com/b0/spectral): A glossy client for Matrix, written in QtQuick Controls 2 and C++.
