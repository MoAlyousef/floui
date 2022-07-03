/*
 MIT License

 Copyright (c) 2022 Mohammed Alyousef

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef __FLOUI_HPP__
#define __FLOUI_HPP__

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

void floui_log(const std::string &s);

struct FlouiViewControllerImpl;

class FlouiViewController {
    FlouiViewControllerImpl *impl;

  public:
    FlouiViewController(void *, void * = nullptr, void * = nullptr);
    static void handle_events(void *view);
    ~FlouiViewController();
};

class Color {
    uint32_t c;

  public:
    explicit Color(uint32_t col);
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    operator uint32_t() const;
    enum {
        White = 0xffffffff,
        Red = 0xff0000ff,
        Green = 0x00ff00ff,
        Blue = 0x0000ffff,
        Black = 0x000000ff,
        Yellow = 0xffff00ff,
        Orange = 0xff7f00ff,
        LightGray = 0xaaaaaaff,
        Gray = 0x7f7f7fff,
        DarkGray = 0x555555ff,
        Magenta = 0xff00ffff,
    };
    static Color system_purple();
    static Color rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
};

#define DECLARE_STYLES(widget)                                                                     \
    widget &background(uint32_t col);                                                              \
    widget &id(const char *val);                                                                   \
    widget &size(int w, int h);

class Widget {
  protected:
    static inline std::unordered_map<const char *, void *> widget_map{};
    void *view = nullptr;

  public:
    explicit Widget(void *v);
    void *inner() const;
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Widget, T>>>
    static T from_id(const char *v) {
        return T{widget_map[v]};
    }
    DECLARE_STYLES(Widget)
};

class Button : public Widget {
  public:
    explicit Button(void *b);
    explicit Button(const std::string &label);
    Button &filled();
    Button &action(std::function<void(Widget &)> &&f);
#ifdef __APPLE__
    Button &action(::id target, SEL s);
#endif
    Button &foreground(uint32_t c);
    DECLARE_STYLES(Button)
};

class Toggle : public Widget {
  public:
    explicit Toggle(void *b);
    explicit Toggle(const std::string &label);
    Toggle &value(bool val);
    bool value();
    Toggle &action(std::function<void(Widget &)> &&f);
#ifdef __APPLE__
    Toggle &action(::id target, SEL s);
#endif
    Toggle &foreground(uint32_t c);
    DECLARE_STYLES(Toggle)
};

class Check : public Widget {
  public:
    explicit Check(void *b);
    explicit Check(const std::string &label);
    Check &value(bool val);
    bool value();
    Check &action(std::function<void(Widget &)> &&f);
#ifdef __APPLE__
    Check &action(::id target, SEL s);
#endif
    Check &foreground(uint32_t c);
    DECLARE_STYLES(Check)
};

class Slider : public Widget {
  public:
    explicit Slider(void *b);
    Slider();
    Slider &value(double val);
    double value();
    Slider &action(std::function<void(Widget &)> &&f);
#ifdef __APPLE__
    Slider &action(::id target, SEL s);
#endif
    Slider &foreground(uint32_t c);
    DECLARE_STYLES(Slider)
};

class Text : public Widget {
  public:
    explicit Text(void *b);
    explicit Text(const std::string &s);
    Text &center();
    Text &bold();
    Text &text(const std::string &s);
    Text &foreground(uint32_t c);
    Text &fontsize(int size);
    DECLARE_STYLES(Text)
};

class TextField : public Widget {
  public:
    explicit TextField(void *b);
    TextField();
    TextField &center();
    TextField &text(const std::string &s);
    std::string text() const;
    TextField &fontsize(int size);
    TextField &foreground(uint32_t c);
    DECLARE_STYLES(TextField)
};

class Spacer : public Widget {
  public:
    explicit Spacer(void *b);
    Spacer();
    DECLARE_STYLES(Spacer)
};

class MainView : public Widget {
  public:
    explicit MainView(void *m);
    MainView(const FlouiViewController &vc, std::initializer_list<Widget> l);
    MainView &spacing(int val);
    DECLARE_STYLES(MainView)
};

class VStack : public Widget {
  public:
    explicit VStack(void *v);
    explicit VStack(std::initializer_list<Widget> l);
    VStack &spacing(int val);
    DECLARE_STYLES(VStack)
};

class HStack : public Widget {
  public:
    explicit HStack(void *v);
    explicit HStack(std::initializer_list<Widget> l);
    HStack &spacing(int val);
    DECLARE_STYLES(HStack)
};

class ImageView : public Widget {
  public:
    explicit ImageView(void *v);
    ImageView();
    explicit ImageView(const std::string &path);
    ImageView &image(const std::string &path);
    DECLARE_STYLES(ImageView)
};

#ifdef FLOUI_IMPL

Color::Color(uint32_t col) : c(col) {}

Color::operator uint32_t() const { return c; }

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : c(((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff)) {}

Color Color::rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return Color(r, g, b, a); }

#ifdef __ANDROID__
// Android stuff
#include <jni.h>

Color Color::system_purple() { return Color(0x7f007fff); }

struct FlouiViewControllerImpl {
    static inline JNIEnv *env = nullptr;
    static inline jobject main_activity = nullptr;
    static inline jobject layout = nullptr;
    static inline std::unordered_map<int, std::function<void(Widget &)> *> callbackmap = {};

    FlouiViewControllerImpl(JNIEnv *env, jobject m, jobject layout) {
        FlouiViewControllerImpl::env = env;
        FlouiViewControllerImpl::main_activity = m;
        FlouiViewControllerImpl::layout = layout;
    }
};

FlouiViewController::FlouiViewController(void *env, void *m, void *layout)
    : impl(new FlouiViewControllerImpl((JNIEnv *)env, (jobject)m, (jobject)layout)) {}

static int floui_get_id(jobject view);

void FlouiViewController::handle_events(void *view) {
    auto v = (jobject)view;
    auto elem = FlouiViewControllerImpl::callbackmap.find(floui_get_id(v));
    if (elem != FlouiViewControllerImpl::callbackmap.end()) {
        auto w = Widget(v);
        (*elem->second)(w);
    }
}

FlouiViewController::~FlouiViewController() { delete impl; }

using c = FlouiViewControllerImpl;

static jobject floui_new_view(const char *klass) {
    auto k = c::env->FindClass(klass);
    auto init = c::env->GetMethodID(k, "<init>", "(Landroid/content/Context;)V");
    auto obj = c::env->NewObject(k, init, c::main_activity);
    auto viewc = c::env->FindClass("android/view/View");
    auto generateViewId = c::env->GetStaticMethodID(viewc, "generateViewId", "()I");
    auto id = c::env->CallStaticIntMethod(viewc, generateViewId);
    auto setId = c::env->GetMethodID(c::env->GetObjectClass(obj), "setId", "(I)V");
    c::env->CallVoidMethod(obj, setId, id);
    return obj;
}

static jobject floui_get_by_id(int val) {
    auto viewc = c::env->FindClass("android/view/View");
    auto findViewById = c::env->GetMethodID(viewc, "findViewById", "(I)Landroid/view/View;");
    auto v = c::env->CallObjectMethod(c::main_activity, findViewById, val);
    return v;
}

int floui_get_id(jobject view) {
    auto viewc = c::env->FindClass("android/view/View");
    auto getId = c::env->GetMethodID(viewc, "getId", "()I");
    return c::env->CallIntMethod(view, getId);
}

void floui_log(const std::string &s) {
    auto cl = c::env->FindClass("android/util/Log");
    auto e = c::env->GetStaticMethodID(cl, "d", "(Ljava/lang/String;Ljava/lang/String;)I");
    c::env->CallStaticIntMethod(cl, e, c::env->NewStringUTF("FlouiApp"),
                                c::env->NewStringUTF(s.c_str()));
}

static constexpr uint32_t argb2rgba(uint32_t argb) { return (argb << 24) | (argb >> 8); }

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        auto v = (jobject)view;                                                                    \
        auto setBackgroundColor =                                                                  \
            c::env->GetMethodID(c::env->GetObjectClass(v), "setBackgroundColor", "(I)V");          \
        c::env->CallVoidMethod(v, setBackgroundColor, argb2rgba(col));                             \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::id(const char *val) {                                                          \
        widget_map[val] = view;                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::size(int w, int h) {                                                           \
        auto v = (jobject)view;                                                                    \
        auto layoutc = c::env->FindClass("android/widget/LinearLayout$LayoutParams");              \
        auto init = c::env->GetMethodID(layoutc, "<init>", "(II)V");                               \
        auto obj = c::env->NewObject(layoutc, init, w, h);                                         \
        auto setLayoutParams = c::env->GetMethodID(c::env->GetObjectClass(v), "setLayoutParams",   \
                                                   "(Landroid/view/ViewGroup$LayoutParams;)V");    \
        c::env->CallVoidMethod(v, setLayoutParams, obj);                                           \
        return *this;                                                                              \
    }

Widget::Widget(void *v) : view(v) {}

void *Widget::inner() const { return view; }

DEFINE_STYLES(Widget)

void *Button_init() {
    auto view = floui_new_view("android/widget/Button");
    auto setTransformationMethod =
        c::env->GetMethodID(c::env->GetObjectClass(view), "setTransformationMethod",
                            "(Landroid/text/method/TransformationMethod;)V");
    c::env->CallVoidMethod(view, setTransformationMethod, nullptr);
    return c::env->NewWeakGlobalRef(view);
}

Button::Button(void *b) : Widget(b) {}

Button::Button(const std::string &label) : Widget(Button_init()) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
}

Button &Button::foreground(uint32_t c) {
    auto v = (jobject)view;
    auto setTextColor = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextColor", "(I)V");
    c::env->CallVoidMethod(v, setTextColor, argb2rgba(c));
    return *this;
}

Button &Button::filled() { return *this; }

Button &Button::action(std::function<void(Widget &)> &&f) {
    auto v = (jobject)view;
    auto setOnClickListener = c::env->GetMethodID(c::env->GetObjectClass(v), "setOnClickListener",
                                                  "(Landroid/view/View$OnClickListener;)V");
    c::env->CallVoidMethod(v, setOnClickListener, c::main_activity);
    c::callbackmap[floui_get_id(v)] = new std::function<void(Widget &)>(f);
    return *this;
}

DEFINE_STYLES(Button)

void *Toggle_init() {
    auto view = floui_new_view("android/widget/Switch");
    auto setTransformationMethod =
        c::env->GetMethodID(c::env->GetObjectClass(view), "setTransformationMethod",
                            "(Landroid/text/method/TransformationMethod;)V");
    c::env->CallVoidMethod(view, setTransformationMethod, nullptr);
    return c::env->NewWeakGlobalRef(view);
}

Toggle::Toggle(void *b) : Widget(b) {}

Toggle::Toggle(const std::string &label) : Widget(Toggle_init()) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
}

Toggle &Toggle::value(bool val) {
    auto v = (jobject)view;
    auto setChecked =
        c::env->GetMethodID(c::env->FindClass("android/widget/Switch"), "setChecked", "(Z)V");
    c::env->CallVoidMethod(v, setChecked, val);
    return *this;
}

bool Toggle::value() {
    auto v = (jobject)view;
    auto isChecked = c::env->GetMethodID(c::env->GetObjectClass(v), "isChecked", "()Z");
    return c::env->CallBooleanMethod(v, isChecked);
}

Toggle &Toggle::foreground(uint32_t c) {
    auto v = (jobject)view;
    auto setTextColor = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextColor", "(I)V");
    c::env->CallVoidMethod(v, setTextColor, argb2rgba(c));
    return *this;
}

Toggle &Toggle::action(std::function<void(Widget &)> &&f) {
    auto v = (jobject)view;
    auto setOnClickListener = c::env->GetMethodID(c::env->GetObjectClass(v), "setOnClickListener",
                                                  "(Landroid/view/View$OnClickListener;)V");
    c::env->CallVoidMethod(v, setOnClickListener, c::main_activity);
    c::callbackmap[floui_get_id(v)] = new std::function<void(Widget &)>(f);
    return *this;
}

DEFINE_STYLES(Toggle)

void *Check_init() {
    auto view = floui_new_view("android/widget/CheckBox");
    auto setTransformationMethod =
        c::env->GetMethodID(c::env->GetObjectClass(view), "setTransformationMethod",
                            "(Landroid/text/method/TransformationMethod;)V");
    c::env->CallVoidMethod(view, setTransformationMethod, nullptr);
    return c::env->NewWeakGlobalRef(view);
}

Check::Check(void *b) : Widget(b) {}

Check::Check(const std::string &label) : Widget(Check_init()) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
}

Check &Check::value(bool val) {
    auto v = (jobject)view;
    auto setChecked =
        c::env->GetMethodID(c::env->FindClass("android/widget/CheckBox"), "setChecked", "(Z)V");
    c::env->CallVoidMethod(v, setChecked, val);
    return *this;
}

bool Check::value() {
    auto v = (jobject)view;
    auto isChecked = c::env->GetMethodID(c::env->GetObjectClass(v), "isChecked", "()Z");
    return c::env->CallBooleanMethod(v, isChecked);
}

Check &Check::foreground(uint32_t c) {
    auto v = (jobject)view;
    auto setTextColor = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextColor", "(I)V");
    c::env->CallVoidMethod(v, setTextColor, argb2rgba(c));
    return *this;
}

Check &Check::action(std::function<void(Widget &)> &&f) {
    auto v = (jobject)view;
    auto setOnClickListener = c::env->GetMethodID(c::env->GetObjectClass(v), "setOnClickListener",
                                                  "(Landroid/view/View$OnClickListener;)V");
    c::env->CallVoidMethod(v, setOnClickListener, c::main_activity);
    c::callbackmap[floui_get_id(v)] = new std::function<void(Widget &)>(f);
    return *this;
}

DEFINE_STYLES(Check)

void *Slider_init() {
    auto view = floui_new_view("com/google/android/material/slider/Slider");
    return c::env->NewWeakGlobalRef(view);
}

Slider::Slider(void *b) : Widget(b) {}

Slider::Slider() : Widget(Slider_init()) {}

Slider &Slider::value(double val) {
    auto v = (jobject)view;
    auto setValue = c::env->GetMethodID(c::env->GetObjectClass(v), "setValue", "(F)V");
    c::env->CallVoidMethod(v, setValue, val);
    return *this;
}

double Slider::value() {
    auto v = (jobject)view;
    auto getValue = c::env->GetMethodID(c::env->GetObjectClass(v), "getValue", "()F");
    return c::env->CallFloatMethod(v, getValue);
}

Slider &Slider::foreground(uint32_t c) { return *this; }

Slider &Slider::action(std::function<void(Widget &)> &&f) {
    auto v = (jobject)view;
    auto addOnChangeListener = c::env->GetMethodID(
        c::env->FindClass("com/google/android/material/slider/Slider"), "addOnChangeListener",
        "(Lcom/google/android/material/slider/BaseOnChangeListener;)V");
    c::env->CallVoidMethod(v, addOnChangeListener, c::main_activity);
    c::callbackmap[floui_get_id(v)] = new std::function<void(Widget &)>(f);
    return *this;
}

DEFINE_STYLES(Slider)

void *Text_init() {
    auto view = floui_new_view("android/widget/TextView");
    return c::env->NewWeakGlobalRef(view);
}

Text::Text(void *b) : Widget(b) {}

Text::Text(const std::string &label) : Widget(Text_init()) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
}

Text &Text::fontsize(int size) {
    auto v = (jobject)view;
    auto setTextSize = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextSize", "(F)V");
    c::env->CallVoidMethod(v, setTextSize, (float)size);
    return *this;
}

Text &Text::bold() {
    auto v = (jobject)view;
    auto setTypeface = c::env->GetMethodID(c::env->GetObjectClass(v), "setTypeface",
                                           "(Landroid/graphics/Typeface;I)V");
    c::env->CallVoidMethod(v, setTypeface, (jobject) nullptr, 1);
    return *this;
}

Text &Text::text(const std::string &label) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
    return *this;
}

Text &Text::center() {
    auto v = (jobject)view;
    auto setGravity = c::env->GetMethodID(c::env->GetObjectClass(v), "setGravity", "(I)V");
    c::env->CallVoidMethod(v, setGravity, 17 /*center*/);
    return *this;
}

Text &Text::foreground(uint32_t c) {
    auto v = (jobject)view;
    auto setTextColor = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextColor", "(I)V");
    c::env->CallVoidMethod(v, setTextColor, argb2rgba(c));
    return *this;
}

DEFINE_STYLES(Text)

void *TextField_init() {
    auto view = floui_new_view("android/widget/EditText");
    return c::env->NewWeakGlobalRef(view);
}

TextField::TextField(void *b) : Widget(b) {}

TextField::TextField() : Widget(TextField_init()) {}

TextField &TextField::fontsize(int size) {
    auto v = (jobject)view;
    auto setTextSize = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextSize", "(F)V");
    c::env->CallVoidMethod(v, setTextSize, (float)size);
    return *this;
}

TextField &TextField::text(const std::string &label) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
    return *this;
}

std::string TextField::text() const {
    auto v = (jobject)view;
    auto getText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "getText", "()Ljava/lang/CharSequence;");
    auto ret = c::env->CallObjectMethod(v, getText);
    return std::string(reinterpret_cast<const char *>(ret));
}

TextField &TextField::center() {
    auto v = (jobject)view;
    auto setGravity = c::env->GetMethodID(c::env->GetObjectClass(v), "setGravity", "(I)V");
    c::env->CallVoidMethod(v, setGravity, 17 /*center*/);
    return *this;
}

TextField &TextField::foreground(uint32_t c) {
    auto v = (jobject)view;
    auto setTextColor = c::env->GetMethodID(c::env->GetObjectClass(v), "setTextColor", "(I)V");
    c::env->CallVoidMethod(v, setTextColor, argb2rgba(c));
    return *this;
}

DEFINE_STYLES(TextField)

void *Spacer_init() {
    auto view = floui_new_view("android/widget/Space");
    return c::env->NewWeakGlobalRef(view);
}

Spacer::Spacer(void *b) : Widget(b) {}

Spacer::Spacer() : Widget(Spacer_init()) {}

DEFINE_STYLES(Spacer)

void *VStack_init() {
    auto view = floui_new_view("android/widget/LinearLayout");
    auto setOrientation =
        c::env->GetMethodID(c::env->GetObjectClass(view), "setOrientation", "(I)V");
    c::env->CallVoidMethod(view, setOrientation, 1 /*vertical*/);
    auto setGravity = c::env->GetMethodID(c::env->GetObjectClass(view), "setGravity", "(I)V");
    c::env->CallVoidMethod(view, setGravity, 17 /*center*/);
    return c::env->NewWeakGlobalRef(view);
}

MainView::MainView(void *m) : Widget(m) {}

MainView::MainView(const FlouiViewController &, std::initializer_list<Widget> l)
    : Widget(VStack_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    c::env->CallVoidMethod(c::layout, addview, v);
    auto getLayoutParams = c::env->GetMethodID(c::env->GetObjectClass(c::layout), "getLayoutParams",
                                               "()Landroid/view/ViewGroup$LayoutParams;");
    auto params = c::env->CallObjectMethod(v, getLayoutParams);
    auto width = c::env->GetFieldID(c::env->GetObjectClass(params), "width", "I");
    c::env->SetIntField(params, width, -1);
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

MainView &MainView::spacing(int space) { return *this; }

DEFINE_STYLES(MainView)

VStack::VStack(void *m) : Widget(m) {}

VStack::VStack(std::initializer_list<Widget> l) : Widget(VStack_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

VStack &VStack::spacing(int space) { return *this; }

DEFINE_STYLES(VStack)

void *HStack_init() {
    auto view = floui_new_view("android/widget/LinearLayout");
    auto setOrientation =
        c::env->GetMethodID(c::env->GetObjectClass(view), "setOrientation", "(I)V");
    c::env->CallVoidMethod(view, setOrientation, 0 /*Horizontal*/);
    auto setGravity = c::env->GetMethodID(c::env->GetObjectClass(view), "setGravity", "(I)V");
    c::env->CallVoidMethod(view, setGravity, 17 /*center*/);
    return c::env->NewWeakGlobalRef(view);
}

HStack::HStack(void *m) : Widget(m) {}

HStack::HStack(std::initializer_list<Widget> l) : Widget(HStack_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

HStack &HStack::spacing(int space) { return *this; }

DEFINE_STYLES(HStack)

void *ImageView_init(const std::string &path) {
    auto getResources = c::env->GetMethodID(c::env->GetObjectClass(c::main_activity),
                                            "getResources", "()Landroid/content/res/Resources;");
    auto resources = c::env->CallObjectMethod(c::main_activity, getResources);
    auto getPackageName = c::env->GetMethodID(c::env->GetObjectClass(c::main_activity),
                                              "getPackageName", "()Ljava/lang/String;");
    auto packageName = c::env->CallObjectMethod(c::main_activity, getPackageName);
    auto getIdentifier =
        c::env->GetMethodID(c::env->GetObjectClass(resources), "getIdentifier",
                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
    auto resId = c::env->CallIntMethod(resources, getIdentifier,
                                       c::env->NewStringUTF(path.substr(0, path.find('.')).c_str()),
                                       c::env->NewStringUTF("drawable"), packageName);
    auto view = floui_new_view("android/widget/ImageView");
    auto setImageResource = c::env->GetMethodID(c::env->FindClass("android/widget/ImageView"),
                                                "setImageResource", "(I)V");
    c::env->CallVoidMethod(view, setImageResource, resId);
    return c::env->NewWeakGlobalRef(view);
}

void *ImageView_init() {
    auto view = floui_new_view("android/widget/ImageView");
    return c::env->NewWeakGlobalRef(view);
}

ImageView::ImageView(void *v) : Widget(v) {}

ImageView::ImageView() : Widget(ImageView_init()) {}

ImageView::ImageView(const std::string &path) : Widget(ImageView_init(path)) {}

ImageView &ImageView::image(const std::string &path) {
    auto v = (jobject)view;
    auto getResources = c::env->GetMethodID(c::env->GetObjectClass(c::main_activity),
                                            "getResources", "()Landroid/content/res/Resources;");
    auto resources = c::env->CallObjectMethod(c::main_activity, getResources);
    auto getPackageName = c::env->GetMethodID(c::env->GetObjectClass(c::main_activity),
                                              "getPackageName", "()Ljava/lang/String;");
    auto packageName = c::env->CallObjectMethod(c::main_activity, getPackageName);
    auto getIdentifier =
        c::env->GetMethodID(c::env->GetObjectClass(resources), "getIdentifier",
                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
    auto resId = c::env->CallIntMethod(resources, getIdentifier,
                                       c::env->NewStringUTF(path.substr(0, path.find('.')).c_str()),
                                       c::env->NewStringUTF("drawable"), packageName);
    auto setImageResource = c::env->GetMethodID(c::env->FindClass("android/widget/ImageView"),
                                                "setImageResource", "(I)V");
    c::env->CallVoidMethod(v, setImageResource, resId);
    return *this;
}

DEFINE_STYLES(ImageView)

#elif defined(__APPLE__)

#import <Foundation/Foundation.h>

@interface Callback : NSObject {
    std::function<void(Widget &)> *fn_;
    void *target_;
}
- (id)initWithTarget:(void *)target Cb:(const std::function<void(Widget &)> &)f;
- (void)invoke;
- (void)dealloc;
@end

void floui_log(const std::string &s) { NSLog(@"%@", [NSString stringWithUTF8String:s.c_str()]); }

@implementation Callback
- (id)initWithTarget:(void *)target Cb:(const std::function<void(Widget &)> &)f {
    self = [super init];
    fn_ = new std::function<void(Widget &)>(f);
    target_ = target;
    return self;
}
- (void)invoke {
    auto w = Widget(target_);
    (*fn_)(w);
}
- (void)dealloc {
    delete fn_;
    fn_ = nil;
}
@end

#if TARGET_OS_IPHONE
// ios stuff
#import <UIKit/UIKit.h>

struct FlouiViewControllerImpl {
    static inline UIViewController *vc = nullptr;
    static inline const char *name = nullptr;
    static inline std::vector<Callback *> callbacks = {};

    FlouiViewControllerImpl(UIViewController *vc, const char *name, void *) {
        FlouiViewControllerImpl::vc = vc;
        FlouiViewControllerImpl::name = name;
    }
};

FlouiViewController::FlouiViewController(void *vc, void *name, void *)
    : impl(new FlouiViewControllerImpl((__bridge UIViewController *)vc, (const char *)name,
                                       nullptr)) {}

void FlouiViewController::handle_events(void *) { return; }

FlouiViewController::~FlouiViewController() { delete impl; }

Color Color::system_purple() {
    CGFloat r = 0, g = 0, b = 0, a = 0;
    [UIColor.purpleColor getRed:&r green:&g blue:&b alpha:&a];
    return Color(
        ((uint32_t)(r * 255) << 24 | (uint32_t)(g * 255) << 16 | (uint32_t)(b * 255) << 8) |
        (uint32_t)(a * 255));
}

static UIColor *col2uicol(uint32_t col) {
    auto r = ((col >> 24) & 0xFF) / 255.0;
    auto g = ((col >> 16) & 0xFF) / 255.0;
    auto b = ((col >> 8) & 0xFF) / 255.0;
    auto a = ((col)&0xFF) / 255.0;
    return [UIColor colorWithRed:r green:g blue:b alpha:a];
}

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        auto v = (__bridge UIView *)view;                                                          \
        v.backgroundColor = col2uicol(col);                                                        \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::id(const char *val) {                                                          \
        widget_map[val] = view;                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::size(int w, int h) {                                                           \
        auto v = (__bridge UIView *)view;                                                          \
        auto frame = v.frame;                                                                      \
        frame.size.width = w;                                                                      \
        frame.size.height = h;                                                                     \
        v.frame = frame;                                                                           \
        return *this;                                                                              \
    }

Widget::Widget(void *v) : view(v) {}

void *Widget::inner() const { return view; }

DEFINE_STYLES(Widget)

Button::Button(void *b) : Widget(b) {}

Button::Button(const std::string &label)
    : Widget((void *)CFBridgingRetain([UIButton buttonWithType:UIButtonTypeCustom])) {
    auto v = (__bridge UIButton *)view;
    [v setTitle:[NSString stringWithUTF8String:label.c_str()] forState:UIControlStateNormal];
    [v setTitleColor:UIColor.blueColor forState:UIControlStateNormal];
}

Button &Button::filled() {
    ((__bridge UIButton *)view).configuration = [UIButtonConfiguration filledButtonConfiguration];
    return *this;
}

Button &Button::action(std::function<void(Widget &)> &&f) {
    auto v = (__bridge UIButton *)view;
    auto &callbacks = FlouiViewControllerImpl::callbacks;
    callbacks.push_back([[Callback alloc] initWithTarget:view Cb:f]);
    [v addTarget:callbacks.back()
                  action:@selector(invoke)
        forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    auto v = (__bridge UIButton *)view;
    [v addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::foreground(uint32_t c) {
    auto v = (__bridge UIButton *)view;
    [v setTitleColor:col2uicol(c) forState:UIControlStateNormal];
    return *this;
}

DEFINE_STYLES(Button)

Toggle::Toggle(void *b) : Widget(b) {}

Toggle::Toggle(const std::string &label) : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    [v setAxis:UILayoutConstraintAxisHorizontal];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    auto l = [UILabel new];
    [l setText:[NSString stringWithUTF8String:label.c_str()]];
    [v addArrangedSubview:l];
    auto s = [UISwitch new];
    [v addArrangedSubview:s];
}

Toggle &Toggle::value(bool val) {
    auto v = (__bridge UIStackView *)view;
    auto o = [[v subviews] lastObject];
    [(UISwitch *)o setOn:val animated:YES];
    return *this;
}

bool Toggle::value() {
    auto v = (__bridge UISwitch *)view;
    auto o = (UISwitch *)[[v subviews] lastObject];
    return o.on;
}

Toggle &Toggle::action(std::function<void(Widget &)> &&f) {
    auto v = (__bridge UISwitch *)view;
    auto o = [[v subviews] lastObject];
    auto &callbacks = FlouiViewControllerImpl::callbacks;
    callbacks.push_back([[Callback alloc] initWithTarget:view Cb:f]);
    [(UISwitch *)o addTarget:callbacks.back()
                      action:@selector(invoke)
            forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Toggle &Toggle::action(::id target, SEL s) {
    auto v = (__bridge UISwitch *)view;
    auto o = [[v subviews] lastObject];
    [(UISwitch *)o addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Toggle &Toggle::foreground(uint32_t c) { return *this; }

DEFINE_STYLES(Toggle)

Check::Check(void *b) : Widget(b) {}

Check::Check(const std::string &label) : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    [v setAxis:UILayoutConstraintAxisHorizontal];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    auto l = [UILabel new];
    [l setText:[NSString stringWithUTF8String:label.c_str()]];
    [v addArrangedSubview:l];
    auto s = [UISwitch new];
    [s setPreferredStyle:UISwitchStyleCheckbox];
    [v addArrangedSubview:s];
}

Check &Check::value(bool val) {
    auto v = (__bridge UIStackView *)view;
    auto o = [[v subviews] lastObject];
    [(UISwitch *)o setOn:val animated:YES];
    return *this;
}

bool Check::value() {
    auto v = (__bridge UISwitch *)view;
    auto o = (UISwitch *)[[v subviews] lastObject];
    return o.on;
}

Check &Check::action(std::function<void(Widget &)> &&f) {
    auto v = (__bridge UISwitch *)view;
    auto o = [[v subviews] lastObject];
    auto &callbacks = FlouiViewControllerImpl::callbacks;
    callbacks.push_back([[Callback alloc] initWithTarget:view Cb:f]);
    [(UISwitch *)o addTarget:callbacks.back()
                      action:@selector(invoke)
            forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Check &Check::action(::id target, SEL s) {
    auto v = (__bridge UISwitch *)view;
    auto o = [[v subviews] lastObject];
    [(UISwitch *)o addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Check &Check::foreground(uint32_t c) { return *this; }

DEFINE_STYLES(Check)

Slider::Slider(void *b) : Widget(b) {}

Slider::Slider() : Widget((void *)CFBridgingRetain([UISlider new])) {
    //    auto v = (__bridge UISlider *)view;
}

Slider &Slider::value(double val) {
    auto v = (__bridge UISlider *)view;
    [v setValue:val];
    return *this;
}

double Slider::value() {
    auto v = (__bridge UISlider *)view;
    return v.value;
}

Slider &Slider::action(std::function<void(Widget &)> &&f) {
    auto v = (__bridge UISlider *)view;
    auto &callbacks = FlouiViewControllerImpl::callbacks;
    callbacks.push_back([[Callback alloc] initWithTarget:view Cb:f]);
    [v addTarget:callbacks.back()
                  action:@selector(invoke)
        forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Slider &Slider::action(::id target, SEL s) {
    auto v = (__bridge UISlider *)view;
    [v addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Slider &Slider::foreground(uint32_t c) { return *this; }

DEFINE_STYLES(Slider)

Text::Text(void *b) : Widget(b) {}

Text::Text(const std::string &s) : Widget((void *)CFBridgingRetain([UILabel new])) {
    auto v = (__bridge UILabel *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    [v setTextColor:UIColor.blackColor];
}

Text &Text::foreground(uint32_t c) {
    auto v = (__bridge UILabel *)view;
    [v setTextColor:col2uicol(c)];
    return *this;
}

Text &Text::center() {
    auto v = (__bridge UILabel *)view;
    [v setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(const std::string &s) {
    auto v = (__bridge UILabel *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

Text &Text::fontsize(int size) {
    auto v = (__bridge UILabel *)view;
    [v setFont:[UIFont systemFontOfSize:size]];
    return *this;
}

Text &Text::bold() {
    auto v = (__bridge UILabel *)view;
    [v setFont:[UIFont boldSystemFontOfSize:v.font.pointSize]];
    return *this;
}

DEFINE_STYLES(Text)

TextField::TextField(void *b) : Widget(b) {}

TextField::TextField() : Widget((void *)CFBridgingRetain([UITextField new])) {
    auto v = (__bridge UITextField *)view;
    [v setTextColor:UIColor.blackColor];
}

TextField &TextField::foreground(uint32_t c) {
    auto v = (__bridge UITextField *)view;
    [v setTextColor:col2uicol(c)];
    return *this;
}

TextField &TextField::center() {
    auto v = (__bridge UITextField *)view;
    [v setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

TextField &TextField::text(const std::string &s) {
    auto v = (__bridge UITextField *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

std::string TextField::text() const {
    return std::string([((__bridge UITextField *)view).text UTF8String]);
}

TextField &TextField::fontsize(int size) {
    auto v = (__bridge UITextField *)view;
    [v setFont:[UIFont systemFontOfSize:size]];
    return *this;
}

DEFINE_STYLES(TextField)

Spacer::Spacer(void *b) : Widget(b) {}

Spacer::Spacer() : Widget((void *)CFBridgingRetain([UIView new])) {}

DEFINE_STYLES(Spacer)

MainView::MainView(void *v) : Widget(v) {}

MainView::MainView(const FlouiViewController &, std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    v.translatesAutoresizingMaskIntoConstraints = NO;
    auto vc = FlouiViewControllerImpl::vc;
    auto label = [UILabel new];
    auto name = FlouiViewControllerImpl::name;
    if (name) {
        [label setText:[NSString stringWithUTF8String:name]];
    } else {
        [label setText:[[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"]];
    }
    [label setTextColor:UIColor.blackColor];
    label.backgroundColor = UIColor.systemGray5Color;
    [label setFont:[UIFont boldSystemFontOfSize:30]];
    [label setTextAlignment:NSTextAlignmentCenter];
    label.frame = CGRectMake(0, 0, vc.view.frame.size.width, 120);
    [vc.view addSubview:label];
    [vc.view addSubview:v];
    [v setAxis:UILayoutConstraintAxisVertical];
    [v setDistribution:UIStackViewDistributionEqualSpacing];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    [v setContentHuggingPriority:UILayoutPriorityRequired forAxis:UILayoutConstraintAxisHorizontal];
    [v.widthAnchor constraintEqualToConstant:vc.view.frame.size.width].active = YES;
    [v.topAnchor constraintEqualToAnchor:label.bottomAnchor constant:20].active = YES;
    for (auto &e : l) {
        auto w = (__bridge UIView *)e.inner();
        w.translatesAutoresizingMaskIntoConstraints = NO;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

MainView &MainView::spacing(int val) {
    auto v = (__bridge UIStackView *)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(MainView)

VStack::VStack(void *v) : Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    [v setAxis:UILayoutConstraintAxisVertical];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto &e : l) {
        auto w = (__bridge UIView *)e.inner();
        w.translatesAutoresizingMaskIntoConstraints = NO;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

VStack &VStack::spacing(int val) {
    auto v = (__bridge UIStackView *)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(VStack)

HStack::HStack(void *v) : Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    [v setAxis:UILayoutConstraintAxisHorizontal];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto &e : l) {
        auto w = (__bridge UIView *)e.inner();
        w.translatesAutoresizingMaskIntoConstraints = NO;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

HStack &HStack::spacing(int val) {
    auto v = (__bridge UIStackView *)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(HStack)

ImageView::ImageView(void *v) : Widget(v) {}

ImageView::ImageView() : Widget((void *)CFBridgingRetain([UIImageView new])) {}

ImageView::ImageView(const std::string &path)
    : Widget((void *)CFBridgingRetain([UIImageView new])) {
    auto i = [UIImage imageNamed:[NSString stringWithUTF8String:path.c_str()]];
    auto v = (__bridge UIImageView *)view;
    [v setImage:i];
    [v setContentMode:UIViewContentModeScaleAspectFit];
}

ImageView &ImageView::image(const std::string &path) {
    auto v = (__bridge UIImageView *)view;
    auto i = [UIImage imageNamed:[NSString stringWithUTF8String:path.c_str()]];
    [v setImage:i];
    return *this;
}

DEFINE_STYLES(ImageView)

#endif // TARGET_OS_IPHONE

#else
// other platform
#endif // __ANDROID__

#endif // FLOUI_IMPL

#endif // __FLOUI_HPP__
