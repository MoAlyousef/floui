# floui
floui, pronounced "flowy", is a single header C++17 lib inspired by SwiftUI, which wraps native iOS and Android controls/widgets, and integrates into the de facto build environments of each platform (XCode and Android Studio).

## Usage

### iOS
You can downoload the floui.hpp header and add it to your xcode project. Remember to change the extension of your ViewController.m file to .mm (to indicate you're using C++).
```objc
// ViewController.mm
#import "ViewController.h"
#define FLOUI_IMPL
#include "floui.hh"

@interface ViewController ()

@end

@implementation ViewController
int val {0};

-(void)updateText {
    Widget::from_id<Text>("mytext").text(std::to_string(val));
}

- (void)viewDidLoad {
    [super viewDidLoad];

    FlouiViewController controller((void *)CFBridgingRetain(self));
    
    auto v = MainView(&controller, {
        Text("Counter")
            .size(600, 100)
            .center()
            .foreground(Color::White)
            .fontsize(30)
            .bold()
            .background(Color::Magenta),
        Spacer().size(0, 50),
        VStack({
            Button("Increment")
                .size(0, 40)
                .filled()
                .background(Color::rgb(0, 0, 255))
                .foreground(Color::rgb(255, 255, 255, 255))
                .action([=](Widget&) {
                    val++;
                    [self updateText];
                }),
            Text("0").id("mytext").size(0, 50),
            Button("Decrement").foreground(0xffffffff)
                    .size(0, 40)
                    .filled()
                    .background(0x0000ffff)
                    .action([=](Widget&) {
                        val--;
                        [self updateText];
                    }),
        }),
        Spacer()
    });
}

@end
```
Add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

![image](https://user-images.githubusercontent.com/37966791/173707028-a6e076c2-4170-459e-88a7-bd555ecfd1fa.png)

### Android
Assuming your application is called myapplication:

1- Create a native C++ application in Android studio, check the C++ toolchain to be C++17 (in the last step of creating a project).

2- Add the floui.hh header to the same dir as your cpp source.

2- Replace MainActivity.java with:
```java
package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ConstraintLayout layout = new ConstraintLayout(this);
        setContentView(layout);
        mainView(layout);
    }
    public native View mainView(View view);
    public native void handleEvent(View view);
    public native View findNativeViewById(String id);

    @Override
    public void onClick(View view) {
        handleEvent(view);
    }
}
```

3- In your cpp source file, replace with this boilerplate:
```cpp
#include <jni.h>
#include <string>
#define FLOUI_IMPL
#include "floui.hh"

static int val = 0;

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_mainView(
        JNIEnv* env,
        jobject main_activity, jobject view) {

    FlouiViewController controller(env, main_activity, view);

    auto main_view = MainView(&controller, {
        Button("Increment").action([=](Widget&) {
            val++;
            Widget::from_id<Text>("val").text(std::to_string(val));
        }),
        Text("0").center().bold().fontsize(20).foreground(Color::Black).id("val"),
        Button("Decrement").action([=](Widget&) {
            val--;
            Widget::from_id<Text>("val").text(std::to_string(val));
        })
    });
    return (jobject) main_view.inner();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_handleEvent(JNIEnv *env, jobject thiz, jobject view) {
    FlouiViewController::handle_events(view);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_findNativeViewById(JNIEnv *env, jobject thiz,
                                                               jstring id) {
    return (jobject)Widget::from_id<Widget>(reinterpret_cast<const char *>(id)).inner();
}
```
Only add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

![image](https://user-images.githubusercontent.com/37966791/175548084-a0105440-dc32-4f09-be82-0029312efe7c.png)

## Currently available controls:
- Text
- TextField
- Button
- VStack (Vertical UIStackView on iOS and LinearLayout on Android)
- HStack (Horizontal UIStackView on iOS and LinearLayout on Android)
- Spacer
- Toggle/Check
- Slider

## Todo
- Wrap more UIKit and Android controls and their methods.
- Use proper layout constraints for the mainview to handle orientation changes. 
