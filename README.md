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
//                .config([UIButtonConfiguration filledButtonConfiguration]) // IOS only
                .foreground(COLOR.whiteColor),
            Text(@"0").id("mytext"),
            dec_btn
                .foreground(COLOR.whiteColor)
//                .config([UIButtonConfiguration filledButtonConfiguration]) // IOS only
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

![image](https://user-images.githubusercontent.com/37966791/173868139-7c715f8f-24d5-41ab-8b5c-b364a9fe3cef.png)

## Todo
- Wrap more UIKit and AppKit controls.
