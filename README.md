WinToast
===================

WinToast is a lightly library written in C++ which brings a complete integration of the modern **toast notifications** of **Windows 8** &  **Windows 10**. 

Toast notifications allows your app to inform the users about relevant information and timely events that they should see and take action upon inside your app, such as a new instant message, a new friend request, breaking news, or a calendar event. 

WinToast integrates all standar templates availables in the [ToastTemplateType enumeration](https://msdn.microsoft.com/en-us/library/windows/apps/br208660.aspx).

| Template     | Descriptin | Example   |
| :------- | ----: | :---: |
| ImageAndText01 | A large image and a single string wrapped across three lines of text. |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601606.png)   |
| ImageAndText02   | A large image, one string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![12](https://i-msdn.sec.s-msft.com/dynimg/IC601607.png)   |
| ImageAndText03 | A large image, one string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601608.png) |
| ImageAndText04 |    A large image, one string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601609.png)  |
| Text01 | Single string wrapped across three lines of text. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601602.png)|
| Text02   | One string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601603.png) |
| Text03 | One string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601604.png)|
| Text04 |   One string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601605.png) |


### Usage

Import the header file wintoastlib.h to your project. Initialize the library with your application info:
        
    using namespace WinToastLib;
    ....
    WinToast::instance()->setAppName(L"WinToastExample");
    WinToast::instance()->setAppUserModelId(
                WinToast::configureAUMI(L"mohabouje", L"wintoast", L"wintoastexample", L"20161006"));
    if (!WinToast::instance()->initialize()) {
        qDebug() << "Error, your system in not compatible!";
    }
    
You could customize your own handler, just create a subclass from `WinToastHandler`:

	class WinToastHandlerExample : public WinToastHandler {
	 public:
		WinToastHandlerExample(); 
		// Public interfaces
		void toastActivated() const;
		void toastDismissed(WinToastDismissalReason state) const;
		void toastFailed() const;
	 };
         
Now, every time you want to launch a new toast, just create a new template and configure it:

	WinToastHandlerExample* handler = new WinToastHandlerExample;
	WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageWithTwoLines);
	templ.setImagePath(L"C:\example.png");
	templ.setTextField(L"title",0);
	templ.setTextField(L"subtitle", 1);

	if (!WinToast::instance()->showToast(templ, handler)) {
	    std::wcout << L"Error: Could not launch your toast notification!";
	}
    
**That's all my folks =)**




