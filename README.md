# floui
Single header Objective-C++ lib for SwiftUI like dev on iOS

## Usage
You can downoload the floui.hpp header and add it to your xcode project. Remember to change the extension of your ViewController.m file to .mm (to indicate you're using C++).
```objc

#import "ViewController.h"
#define FLOUI_IMPL
#import "floui.hpp"

@interface ViewController ()

@end

@implementation ViewController
Button dec_btn(@"Decrement"); // since we want to use a C++ lambda
int val;

- (void)viewDidLoad {
    [super viewDidLoad];
    MainView(self, {
        Text(@"Counter")
            .center()
            .foreground(UIColor.whiteColor)
            .font([UIFont boldSystemFontOfSize:30])
            .background(UIColor.purpleColor),
        Spacer(),
        VStack({
            Button(@"Increment")
                .action(self, @selector(increment))
                .config([UIButtonConfiguration filledButtonConfiguration])
                .foreground(UIColor.whiteColor),
            Text(@"0").id("mytext"),
            dec_btn
                .foreground(UIColor.whiteColor)
                .config([UIButtonConfiguration filledButtonConfiguration])
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
