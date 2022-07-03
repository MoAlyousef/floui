# floui
floui, pronounced "flowy", is a single header C++17 lib inspired by SwiftUI, which wraps native iOS and Android controls/widgets, and integrates into the de facto build environments of each platform (XCode and Android Studio).

## Usage

### iOS
You can downoload the floui.hpp header and add it to your xcode project. Remember to change the extension of your ViewController.m file to .mm (to indicate you're using C++).
```objc
// ViewController.mm
#import "ViewController.h"
#define FLOUI_IMPL
#include "floui.hpp"

static int val = 0;

MainView myview(const FlouiViewController &fvc) {
    auto main_view = MainView(fvc, {
            Button("Increment")
                .filled()
                .background(Color::rgb(0, 0, 255))
                .foreground(Color::rgb(255, 255, 255, 255))
                .action([=](Widget&) {
                    val++;
                    Widget::from_id<Text>("mytext").text(std::to_string(val));
                }),
            Text("0")
                .id("mytext")
                .size(0, 50),
            Button("Decrement")
                .foreground(0xffffffff)
                .filled()
                .background(0x0000ffff)
                .action([=](Widget&) {
                    val--;
                    Widget::from_id<Text>("mytext").text(std::to_string(val));
                }),
    });
    return main_view;
}

@interface ViewController ()

@end

@implementation ViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    FlouiViewController controller((void *)CFBridgingRetain(self));
    myview(controller);
}

@end
```
Add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

![image](https://user-images.githubusercontent.com/37966791/177045481-201189b8-8f2b-4004-9b32-556cee3fce1a.png)

### Android
Assuming your application is called "My Application" (the default for Android Studio):

1- Create a native C++ application in Android studio, check the C++ toolchain to be C++17 (in the last step of creating a project).

2- Add the floui.hpp header to the same dir as your cpp source.

2- Replace MainActivity.java with:
```java
package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.os.Bundle;
import android.view.View;

import com.google.android.material.slider.Slider;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, Slider.OnChangeListener {
    static {
        System.loadLibrary("myapplication");
    }

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

    @Override
    public void onValueChange(@NonNull Slider slider, float value, boolean fromUser) {
        handleEvent(slider);
    }
}
```
To replace the main application text on the top, you can do so by changing the app_name value in res/values/strings.xml.

3- In your cpp source file, replace with this boilerplate:
(Normally Android Studio will prompt you to implement the missing native methods (mainView, handleEvent and findNativeViewById) so much of it should be automatic)

```cpp
#include <jni.h>
#include <string>
#define FLOUI_IMPL
#include "floui.hpp"

static int val = 0;

MainView myview(const FlouiViewController &fvc) {
    auto main_view = MainView(fvc, {
            Button("Increment")
                .action([=](Widget&) {
                    val++;
                    Widget::from_id<Text>("val").text(std::to_string(val));
                }),
            Text("0")
                .center()
                .bold()
                .fontsize(20)
                .foreground(Color::Black)
                .id("val"),
            Button("Decrement")
                .action([=](Widget&) {
                    val--;
                    Widget::from_id<Text>("val").text(std::to_string(val));
                }),
            Slider()
                .action([](Widget &w) {
                    auto slider_value = Slider(w.inner()).value();
                    floui_log(std::to_string(slider_value)); // to print the values
                })
    });
    return main_view;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_mainView(JNIEnv* env, jobject main_activity, jobject view) {
    FlouiViewController controller(env, main_activity, view);
    return (jobject) myview(controller).inner();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_handleEvent(JNIEnv *env, jobject thiz, jobject view) {
    FlouiViewController::handle_events(view);
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_findNativeViewById(JNIEnv *env, jobject thiz, jstring id) {
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
- ImageView

## Current limitations:
- Use of const std::string& for text values, std::string_view might not be null-terminated. Converting NSString from a c-string requires strings to be null-terminated.
- Sliders on Android take the full width of the LinearLayout, so this must be taken into consideration if code is shared also with iOS.
- Users of this library should ensure correct type usage when acquiring the type from Widget:
```cpp
auto button1 = Widget::from_id<Button>("some_id");
auto button2 = Button(some_widget.inner());
```
Maybe std::any can be used in the library and such casts can pass thru std::any_cast, the problem on Android is that everything is a jobject, and equality can only be checked via JNIenv::IsSameObject, also RTTI is disabled by default on ndk-build. 
- Adding images has to be in the project's resource file. 
    - In Android Studio: Resource Manager, Import Drawables. This will add the file to res/drawable. The file can be accessed directly ImageView("MyImage.jpg").
    - In XCode: You can simple drag images into Assets.xcassets, then the image can be accessed directly ImageView("MyImage.jpg").
- Widgets are not thread-safe.

## Todo
- Wrap more UIKit and Android controls and their methods.
