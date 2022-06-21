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

#include <TargetConditionals.h>
#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <vector>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
using OSView = UIView;
using OSButton = UIButton;
using OSLabel = UILabel;
using OSTextField = UITextField;
using OSStackView = UIStackView;
using OSColor = UIColor;
using OSFont = UIFont;
using OSViewController = UIViewController;
#elif TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
using OSView = NSView;
using OSButton = NSButton;
using OSLabel = NSTextField;
using OSTextField = NSTextField;
using OSStackView = NSStackView;
using OSColor = NSColor;
using OSFont = NSFont;
using OSViewController = NSViewController;
#else
#error "Unsupported platform"
#endif // TARGET_OS_IPHONE

#define DECLARE_STYLES(widget)                                                                     \
    widget &background(OSColor *col);                                                              \
    widget &tint(OSColor *col);                                                                    \
    widget &id(const char *val);                                                                   \
    widget &size(int w, int h);

#if TARGET_OS_OSX
@interface FlouiView : NSView
- (id)init;
- (id)initWithFrame:(NSRect)rect;
- (BOOL)isFlipped;
@end
#endif

@interface Callback : NSObject {
    std::function<void()> *fn_;
}
- (id)initWithCb:(const std::function<void()> &)f;
- (void)invoke;
- (void)dealloc;
@end

class Widget {
  protected:
    static inline std::unordered_map<const char *, OSView *> widget_map{};
    OSView *view = nullptr;

  public:
    explicit Widget(OSView *v);
    explicit operator OSView *() const;
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Widget, T>>>
    static T from_id(const char *v) {
        return T{widget_map[v]};
    }
    DECLARE_STYLES(Widget)
};

class Button : public Widget {
    Callback *cb_ = nullptr;

  public:
    explicit Button(OSView *b);
    explicit Button(NSString *label);
    Button &filled();
    Button &action(std::function<void()> &&f);
    Button &action(::id target, SEL s);
    Button &foreground(OSColor *c);
    explicit operator OSButton *() const;
    DECLARE_STYLES(Button)
};

class Text : public Widget {
  public:
    explicit Text(OSView *b);
    explicit Text(NSString *s);
    Text &center();
    Text &text(NSString *s);
    Text &font(OSFont *f);
    Text &foreground(OSColor *c);
    explicit operator OSLabel *() const;
    DECLARE_STYLES(Text)
};

class TextField : public Widget {
  public:
    explicit TextField(OSView *b);
    TextField();
    TextField &center();
    TextField &text(NSString *s);
    NSString *text() const;
    TextField &font(OSFont *f);
    TextField &foreground(OSColor *c);
    explicit operator OSTextField *() const;
    DECLARE_STYLES(TextField)
};

class Spacer : public Widget {
  public:
    explicit Spacer(OSView *b);
    Spacer();
    DECLARE_STYLES(Spacer)
};

class MainView : public Widget {
  public:
    explicit MainView(OSView *);
    MainView(OSViewController *vc, std::initializer_list<Widget> l);
    MainView &spacing(int val);
    explicit operator OSView *() const;
    DECLARE_STYLES(MainView)
};

class VStack : public Widget {
  public:
    explicit VStack(OSView *v);
    explicit VStack(std::initializer_list<Widget> l);
    VStack &spacing(int val);
    explicit operator OSStackView *() const;
    DECLARE_STYLES(VStack)
};

class HStack : public Widget {
  public:
    explicit HStack(OSView *v);
    explicit HStack(std::initializer_list<Widget> l);
    HStack &spacing(int val);
    explicit operator OSStackView *() const;
    DECLARE_STYLES(HStack)
};

#ifdef FLOUI_IMPL

#if TARGET_OS_IPHONE

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(UIColor *col) {                                                     \
        view.backgroundColor = col;                                                                \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::tint(UIColor *col) {                                                           \
        view.tintColor = col;                                                                      \
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

Widget::Widget(UIView *v) : view(v) {}

Widget::operator OSView *() const { return view; }

DEFINE_STYLES(Widget)

Button::Button(OSView *b) : Widget(b) {}

Button::Button(NSString *label) : Widget([UIButton buttonWithType:UIButtonTypeCustom]) {
    [(UIButton *)view setTitle:label forState:UIControlStateNormal];
    [(UIButton *)view setTitleColor:UIColor.blueColor forState:UIControlStateNormal];
}

Button &Button::filled() {
    ((UIButton *)view).configuration = [UIButtonConfiguration filledButtonConfiguration];
    return *this;
}

Button &Button::action(std::function<void()> &&f) {
    cb_ = [[Callback alloc] initWithCb:f];
    [(UIButton *)view addTarget:cb_
                         action:@selector(invoke)
               forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    [(UIButton *)view addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button::operator UIButton *() const { return (UIButton *)view; }

Button &Button::foreground(UIColor *c) {
    [(UIButton *)view setTitleColor:c forState:UIControlStateNormal];
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(UIView *b) : Widget(b) {}

Text::Text(NSString *s) : Widget([UILabel new]) {
    [(UILabel *)view setText:s];
    [(UILabel *)view setTextColor:UIColor.blackColor];
}

Text &Text::foreground(UIColor *c) {
    [(UILabel *)view setTextColor:c];
    return *this;
}

Text &Text::center() {
    [(UILabel *)view setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(NSString *s) {
    [(UILabel *)view setText:s];
    return *this;
}

Text &Text::font(UIFont *font) {
    [(UILabel *)view setFont:font];
    return *this;
}

Text::operator UILabel *() const { return (UILabel *)view; }

DEFINE_STYLES(Text)

TextField::TextField(UIView *b) : Widget(b) {}

TextField::TextField() : Widget([UITextField new]) {
    [(UITextField *)view setTextColor:UIColor.blackColor];
}

TextField &TextField::foreground(UIColor *c) {
    [(UITextField *)view setTextColor:c];
    return *this;
}

TextField &TextField::center() {
    [(UITextField *)view setTextAlignment:NSTextAlignmentCenter];
    return *this;
}

TextField &TextField::text(NSString *s) {
    [(UITextField *)view setText:s];
    return *this;
}

TextField &TextField::font(UIFont *font) {
    [(UITextField *)view setFont:font];
    return *this;
}

TextField::operator UITextField *() const { return (UITextField *)view; }

DEFINE_STYLES(TextField)

Spacer::Spacer(UIView *b) : Widget(b) {}

Spacer::Spacer() : Widget([UIView new]) {}

DEFINE_STYLES(Spacer)

MainView::MainView(UIView *v) : Widget(v) {}

MainView::MainView(UIViewController *vc, std::initializer_list<Widget> l)
    : Widget([UIStackView new]) {
    [vc.view addSubview:view];
    view.frame = vc.view.frame;
    [(UIStackView *)view setAxis:UILayoutConstraintAxisVertical];
    [(UIStackView *)view setDistribution:UIStackViewDistributionFillEqually];
    [(UIStackView *)view setAlignment:UIStackViewAlignmentCenter];
    [(UIStackView *)view setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [(UIStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
        [w.leadingAnchor constraintEqualToAnchor:view.leadingAnchor constant:0].active = YES;
        [w.trailingAnchor constraintEqualToAnchor:view.trailingAnchor constant:0].active = YES;
    }
}

MainView &MainView::spacing(int val) {
    [(UIStackView *)view setSpacing:val];
    return *this;
}

MainView::operator UIView *() const { return view; }

DEFINE_STYLES(MainView)

VStack::VStack(UIView *v) : Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l) : Widget([UIStackView new]) {
    [(UIStackView *)view setAxis:UILayoutConstraintAxisVertical];
    [(UIStackView *)view setDistribution:UIStackViewDistributionFillEqually];
    [(UIStackView *)view setAlignment:UIStackViewAlignmentCenter];
    [(UIStackView *)view setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [(UIStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

VStack &VStack::spacing(int val) {
    [(UIStackView *)view setSpacing:val];
    return *this;
}

VStack::operator UIStackView *() const { return (UIStackView *)view; }

DEFINE_STYLES(VStack)

HStack::HStack(UIView *v) : Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l) : Widget([UIStackView new]) {
    [(UIStackView *)view setAxis:UILayoutConstraintAxisHorizontal];
    [(UIStackView *)view setDistribution:UIStackViewDistributionFillEqually];
    [(UIStackView *)view setAlignment:UIStackViewAlignmentCenter];
    [(UIStackView *)view setSpacing:10];
    for (auto e : l) {
        auto w = (UIView *)e;
        [(UIStackView *)view addArrangedSubview:w];
        if (w.frame.size.width != 0)
            [w.widthAnchor constraintEqualToConstant:w.frame.size.width].active = YES;
        if (w.frame.size.height != 0)
            [w.heightAnchor constraintEqualToConstant:w.frame.size.height].active = YES;
    }
}

HStack &HStack::spacing(int val) {
    [(UIStackView *)view setSpacing:val];
    return *this;
}

HStack::operator UIStackView *() const { return (UIStackView *)view; }

DEFINE_STYLES(HStack)

#else
// cococa stuff
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

Widget::operator OSView *() const { return view; }

DEFINE_STYLES(Widget)

Button::Button(OSView *b) : Widget(b) {}

Button::Button(NSString *label) : Widget([NSButton new]) { [(NSButton *)view setTitle:label]; }

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

Button::operator OSButton *() const { return (OSButton *)view; }

Button &Button::foreground(OSColor *c) {
    ((NSButton *)view).contentTintColor = c;
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(OSView *b) : Widget(b) {}

Text::Text(NSString *s) : Widget([NSTextField new]) {
    [(NSTextField *)view setBezeled:NO];
    [(NSTextField *)view setDrawsBackground:NO];
    [(NSTextField *)view setEditable:NO];
    [(NSTextField *)view setSelectable:NO];
    [(NSTextField *)view setStringValue:s];
}

Text &Text::foreground(OSColor *c) {
    [(NSTextField *)view setTextColor:c];
    return *this;
}

Text &Text::center() {
    [(NSTextField *)view setAlignment:NSTextAlignmentCenter];
    return *this;
}

Text &Text::text(NSString *s) {
    [(NSTextField *)view setStringValue:s];
    return *this;
}

Text &Text::font(OSFont *font) {
    [(NSTextField *)view setFont:font];
    return *this;
}

Text::operator OSLabel *() const { return (NSTextField *)view; }

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

TextField &TextField::text(NSString *s) {
    [(NSTextField *)view setStringValue:s];
    return *this;
}

NSString *TextField::text() const {
    return [(NSTextField *)view stringValue];
}

TextField &TextField::font(OSFont *font) {
    [(NSTextField *)view setFont:font];
    return *this;
}

TextField::operator NSTextField *() const { return (NSTextField *)view; }

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
MainView::operator OSView *() const { return view; }

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

VStack::operator OSStackView *() const { return (NSStackView *)view; }

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

HStack::operator OSStackView *() const { return (NSStackView *)view; }

DEFINE_STYLES(HStack)

#endif // TARGET_OS_IPHONE

#endif // FLOUI_IMPL

#endif // __FLOUI_H__
