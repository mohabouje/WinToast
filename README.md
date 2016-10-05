WinToast
===================

WinToast is a lightly library written in C++ which brings a complete integration of the modern **toast notifications** of **Windows 8** &  **Windows 10**. 

Toast notifications allows your app to inform the users about relevant information and timely events that they should see and take action upon inside your app, such as a new instant message, a new friend request, breaking news, or a calendar event. 

WinToast integrates all standar templates availables in the [ToastTemplateType enumeration](https://msdn.microsoft.com/en-us/library/windows/apps/br208660.aspx).

| Template     | Descriptin | Example   |
| :------- | ----: | :---: |
| ImageWithOneLine | A large image and a single string wrapped across three lines of text. |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601606.png)   |
| ImageWithTwoLines   | A large image, one string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![12](https://i-msdn.sec.s-msft.com/dynimg/IC601607.png)   |
| ImageWithThreeLines | A large image, one string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601608.png) |
| ImageWithFourLines |    A large image, one string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601609.png)  |
| TextOneLine | Single string wrapped across three lines of text. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601602.png)|
| TextTwoLines   | One string of bold text on the first line, one string of regular text wrapped across the second and third lines.   |  ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601603.png) |
| TextThreeLines | One string of bold text wrapped across the first two lines, one string of regular text on the third line. | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601604.png)|
| TextFourLines |   One string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.     | ![enter image description here](https://i-msdn.sec.s-msft.com/dynimg/IC601605.png) |


### Usage

Import the header file wintoastlib.h to your project from the include folder. Initialize the library with your application info:
        
       using namespace WinToastLib;
       ....
       WinToast::instance()->setAppName(L"WinToastExample");
	    WinToast::instance()->setAppUserModelId(L"WinToastExample_AUMI");
	    if (!WinToast::instance()->initialize()) {
	        std::cout << "Error, probably your system in not compatible!";
	    }
If you want to handle the Toast Notification Events, create your custom `WinToastHandler`:

     class WinToastHandlerExample : public WinToastHandler {
         public:
        	WinToastHandlerExample(); 
		// Public interfaces
		virtual void toastActivated() const;
		virtual void toastDismissed(WinToastDismissalReason state) const;
		virtual void toastFailed() const;
            protected:
            	....
         };
         
And use it in your custom `WinToastTemplate`:

    class MyTemplate : public WinToastTemplate {
		   public:
		    MyTemplate(const WinToastTemplateType& type = ImageWithTwoLines);
		    ~MyTemplate();
		    virtual WinToastHandler* handler() const { return WinToastHandlerExample; };
	  }

Now, every time you want to launch a new toast, just create a new template and use:

    MyTemplate templ = WinToastTemplate(WinToastTemplate::ImageWithTwoLines);
    templ.setImagePath(L"path/example.png");
    templ.setTextField(L"Firs line", 0);
    templ.setTextField(L"Second line", 1);
    if (!WinToast::instance()->showToast(templ)) {
        std::cout << "Could not launch your toast notification =(!");
    }
    
**That's all my folks =)**




