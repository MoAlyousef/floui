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
            .foreground(Color::White)
            .fontsize(30)
            .bold()
            .background(Color::Magenta),
        Spacer().size(0, 50),
        VStack({
            Button("Increment")
                .size(0, 40)
                .filled()
                .background(Color::Rgb(0, 0, 255))
                .foreground(Color::Rgb(255, 255, 255, 255))
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
        LinearLayout v = (LinearLayout) mainView(layout);
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

3- In your cpp source file, replace with this boilerplate:
```cpp
#include <jni.h>
#include <string>
#define FLOUI_IMPL
#include "floui.hh"

class MyViewController: FlouiViewController {
    static inline int val = 0;
public:
    MyViewController(JNIEnv* env, jobject m, jobject view): FlouiViewController(env, m, view) {}
    static void handle_events(void *view) {
        FlouiViewController::handle_events(view);
    }
    Widget didLoad() {
        auto v = MainView(this, {
                Button("Increment")
                        .foreground(0x0000ffff)
                        .action([=](Widget &) {
                            val++;
                            Widget::from_id<Text>("mytext").text(std::to_string(val).c_str());
                        }),
                Text("0")
                        .bold()
                        .center()
                        .id("mytext")
                        .size(400, 0)
                        .fontsize(24),
                Button("Decrement")
                        .foreground(0xff0000ff)
                        .action([=](Widget &w) {
                            val--;
                            Widget::from_id<Text>("mytext").text(std::to_string(val).c_str());
                        })
        });
        return v;
    }
};

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_cppal_MainActivity_mainView(
        JNIEnv* env,
        jobject m, jobject view) {
    MyViewController controller(env, m, view);
    return (jobject) controller.didLoad().inner();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cppal_MainActivity_handleEvent(
        JNIEnv* env,
        jobject m, jobject view) {
    MyViewController::handle_events(view);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_cppal_MainActivity_findViewByNativeId(JNIEnv *env, jobject thiz, jstring id) {
    return (jobject)Widget::from_id<Widget>(reinterpret_cast<const char *>(id)).inner();
}
```
Only add the `#define FLOUI_IMPL` before including floui.hpp in only one source file.

![image](https://user-images.githubusercontent.com/37966791/175548084-a0105440-dc32-4f09-be82-0029312efe7c.png)

## Todo
- Wrap more UIKit and Android controls.
- Use proper layout constraints for the mainview to handle orientation changes. 
- Add a Color enum with common colors.
