// Only for testing

#include <jni.h>
#include <string>
#define __ANDROID__
#define FLOUI_IMPL
#include "../floui.hh"

class MyViewController: FlouiViewController {
    static inline int val = 0;
public:
    MyViewController(JNIEnv* env, jobject main_activity, jobject layout)
        : FlouiViewController(env, main_activity, layout) {}
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
        jobject main_activity, jobject layout) {
    MyViewController controller(env, main_activity, layout);
    return (jobject) controller.didLoad().inner();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_cppal_MainActivity_handleEvent(
        JNIEnv* env,
        jobject main_activity, jobject view) {
    MyViewController::handle_events(view);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_cppal_MainActivity_findViewByNativeId(JNIEnv *env, jobject thiz, jstring id) {
    return (jobject)Widget::from_id<Widget>(reinterpret_cast<const char *>(id)).inner();
}