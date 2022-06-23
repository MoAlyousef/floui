# floui
floui, pronounced "flowy", is a single header C++ lib inspired by SwiftUI, which wraps native iOS and Android controls/widgets, and integrates into the de facto build environments of each platform (XCode and Android Studio).

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

    auto v = MainView((void *)CFBridgingRetain(self), {
        Text("Counter")
            .size(600, 100)
            .center()
            .foreground(0xffffffff)
            .fontsize(30)
            .background(0xff00ffff),
        Spacer().size(0, 50),
        VStack({
            Button("Increment")
                .size(0, 40)
                .filled()
                .background(0x0000ffff)
                .foreground(0xffffffff)
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
        setContentView(R.layout.activity_main);
        LinearLayout v = (LinearLayout) mainView(findViewById(R.id.activity_main));
        v.getLayoutParams().width = ViewGroup.LayoutParams.MATCH_PARENT;
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

3- In your activity_main.xml, give your activity_main an id:
![image](https://user-images.githubusercontent.com/37966791/175358265-9d13c0b3-0655-4c30-8bd9-e36ac5dab7d8.png)
Here we call it "activity_main", since we'll be using findViewById(R.id.activity_main).

4- In your cpp source file, replace with this boilerplate:
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

    FlouiViewController controller(env, m, view);
    
    auto mainView = MainView(&controller, {
        Button("Increment").action([=](Widget) {
            val++;
            Widget::from_id<Text>("val").text(std::to_string(val));
        }),
        Text(0).id("val"),
        Button("Decrement").action([=](Widget) {
            val--;
            Widget::from_id<Text>("val").text(std::to_string(val));
        })
    });
    
    return (jobject) main_view.inner();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_handleEvent(JNIEnv *env, jobject thiz, jobject view) {
    floui_jni_handle_events(view);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_findNativeViewById(JNIEnv *env, jobject thiz,
                                                               jstring id) {
    return (jobject)Widget::from_id<Widget>(reinterpret_cast<const char *>(id)).inner();
}
```
Only add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

## Todo
- Wrap more UIKit and Android controls.
- Use proper layout constraints for the mainview to handle orientation changes. 
