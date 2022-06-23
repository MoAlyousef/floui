Assuming your application is called myapplication:

1- Create a native C++ application in Android studio, check the C++ toolchain to be C++17.

2- Replace main activity (MainActivity.java) with:
```java
package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.app.ActionBar;
import android.os.Bundle;
import android.view.View;

import com.example.myapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    static {
        System.loadLibrary("myapplication");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        ConstraintLayout root = binding.getRoot();
        setContentView(root);
        View view = mainView(root);
        view.getLayoutParams().width = ActionBar.LayoutParams.MATCH_PARENT;
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

3- Ensure your build.gradle (app) has the:
```gradle
    buildFeatures {
        viewBinding true
    }
```
After the externalNativeBuild.

4- In your native-lib.cpp, replace with this boilerplate:
```cpp
#include <jni.h>
#include <string>
#define FLOUI_IMPL
#include "floui.h"

static int val = 0;

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_mainView(
        JNIEnv* env,
        jobject main_activity, jobject view) {
    auto fvc = new FlouiViewController(env, main_activity, view);
    auto main_view = MainView(fvc, {
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
    return static_cast<jobject>(Widget::from_id<Widget>(
            reinterpret_cast<const char *>(id)).inner());
}
```
