
constexpr uint32_t argb2rgba(uint32_t argb);

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        auto v = (jobject)view;                                                                    \
        auto setBackgroundColor =                                                                  \
            c::env->GetMethodID(c::env->GetObjectClass(v), "setBackgroundColor", "(I)V");          \
        c::env->CallVoidMethod(v, setBackgroundColor, argb2rgba(col));                             \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::tint(uint32_t col) { return *this; }                                           \
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

using c = FlouiViewControllerImpl;

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
    auto ret = c::env->CallVoidMethod(v, getText);
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
    for (auto e : l) {
        c::env->CallVoidMethod((jobject)view, addview, (jobject)e.inner());
    }
}

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
    for (auto e : l) {
        c::env->CallVoidMethod((jobject)view, addview, (jobject)e.inner());
    }
}

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
    for (auto e : l) {
        c::env->CallVoidMethod((jobject)view, addview, (jobject)e.inner());
    }
}

DEFINE_STYLES(HStack)

void floui_jni_handle_events(jobject view) {
    for (const auto obj : c::callbackmap) {
        if (c::env->IsSameObject(obj.first, view)) { // can't depend on std::hash to get the cb
            auto w = Widget(view);
            (*obj.second)(w);
        }
    }
}

constexpr uint32_t argb2rgba(uint32_t argb) {
    return ((argb & 0x00FF0000) >> 16) | ((argb & 0x0000FF00)) | ((argb & 0x000000FF) << 16) |
           ((argb & 0xFF000000));
}

void floui_log(const char *s) {
    auto cl = c::env->FindClass("android/util/Log");
    auto e = c::env->GetStaticMethodID(cl, "d", "(Ljava/lang/String;Ljava/lang/String;)I");
    c::env->CallStaticIntMethod(cl, e, c::env->NewStringUTF("FlouiApp"), c::env->NewStringUTF(s));
}