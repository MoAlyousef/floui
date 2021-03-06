# floui
floui, pronounced "flowy", is a proof-of-concept single header C++17 lib inspired by SwiftUI, which wraps native iOS and Android controls/widgets, and integrates into the de facto build environments of each platform (XCode and Android Studio).

![image](https://user-images.githubusercontent.com/37966791/177225333-50fc1431-3a02-49df-8b87-98ca0245c9bc.png)

## Currently available controls:
- Text
- TextField
- Button
- VStack (Vertical UIStackView on iOS and LinearLayout on Android)
- HStack (Horizontal UIStackView on iOS and LinearLayout on Android)
- Spacer
- Toggle/Check (tvOS doesn't support it)
- Slider (tvOS doesn't support it)
- ImageView
- WebView
- ScrollView

## Why
- A single language for iOS and Android development using native controls.
- C++ is already supported on both platforms and their de facto IDE's, so why not use it to instantiate native controls.
- Adds little overhead compared to managed languages targetting iOS and Android.
- Ability to use C/C++ (and in case of iOS, Objective-C/C++) within your codebase.
- Builds with your codebase, no toolchain/build system manipulation required.
- Produces slimmer binaries than Swift. Also doesn't require rebuilding your app with each XCode release (in Swift's case). 

## Why not
- Missing controls (ex. TableView). 
- Trying to do anything more involved, you'd have to use the native language of the platform. In Android's case, jni programming is a circle of hell of its own. You can however access the natively created views from Java.
- WatchOS is not wrapped, since it doesn't use UIKit. 
- If you're only targetting apple platforms, SwiftUI is more pleasant to write and can target all apple platforms, including WatchOS and OSX.
- Too early for complex guis. Doesn't support fragments, multiple views, navigation. 

## Usage

### iOS
You can downoload the floui.hpp header and add it to your xcode project. Remember to change the extension of your ViewController.m file to .mm (to indicate you're using C++).
```objc
// ViewController.mm
#import "ViewController.h"
#define FLOUI_IMPL
#include "floui.hpp"

using namespace floui;

static int val = 0;

MainView myview(const FlouiViewController &fvc) {
    auto main_view = MainView(fvc, {
            Button("Increment")
                .action([=](Widget&) {
                    val++;
                    Widget::from_id<Text>("mytext").text(std::to_string(val));
                }),
            Text("0")
                .id("mytext"),
            Button("Decrement")
                .foreground(Color::Red)
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
Add the `#define FLOUI_IMPL` before including floui.hpp in only one Objective-C++ source file.

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

using namespace floui;

static int val = 0;

MainView myview(const FlouiViewController &fvc) {
    auto main_view = MainView(fvc, {
            Button("Increment")
                .foreground(Color::Blue)
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
                .foreground(Color::Red)
                .action([=](Widget&) {
                    val--;
                    Widget::from_id<Text>("val").text(std::to_string(val));
                }),
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

## Usage outside of the platform IDE
Once you've created your project in XCode or Android Studio, development no longer requires them. You can continue using them or use your preferred code editor. You can simply invoke the build system directly (xcodebuild or gradle) from the command-line.
- iOS
    - To build for simulator:
    `xcodebuild build -configuration Debug -sdk iphonesimulator CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO`
    - To boot a simulator from the command-line:
    `xcrun simctl list` then `xcrun simctl boot <device>`. e.x. `xcrun simctl boot "iPhone 13 Pro"`.
    - To run on the simulator:
    ```
    xcrun simctl install "iPhone 13 Pro" build/Debug-iphonesimulator/myproj.app
    xcrun simctl launch "iPhone 13 Pro" com.neurosrg.myproj
    ```
    The launch command should use your bundle identifier (can be found when you create the XCode project under General > Identity)

P.S. You can also generate your project using CMake using the https://github.com/leetal/ios-cmake toolchain file. Your CMakeLists.txt would look something like:
```cmake
cmake_minimum_required(VERSION 3.0)
project(myproj)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MACOSX_BUNDLE_BUNDLE_NAME "A myproj example")
set(MACOSX_BUNDLE_BUNDLE_VERSION 0.1.0)
set(MACOSX_BUNDLE_COPYRIGHT "Copyright ?? 2022 moalyousef.github.io. All rights reserved.")
set(MACOSX_BUNDLE_GUI_IDENTIFIER com.neurosrg.myproj)
set(MACOSX_BUNDLE_ICON_FILE app)
set(MACOSX_BUNDLE_LONG_VERSION_STRING 0.1.0)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1)

add_executable(myproj src/main.mm)
target_link_libraries(myproj PUBLIC "-framework UIKit")
target_compile_features(myproj PUBLIC cxx_std_17)
```
And then configured:
`cmake -Bbin -GXcode -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DPLATFORM=OS64COMBINED`. This can then be built from the command-line using xcodebuild.

- Android
    - You can build using the gradlew script that Android Studio prepares for your project:
    `./gradelw assembleDebug` or `gradlew.bat assembleDebug` on Windows
    - To install on a device:
    `gradlew installDebug`
    - To start a simulator from the command line:
    `$ANDROID_HOME/Sdk/emulator/emulator -list-avds` the select your avd using `$ANDROID_HOME/Sdk/emulator/emulator -avd "<your chosen device>"`

Building for production is easier done through the IDE which offers codesigning and code-shrinking on Android.

## Current limitations:
- Use of const std::string& for text values, std::string_view might not be null-terminated. Converting NSString or jstring from a c string requires strings to be null-terminated.
- Sliders on Android take the full width of the LinearLayout, so this must be taken into consideration if code is shared also with iOS.
- Users of this library should ensure correct type usage when acquiring the type from a Widget, like in a callback:
```cpp
auto button = Widget::from_id<Button>("some_id");
// or
auto slider = Slider(some_widget.inner());
```
Maybe std::any can be used in the library and such casts can pass thru std::any_cast, the problem on Android is that everything is a jobject, and equality can only be checked via JNIenv::IsSameObject, also RTTI is disabled by default on ndk-build. 
- Adding images has to be in the project's resource file. 
    - In Android Studio: Resource Manager, Import Drawables. This will add the file to res/drawable. The file can be accessed directly ImageView("MyImage.jpg").
    - In XCode: You can simply drag images into Assets.xcassets, then the image can be accessed directly ImageView("MyImage.jpg").
- Using the WebView widget
    - on iOS:
        - Requires adding WebKit.framework under General > Frameworks, Libraries and Embedded Content. 
        - Requires defining FLOUI_IOS_WEBVIEW since WebKit isn't linked by default, this avoids link errors.
        - To load local files, precede theme with `file:///` and the path of the file, which should be added to your xcode project. The path can be passed to WebView::load_url().
    - On Android:
        - To load local files, precede them with `file:///` and the path of the file, which should be added to an assets folder (File > New > Folder > Assets folder). 
        - To load http requests, you need to enable the internet permission in your AndroidManifest.xml:
        `<uses-permission android:name="android.permission.INTERNET" />`

- Widgets are not thread-safe.

## Todo
- Wrap more UIKit and Android controls and their methods.
- Code refactoring.
