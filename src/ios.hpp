#include "header.hpp"

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        view.backgroundColor = col2uicol(col);                                                     \
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

@implementation Callback
- (id)initWithTarget:(void *)target Cb:(const std::function<void(Widget &)> &)f {
    self = [super init];
    fn_ = new std::function<void(Widget &)>(f);
    target_ = target;
    return self;
}
- (void)invoke {
    auto w = Widget(target);
    (*fn_)(w);
}
- (void)dealloc {
    delete fn_;
    fn_ = nil;
}
@end

UIColor *col2uicol(uint32_t col) {
    auto r = ((col >> 24) & 0xFF) / 255.0;
    auto g = ((col >> 16) & 0xFF) / 255.0;
    auto b = ((hexValue >> 8) & 0xFF) / 255.0;
    auto a = ((hexValue) & 0xFF) / 255.0;
    return [UIColor colorWithRed:r green:g blue:b alpha:a];
}

Widget::Widget(void *v) : view(v) {}

DEFINE_STYLES(Widget)

Button::Button(void *b) : Widget(b) {}

Button::Button(const std::string &label) : Widget([UIButton buttonWithType:UIButtonTypeCustom]) {
    auto v = (UIButton *)view;
    [v setTitle:label forState:UIControlStateNormal];
    [v setTitleColor:UIColor.blueColor forState:UIControlStateNormal];
}

Button &Button::filled() {
    ((UIButton *)view).configuration = [UIButtonConfiguration filledButtonConfiguration];
    return *this;
}

Button &Button::action(std::function<void(Widget &)> &&f) {
    auto v = (UIButton *)view;
    cb_ = [[Callback alloc] initWithTarget:view Cb:f];
    [v addTarget:cb_
                         action:@selector(invoke)
               forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    auto v = (UIButton *)view;
    [v addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::foreground(uint32_t c) {
    auto v = (UIButton *)view;
    [v setTitleColor:col2uicol(c) forState:UIControlStateNormal];
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(void *b) : Widget(b) {}

Text::Text(const std::string &s) : Widget([UILabel new]) {
    auto v = (UILabel *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    [v setTextColor:UIColor.blackColor];
}

Text &Text::foreground(uint32_t c) {
    auto v = (UILabel *)view;
    [v setTextColor:col2uicol(c)];
    return *this;
}

Text &Text::center() {
    auto v = (UILabel *)view;
    [v setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(const std::string &s) {
    auto v = (UILabel *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

Text &Text::fontsize(int size) {
    auto v = (UILabel *)view;
    [v setFont:[UIFont systemFontOfSize:size]];
    return *this;
}

DEFINE_STYLES(Text)

TextField::TextField(void *b) : Widget(b) {}

TextField::TextField() : Widget([UITextField new]) {
    auto v = (UITextField *)view;
    [v setTextColor:UIColor.blackColor];
}

TextField &TextField::foreground(uint32_t c) {
    auto v = (UITextField *)view;
    [v setTextColor:col2uicol(c)];
    return *this;
}

TextField &TextField::center() {
    auto v = (UITextField *)view;
    [v setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

TextField &TextField::text(const std::string &s) {
    auto v = (UITextField *)view;
    [v setText:[NSString stringWithUTF8String:s.c_str()]];
    return *this;
}

std::string TextField::text() const {
    return std::string([[(NSTextField *)view stringValue] UTF8String]);
}

TextField &TextField::fontsize(int size) {
    auto v = (UITextField *)view;
    [v setFont:[UIFont systemFontOfSize:size]];
    return *this;
}

DEFINE_STYLES(TextField)

Spacer::Spacer(void *b) : Widget(b) {}

Spacer::Spacer() : Widget([UIView new]) {}

DEFINE_STYLES(Spacer)

MainView::MainView(void *v) : Widget(v) {}

MainView::MainView(void *fvc, std::initializer_list<Widget> l)
    : Widget([UIStackView new]) {
    auto v = (UIStackView*)view;
    auto vc = (UIViewController *)fvc;
    [vc.view addSubview:view];
    view.frame = vc.view.frame;
    [v setAxis:UILayoutConstraintAxisVertical];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
        [w.leadingAnchor constraintEqualToAnchor:view.leadingAnchor constant:0].active = YES;
        [w.trailingAnchor constraintEqualToAnchor:view.trailingAnchor constant:0].active = YES;
    }
}

MainView &MainView::spacing(int val) {
    auto v = (UIStackView*)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(MainView)

VStack::VStack(void *v) : Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l) : Widget([UIStackView new]) {
    auto v = (UIStackView*)view;
    [v setAxis:UILayoutConstraintAxisVertical];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

VStack &VStack::spacing(int val) {
    auto v = (UIStackView*)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(VStack)

HStack::HStack(void *v) : Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l) : Widget([UIStackView new]) {
    auto v = (UIStackView*)view;
    [v setAxis:UILayoutConstraintAxisHorizontal];
    [v setDistribution:UIStackViewDistributionFillEqually];
    [v setAlignment:UIStackViewAlignmentCenter];
    [v setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [v addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

HStack &HStack::spacing(int val) {
    auto v = (UIStackView*)view;
    [v setSpacing:val];
    return *this;
}

DEFINE_STYLES(HStack)