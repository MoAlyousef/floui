#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(OSColor *col) {                                                     \
        view.layer.backgroundColor = col.CGColor;                                                  \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::tint(OSColor *col) {                                                           \
        (void)col;                                                                                 \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::id(const char *val) {                                                          \
        widget_map[val] = view;                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::size(int w, int h) {                                                           \
        auto frame = view.frame;                                                                   \
        frame.size.width = w;                                                                      \
        frame.size.height = h;                                                                     \
        view.frame = frame;                                                                        \
        return *this;                                                                              \
    }

@implementation FlouiView
- (id)init {
    self = [super init];
    return self;
}
- (id)initWithFrame:(NSRect)rect {
    self = [super initWithFrame:rect];
    return self;
}
- (BOOL)isFlipped {
    return YES;
}
@end

@implementation Callback
- (id)initWithCb:(const std::function<void()> &)f {
    self = [super init];
    fn_ = new std::function<void()>(f);
    return self;
}
- (void)invoke {
    (*fn_)();
}
- (void)dealloc {
    delete fn_;
    fn_ = nil;
}
@end

Widget::Widget(OSView *v) : view(v) { view.wantsLayer = YES; }


DEFINE_STYLES(Widget)

Button::Button(OSView *b) : Widget(b) {}

Button::Button(const std::string &label) : Widget([NSButton new]) { [(NSButton *)view setTitle:[NSString stringWithUTF8String:label.c_str()]]; }

Button &Button::filled() { return *this; }

Button &Button::action(std::function<void()> &&f) {
    cb_ = [[Callback alloc] initWithCb:f];
    [(NSButton *)view setTarget:cb_];
    [(NSButton *)view setAction:@selector(invoke)];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    [(NSButton *)view setTarget:target];
    [(NSButton *)view setAction:s];
    return *this;
}


Button &Button::foreground(OSColor *c) {
    ((NSButton *)view).contentTintColor = c;
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(OSView *b) : Widget(b) {}

Text::Text(const std::string &s) : Widget([NSTextField new]) {
    [(NSTextField *)view setBezeled:NO];
    [(NSTextField *)view setDrawsBackground:NO];
    [(NSTextField *)view setEditable:NO];
    [(NSTextField *)view setSelectable:NO];
    [(NSTextField *)view setStringValue:[NSString stringWithUTF8String:s.c_str()]];
}

Text &Text::foreground(OSColor *c) {
    [(NSTextField *)view setTextColor:c];
    return *this;
}

Text &Text::center() {
    [(NSTextField *)view setAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(const std::string &s) {
    [(NSTextField *)view setStringValue:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

Text &Text::font(OSFont *font) {
    [(NSTextField *)view setFont:font];
    return *this;
}


DEFINE_STYLES(Text)

TextField::TextField(OSView *b) : Widget(b) {}

TextField::TextField() : Widget([NSTextField new]) {}

TextField &TextField::foreground(OSColor *c) {
    [(NSTextField *)view setTextColor:c];
    return *this;
}

TextField &TextField::center() {
    [(NSTextField *)view setAlignment:NSTextAlignmentCenter];
    return *this;
}

TextField &TextField::text(const std::string &s) {
    [(NSTextField *)view setStringValue:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

std::string TextField::text() const {
    return std::string([[(NSTextField *)view stringValue] UTF8String]);
}

TextField &TextField::font(OSFont *font) {
    [(NSTextField *)view setFont:font];
    return *this;
}


DEFINE_STYLES(TextField)

Spacer::Spacer(OSView *b) : Widget(b) {}

Spacer::Spacer() : Widget([NSView new]) {}

DEFINE_STYLES(Spacer)

MainView::MainView(OSView *v) : Widget(v) {}

MainView::MainView(OSViewController *vc, std::initializer_list<Widget> l)
    : Widget([NSStackView new]) {
    [vc.view addSubview:view];
    view.frame = vc.view.frame;
    [(NSStackView *)view setOrientation:NSUserInterfaceLayoutOrientationVertical];
    [(NSStackView *)view setDistribution:NSStackViewDistributionFillEqually];
    [(NSStackView *)view setAlignment:NSLayoutAttributeCenterX];
    for (auto e : l) {
        auto w = (NSView *)e;
        [(NSStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
        [w.leadingAnchor constraintEqualToAnchor:view.leadingAnchor constant:0].active = YES;
        [w.trailingAnchor constraintEqualToAnchor:view.trailingAnchor constant:0].active = YES;
    }
}

DEFINE_STYLES(MainView)

VStack::VStack(OSView *v) : Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l) : Widget([NSStackView new]) {
    [(NSStackView *)view setOrientation:NSUserInterfaceLayoutOrientationVertical];
    [(NSStackView *)view setDistribution:NSStackViewDistributionFillEqually];
    [(NSStackView *)view setAlignment:NSLayoutAttributeCenterX];
    [(NSStackView *)view setSpacing:10];
    for (auto e : l) {
        auto w = (NSView *)e;
        [(NSStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}


DEFINE_STYLES(VStack)

HStack::HStack(OSView *v) : Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l) : Widget([NSStackView new]) {
    [(NSStackView *)view setOrientation:NSUserInterfaceLayoutOrientationHorizontal];
    [(NSStackView *)view setDistribution:NSStackViewDistributionFillEqually];
    [(NSStackView *)view setAlignment:NSLayoutAttributeCenterY];
    [(NSStackView *)view setSpacing:10];
    for (auto e : l) {
        auto w = (NSView *)e;
        [(NSStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}


DEFINE_STYLES(HStack)