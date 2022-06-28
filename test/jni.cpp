// For testing purposes. Don't define __ANDROID__ !

#include <jni.h>
#include <string>
#define __ANDROID__
#define FLOUI_IMPL
#include "../floui.hh"

static int val = 0;

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_mainView(
        JNIEnv* env,
        jobject main_activity, jobject view) {

    FlouiViewController controller(env, main_activity, view);

    auto main_view = MainView(&controller, {
        Button("Increment").action([=](Widget) {
            val++;
            Widget::from_id<Text>("val").text(std::to_string(val));
        }),
        Text("0").center().bold().fontsize(20).foreground(Color::Black).id("val"),
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
    FlouiViewController::handle_events(view);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_findNativeViewById(JNIEnv *env, jobject thiz,
                                                               jstring id) {
    return (jobject)Widget::from_id<Widget>(reinterpret_cast<const char *>(id)).inner();
}