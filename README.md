![releases](https://img.shields.io/github/tag/mohabouje/WinToast.svg)
![issues](https://img.shields.io/github/issues/mohabouje/WinToast.svg)
![license](https://img.shields.io/github/license/mohabouje/WinToast.svg)
![built](https://img.shields.io/badge/built%20with-MVSC-6f62ba.svg)

[![GitHub forks](https://img.shields.io/github/forks/mohabouje/WinToast.svg?style=social&label=Fork)]()
[![GitHub stars](https://img.shields.io/github/stars/mohabouje/WinToast.svg?style=social&label=Star)]()
[![GitHub watchers](https://img.shields.io/github/watchers/mohabouje/WinToast.svg?style=social&label=Watch)]()

***

WinToast
===================

WinToast is a lightly library written in C++ which brings a complete integration of the modern **toast notifications** of **Windows 8** &  **Windows 10**. 

Toast notifications allows your app to inform the users about relevant information and timely events that they should see and take action upon inside your app, such as a new instant message, a new friend request, breaking news, or a calendar event. 

1. [Toast Templates](#id1)
2. [Event Handler](#id2)
3. [Expiration Time](#id3)
4. [Modern Features](#id4)
5. [Example of usage](#id5)

<div id='id1' />

## Toast Templates

WinToast integrates all standard templates availables in the [ToastTemplateType enumeration](https://msdn.microsoft.com/en-us/library/windows/apps/br208660.aspx).

| Template     | Description | Example   |
| :------- | ----: | :---: |
| ImageAndText01 | A large image and a single string wrapped across three lines of text. |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601606.png)   |
| ImageAndText02   | A large image, one string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![12](https://i-msdn.sec.s-msft.com/dynimg/IC601607.png)   |
| ImageAndText03 | A large image, one string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601608.png) |
| ImageAndText04 |    A large image, one string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601609.png)  |
| Text01 | Single string wrapped across three lines of text. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601602.png)|
| Text02   | One string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601603.png) |
| Text03 | One string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601604.png)|
| Text04 |   One string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601605.png) |

Example of a `ImageAndText02` template:

```cpp
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
templ.setImagePath(L"C:/example.png");
templ.setTextField(L"title", WinToastTemplate::FirstLine);
templ.setTextField(L"subtitle", WinToastTemplate::SecondLine);
 ```  
<div id='id2' />

## Event Handler

WinToast handle different events:

 - **Activated**: Occurs when user activates a toast notification through a click or touch. Apps that are running subscribe to this event
 - **Dismissed**: Occurs when a toast notification leaves the screen, either by expiring or being explicitly dismissed by the user. 
	* Application Hidden:  The application hid the toast using ToastNotifier.hide.
	* User Canceled: The user dismissed the toast.
	* Timed Out: The toast has expired
 - **Failed**: Occurs when an error is caused when Windows attempts to raise a toast notification.

Create your custom handler to interact with the user actions by subclassing the interface `IWinToastHandler`:

```cpp
class WinToastHandlerExample : public IWinToastHandler {
 public:
	WinToastHandlerExample(); 
	// Public interfaces
	void toastActivated() const;
	void toastDismissed(WinToastDismissalReason state) const;
	void toastFailed() const;
 };
 ``` 
 <div id='id3' />
 
 ## Expiration Time
 
Set the time after which a toast notification is no longer considered current or valid and should not be displayed. Windows attemps to raise toast notifications immediately after you call Show, so this property is rarely used. 
 
> For Windows 8.x app, this property also causes the toast notification to be removed from the
> Action Center once the specified data and time is reached.

 <div id='id4' />
 
 ## Modern features - Windows 10

If your system support the new modern features (Version > Windows 8.1) available in Windows 10,  you can add some interesting fields as:

 - **Actions**: you can add your own actions, this fact allow you to interact with user in a different way:

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

!["Toast with some actions"](https://lh3.googleusercontent.com/uJE_H0aBisOZ-9GynEWgA7Hha8tHEI-i0aHrFuOFDBsPSD-IJ-qEN0Y7XY4VI5hp_5MQ9xjWbFcm)
 - **Attribution text**: you can add/remove the attribution text, by default is empty.  Use `WinToastTemplate::setAttributionText` to modify it.
 - **Audio Properties**: you can modify the different behaviors of the sound:
	 - *Default*: plays the audio file just one time.
	 - *Silent*: turn off the sound.
	 - *Loop*: plays the given sound in a loop during the toast existence.

> WinToast allows the modification of the default audio file. Add 
> the given file in to your projects resources (*must be ms-appx:// or
> ms-appdata:// path*) and define it by calling: `WinToastTemplate::setAudioPath`

***By default, WinToast checks if your systems support the features, ignoring the not supported ones.***
 
<div id='id5' />

## Example of Usage

*For an easy usage,  you can just use the available singleton instance.* 

First step, Import the header file wintoastlib.h to your project. You should check if your Windows Version is supported by the library.

```cpp
using namespace WinToastLib;
....
if (!WinToast::isCompatible()) {
    std::wcout << L"Error, your system in not supported!" << std::endl;
}
```

 Configure your [App User Model Id](https://msdn.microsoft.com/en-us/library/windows/desktop/dd378459%28v=vs.85%29.aspx), this can be done by using the existing helper:

```cpp        
using namespace WinToastLib;
....
WinToast::instance()->setAppName(L"WinToastExample");
const auto aumi = WinToast::configureAUMI(L"mohabouje", L"wintoast", L"wintoastexample", L"20161006");
WinToast::instance()->setAppUserModelId(aumi);	
```	
Initialize all the dependencies and check if WinToast has been initialized successfully in your system:

```cpp		
if (!WinToast::instance()->initialize()) {
  std::wcout << L"Error, could not initialize the lib!" << std::endl;
}
```
    
Implement your own action handler by subclassing the interface `IWinToastHandler` and custom your template:

```cpp
WinToastHandlerExample* handler = new WinToastHandlerExample;
WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
templ.setImagePath(L"C:/example.png");
templ.setTextField(L"title", WinToastTemplate::FirstLine);
templ.setTextField(L"subtitle", WinToastTemplate::SecondLine);
 ```   

Finaly show the final results.

```cpp

if (!WinToast::instance()->showToast(templ, handler)) {
    std::wcout << L"Error: Could not launch your toast notification!" << std::endl;
}
 ```   






