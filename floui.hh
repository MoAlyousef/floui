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

#ifndef __FLOUI_H__
#define __FLOUI_H__

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

void floui_log(const std::string &s);

#ifdef __ANDROID__
struct FlouiViewControllerImpl;

class FlouiViewController {
    FlouiViewControllerImpl *impl;

  public:
    FlouiViewController(void *, void *, void *);
    static void handle_events(void *view);
};
#endif // __ANDROID__

class Color {
    uint32_t c;

  public:
    explicit Color(uint32_t col);
    operator uint32_t();
    struct Rgb {
        Rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    Color(Rgb rgb);
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
    void *cb_ = nullptr;

  public:
    explicit Button(void *b);
    explicit Button(const std::string &label);
    Button &text(const std::string &label);
    Button &filled();
    Button &action(std::function<void(Widget &)> &&f);
#ifdef __APPLE__
    Button &action(::id target, SEL s);
#endif
    Button &foreground(uint32_t c);
    DECLARE_STYLES(Button)
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
    MainView(void *vc, std::initializer_list<Widget> l);
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

#ifdef FLOUI_IMPL

Color::Color(uint32_t col) : c(col) {}

operator uint32_t() { return c; }

Color::Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

Color::Color(Rgb rgb) : c(((rgb.r & 0xff) << 24) + ((rgb.g & 0xff) << 16) + ((rgb.b & 0xff) << 8) + (rgb.a & 0xff)) {}

#ifdef __ANDROID__
// Android stuff
#include <jni.h>

Color Color::system_purple() { return Color(0x7f007fff); }

struct FlouiViewControllerImpl {
    static inline JNIEnv *env = nullptr;
    static inline jobject m = nullptr;
    static inline jobject layout = nullptr;
    static inline std::unordered_map<jobject, std::function<void(Widget &)> *> callbackmap = {};

    FlouiViewControllerImpl(JNIEnv *env, jobject m, jobject layout) {
        FlouiViewControllerImpl::env = env;
        FlouiViewControllerImpl::m = m;
        FlouiViewControllerImpl::layout = layout;
    }
};

FlouiViewController::FlouiViewController(void *env, void *m, void *layout)
    : impl(new FlouiViewControllerImpl((JNIEnv *)env, (jobject)m, (jobject)layout)) {}

void FlouiViewController::handle_events(void *view) {
    auto v = (jobject)view;
    for (const auto obj : FlouiViewControllerImpl::callbackmap) {
        if (FlouiViewControllerImpl::env->IsSameObject(
                obj.first, v)) { // can't depend on std::hash to get the cb
            auto w = Widget(v);
            (*obj.second)(w);
        }
    }
}

using c = FlouiViewControllerImpl;

void floui_log(const std::string &s) {
    auto cl = c::env->FindClass("android/util/Log");
    auto e = c::env->GetStaticMethodID(cl, "d", "(Ljava/lang/String;Ljava/lang/String;)I");
    c::env->CallStaticIntMethod(cl, e, c::env->NewStringUTF("FlouiApp"),
                                c::env->NewStringUTF(s.c_str()));
}

constexpr uint32_t argb2rgba(uint32_t argb) { return (argb << 24) | (argb >> 8); }

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
        auto setWidth = c::env->GetMethodID(c::env->GetObjectClass(v), "setMinimumWidth", "(I)V"); \
        auto setHeight =                                                                           \
            c::env->GetMethodID(c::env->GetObjectClass(v), "setMinimumHeight", "(I)V");            \
        if (w != 0)                                                                                \
            c::env->CallVoidMethod(v, setWidth, w);                                                \
        if (h != 0)                                                                                \
            c::env->CallVoidMethod(v, setHeight, h);                                               \
        return *this;                                                                              \
    }

Widget::Widget(void *v) : view(v) {}

void *Widget::inner() const { return view; }

DEFINE_STYLES(Widget)

void *Button_init() {
    auto btnc = c::env->FindClass("android/widget/Button");
    auto init = c::env->GetMethodID(btnc, "<init>", "(Landroid/content/Context;)V");
    auto btn = c::env->NewObject(btnc, init, c::m);
    return c::env->NewGlobalRef(btn);
}

Button::Button(void *b) : Widget(b) {}

Button::Button(const std::string &label) : Widget(Button_init()) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
}

Button &Button::text(const std::string &label) {
    auto v = (jobject)view;
    auto setText =
        c::env->GetMethodID(c::env->GetObjectClass(v), "setText", "(Ljava/lang/CharSequence;)V");
    c::env->CallVoidMethod(v, setText, c::env->NewStringUTF(label.c_str()));
    return *this;
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
    c::callbackmap[v] = new std::function<void(Widget &)>(f);
    auto setOnClickListener = c::env->GetMethodID(c::env->GetObjectClass(v), "setOnClickListener",
                                                  "(Landroid/view/View$OnClickListener;)V");
    c::env->CallVoidMethod(v, setOnClickListener, c::m);
    return *this;
}

DEFINE_STYLES(Button)

void *Text_init() {
    auto tvc = c::env->FindClass("android/widget/TextView");
    auto init = c::env->GetMethodID(tvc, "<init>", "(Landroid/content/Context;)V");
    auto tv = c::env->NewObject(tvc, init, c::m);
    return c::env->NewGlobalRef(tv);
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
    auto tvc = c::env->FindClass("android/widget/EditText");
    auto init = c::env->GetMethodID(tvc, "<init>", "(Landroid/content/Context;)V");
    auto tv = c::env->NewObject(tvc, init, c::m);
    return c::env->NewGlobalRef(tv);
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
    auto vc = c::env->FindClass("android/widget/Space");
    auto init = c::env->GetMethodID(vc, "<init>", "(Landroid/content/Context;)V");
    auto v = c::env->NewObject(vc, init, c::m);
    return c::env->NewGlobalRef(v);
}

Spacer::Spacer(void *b) : Widget(b) {}

Spacer::Spacer() : Widget(Spacer_init()) {}

DEFINE_STYLES(Spacer)

void *MainView_init() {
    auto listc = c::env->FindClass("android/widget/LinearLayout");
    auto init = c::env->GetMethodID(listc, "<init>", "(Landroid/content/Context;)V");
    auto list = c::env->NewObject(listc, init, c::m);
    auto setOrientation = c::env->GetMethodID(listc, "setOrientation", "(I)V");
    c::env->CallVoidMethod(list, setOrientation, 1 /*vertical*/);
    auto setGravity = c::env->GetMethodID(listc, "setGravity", "(I)V");
    c::env->CallVoidMethod(list, setGravity, 17 /*center*/);
    return c::env->NewGlobalRef(list);
}

MainView::MainView(void *m) : Widget(m) {}

MainView::MainView(void *vc, std::initializer_list<Widget> l) : Widget(MainView_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    c::env->CallVoidMethod(c::layout, addview, v);
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

MainView &MainView::spacing(int space) { return *this; }

DEFINE_STYLES(MainView)

void *VStack_init() {
    auto listc = c::env->FindClass("android/widget/LinearLayout");
    auto init = c::env->GetMethodID(listc, "<init>", "(Landroid/content/Context;)V");
    auto list = c::env->NewObject(listc, init, c::m);
    auto setOrientation = c::env->GetMethodID(listc, "setOrientation", "(I)V");
    c::env->CallVoidMethod(list, setOrientation, 1 /*vertical*/);
    auto setGravity = c::env->GetMethodID(listc, "setGravity", "(I)V");
    c::env->CallVoidMethod(list, setGravity, 17 /*center*/);
    return c::env->NewGlobalRef(list);
}

VStack::VStack(void *m) : Widget(m) {}

VStack::VStack(std::initializer_list<Widget> l) : Widget(MainView_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    c::env->CallVoidMethod(c::layout, addview, v);
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

VStack &VStack::spacing(int space) { return *this; }

DEFINE_STYLES(VStack)

void *HStack_init() {
    auto listc = c::env->FindClass("android/widget/LinearLayout");
    auto init = c::env->GetMethodID(listc, "<init>", "(Landroid/content/Context;)V");
    auto list = c::env->NewObject(listc, init, c::m);
    auto setGravity = c::env->GetMethodID(listc, "setGravity", "(I)V");
    c::env->CallVoidMethod(list, setGravity, 17 /*center*/);
    return c::env->NewGlobalRef(list);
}

HStack::HStack(void *m) : Widget(m) {}

HStack::HStack(std::initializer_list<Widget> l) : Widget(MainView_init()) {
    auto v = (jobject)view;
    auto addview = c::env->GetMethodID(c::env->FindClass("android/view/ViewGroup"), "addView",
                                       "(Landroid/view/View;)V");
    c::env->CallVoidMethod(c::layout, addview, v);
    for (auto &e : l) {
        c::env->CallVoidMethod(v, addview, (jobject)e.inner());
    }
}

HStack &HStack::spacing(int space) { return *this; }

DEFINE_STYLES(HStack)

#elif defined(__APPLE__)

#import <Foundation/Foundation.h>

Color Color::system_purple() {
    CGFloat r = 0, g = 0, b = 0, a = 0;
    [UIColor.purpleColor getRed:&r green:&g blue:&b alpha:&a];
    return Color(((r*255)<<24 | (g*255)<<16 | (b*255)<<8) | (a*255));  
}

void floui_log(const std::string &s) { NSLog(@"%@", [NSString stringWithUTF8String:s.c_str()]); }

@interface Callback : NSObject {
    std::function<void(Widget &)> *fn_;
    void *target_;
}
- (id)initWithTarget:(void *)target Cb:(const std::function<void(Widget &)> &)f;
- (void)invoke;
- (void)dealloc;
@end

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

UIColor *col2uicol(uint32_t col) {
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
    cb_ = (void *)CFBridgingRetain([[Callback alloc] initWithTarget:view Cb:f]);
    [v addTarget:(__bridge Callback *)cb_
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

MainView::MainView(void *fvc, std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([UIStackView new])) {
    auto v = (__bridge UIStackView *)view;
    auto vc = (__bridge UIViewController *)fvc;
    [vc.view addSubview:v];
    v.frame = vc.view.frame;
    [v setAxis:UILayoutConstraintAxisVertical];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto &e : l) {
        auto w = (__bridge UIView *)e.inner();
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
        [w.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:0].active = YES;
        [w.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:0].active = YES;
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

#endif // TARGET_OS_IPHONE

#else
// other platform
#endif // __ANDROID__

#endif // FLOUI_IMPL

#endif // __FLOUI_H__
