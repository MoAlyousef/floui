# floui
Single header Objective-C++ lib inspired by SwiftUI

## Usage
You can downoload the floui.hpp header and add it to your xcode project. Remember to change the extension of your ViewController.m file to .mm (to indicate you're using C++).
```objc
// ViewController.mm

#import "ViewController.h"
#define FLOUI_IMPL
#include "floui.hpp"

#if TARGET_OS_IPHONE
#define FONT UIFont
#define COLOR UIColor
#else
#define FONT NSFont
#define COLOR NSColor
#endif

@implementation ViewController
Button dec_btn(@"Decrement"); // since we want to use a C++ lambda
int val;

- (void)viewDidLoad {
    [super viewDidLoad];
#if TARGET_OS_OSX
    self.view.frame = NSMakeRect(0, 0, 600, 400);
#endif
    MainView(self, {
        Text(@"Counter")
            .center()
            .foreground(COLOR.whiteColor)
            .font([FONT boldSystemFontOfSize:30])
            .background(COLOR.purpleColor),
        Spacer(),
        VStack({
            Button(@"Increment")
                .action(self, @selector(increment))
                .filled()
                .background(COLOR.blueColor)
                .foreground(COLOR.whiteColor),
            Text(@"0").id("mytext"),
            dec_btn
                .foreground(COLOR.whiteColor)
                .filled()
                .background(COLOR.blueColor)
                .action([=] { // modern C++ lambda syntax
                val--;
                Widget::from_id<Text>("mytext").text([NSString stringWithFormat:@"%d", val]);
            }),
        }),
        Spacer(),
        Spacer(),
    });
}
-(void)increment {
    val++;
    Widget::from_id<Text>("mytext").text([NSString stringWithFormat:@"%d", val]);
}

@end
```
Add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

![image](https://user-images.githubusercontent.com/37966791/173707028-a6e076c2-4170-459e-88a7-bd555ecfd1fa.png)

![image](https://user-images.githubusercontent.com/37966791/173926274-4ea69936-708d-4b24-92e3-48e40bde8ce5.png)

## Todo
- Wrap more UIKit and AppKit controls.
