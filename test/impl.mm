// An implementation for osx cocoa, only for testing purposes

#define FLOUI_IMPL
#include "../floui.hh"

#import <Cocoa/Cocoa.h>

struct FlouiViewControllerImpl {
    static inline NSViewController *vc = nullptr;
    static inline const char *name = nullptr;
    static inline std::vector<Callback *> callbacks = {};

    FlouiViewControllerImpl(NSViewController *vc, const char *name, void *) {
        FlouiViewControllerImpl::vc = vc;
        FlouiViewControllerImpl::name = name;
    }
};

FlouiViewController::FlouiViewController(void *vc, void *name, void *)
    : impl(new FlouiViewControllerImpl((__bridge NSViewController *)vc, (const char *)name,
                                       nullptr)) {}

void FlouiViewController::handle_events(void *) { return; }

FlouiViewController::~FlouiViewController() { delete impl; }

NSColor *col2nscol(uint32_t col) {
    auto r = ((col >> 24) & 0xFF) / 255.0;
    auto g = ((col >> 16) & 0xFF) / 255.0;
    auto b = ((col >> 8) & 0xFF) / 255.0;
    auto a = ((col)&0xFF) / 255.0;
    return [NSColor colorWithRed:r green:g blue:b alpha:a];
}

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        auto v = (__bridge NSView *)view;                                                          \
        v.layer.backgroundColor = col2nscol(col).CGColor;                                          \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::id(const char *val) {                                                          \
        widget_map[val] = view;                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::size(int w, int h) {                                                           \
        auto v = (__bridge NSView *)view;                                                          \
        auto frame = v.frame;                                                                      \
        frame.size.width = w;                                                                      \
        frame.size.height = h;                                                                     \
        v.frame = frame;                                                                           \
        return *this;                                                                              \
    }

Widget::Widget(void *v) : view(v) { ((__bridge NSView *)view).wantsLayer = YES; }

void *Widget::inner() const { return view; }

DEFINE_STYLES(Widget)

Button::Button(void *b) : Widget(b) {}

Button::Button(const std::string &label) : Widget((void *)CFBridgingRetain([NSButton new])) {
    [(__bridge NSButton *)view setTitle:[NSString stringWithUTF8String:label.c_str()]];
}

Button &Button::filled() { return *this; }

Button &Button::action(std::function<void(Widget &)> &&f) {
    auto v = (__bridge NSButton *)view;
    auto &callbacks = FlouiViewControllerImpl::callbacks;
    callbacks.push_back([[Callback alloc] initWithTarget:view Cb:f]);
    [v setTarget:callbacks.back()];
    [v setAction:@selector(invoke)];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    auto v = (__bridge NSButton *)view;
    [v setTarget:target];
    [v setAction:s];
    return *this;
}

Button &Button::foreground(uint32_t c) {
    auto v = (__bridge NSButton *)view;
    v.contentTintColor = col2nscol(c);
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(void *b) : Widget(b) {}

Text::Text(const std::string &s) : Widget((void *)CFBridgingRetain([NSTextField new])) {
    auto v = (__bridge NSTextField *)view;
    [v setBezeled:NO];
    [v setDrawsBackground:NO];
    [v setEditable:NO];
    [v setSelectable:NO];
    [v setStringValue:[NSString stringWithUTF8String:s.c_str()]];
}

Text &Text::foreground(uint32_t c) {
    auto v = (__bridge NSTextField *)view;
    [v setTextColor:col2nscol(c)];
    return *this;
}

Text &Text::center() {
    auto v = (__bridge NSTextField *)view;
    [v setAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(const std::string &s) {
    auto v = (__bridge NSTextField *)view;
    [v setStringValue:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

Text &Text::fontsize(int size) {
    auto v = (__bridge NSTextField *)view;
    [v setFont:[NSFont systemFontOfSize:size]];
    return *this;
}

Text &Text::bold() {
    auto v = (__bridge NSTextField *)view;
    [v setFont:[NSFont boldSystemFontOfSize:v.font.pointSize]];
    return *this;
}

DEFINE_STYLES(Text)

TextField::TextField(void *b) : Widget(b) {}

TextField::TextField() : Widget((void *)CFBridgingRetain([NSTextField new])) {}

TextField &TextField::foreground(uint32_t c) {
    auto v = (__bridge NSTextField *)view;
    [v setTextColor:col2nscol(c)];
    return *this;
}

TextField &TextField::center() {
    auto v = (__bridge NSTextField *)view;
    [v setAlignment:NSTextAlignmentCenter];
    return *this;
}

TextField &TextField::text(const std::string &s) {
    auto v = (__bridge NSTextField *)view;
    [v setStringValue:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

std::string TextField::text() const {
    auto v = (__bridge NSTextField *)view;
    return std::string([[v stringValue] UTF8String]);
}

TextField &TextField::fontsize(int size) {
    auto v = (__bridge NSTextField *)view;
    [v setFont:[NSFont systemFontOfSize:size]];
    return *this;
}

DEFINE_STYLES(TextField)

Spacer::Spacer(void *b) : Widget(b) {}

Spacer::Spacer() : Widget((void *)CFBridgingRetain([NSView new])) {}

DEFINE_STYLES(Spacer)

MainView::MainView(void *v) : Widget(v) {}

MainView::MainView(const FlouiViewController &, std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([NSStackView new])) {
    auto vc = FlouiViewControllerImpl::vc;
    auto v = (__bridge NSStackView *)view;
    [vc.view addSubview:v];
    v.frame = vc.view.frame;
    [v setOrientation:NSUserInterfaceLayoutOrientationVertical];
    [v setDistribution:NSStackViewDistributionFillEqually];
    [v setAlignment:NSLayoutAttributeCenterX];
    for (auto &e : l) {
        auto w = (__bridge NSView *)e.inner();
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
        [w.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:0].active = YES;
        [w.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:0].active = YES;
    }
}

DEFINE_STYLES(MainView)

VStack::VStack(void *v) : Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([NSStackView new])) {
    auto v = (__bridge NSStackView *)view;
    [v setOrientation:NSUserInterfaceLayoutOrientationVertical];
    [v setDistribution:NSStackViewDistributionFillEqually];
    [v setAlignment:NSLayoutAttributeCenterX];
    [v setSpacing:10];
    for (auto &e : l) {
        auto w = (__bridge NSView *)e.inner();
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

DEFINE_STYLES(VStack)

HStack::HStack(void *v) : Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l)
    : Widget((void *)CFBridgingRetain([NSStackView new])) {
    auto v = (__bridge NSStackView *)view;
    [v setOrientation:NSUserInterfaceLayoutOrientationHorizontal];
    [v setDistribution:NSStackViewDistributionFillEqually];
    [v setAlignment:NSLayoutAttributeCenterY];
    [v setSpacing:10];
    for (auto &e : l) {
        auto w = (__bridge NSView *)e.inner();
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

DEFINE_STYLES(HStack)