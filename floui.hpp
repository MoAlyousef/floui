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

#ifndef floui_h
#define floui_h


#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#define OSVIEW UIView
#define OSBUTTON UIButton
#define OSLABEL UILabel
#define OSSTACKVIEW UIStackView
#define OSCOLOR UIColor
#define OSFONT UIFont
#define OSVIEWCONTROLLER UIViewController
#elif TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
#define OSVIEW NSView
#define OSBUTTON NSButton
#define OSLABEL NSTextField
#define OSSTACKVIEW NSStackView
#define OSCOLOR NSColor
#define OSFONT NSFont
#define OSVIEWCONTROLLER NSViewController
#else
#error "Unsupported platform"
#endif

#define DECLARE_STYLES(widget)\
widget &background(OSCOLOR *col);\
widget &tint(OSCOLOR *col);\
widget &id(const char *val);

#if TARGET_OS_OSX
@interface MyView: NSView
-(id)init;
-(id)initWithFrame:(NSRect)rect;
-(BOOL)isFlipped;
@end
#endif

@interface Callback: NSObject {
    std::function<void()> *fn_;
}
-(id)initWithCb:(const std::function<void()> &)f;
-(void)invoke;
-(void)dealloc;
@end

class Widget {
protected:
    static inline std::unordered_map<const char *, OSVIEW *> widget_map {};
    OSVIEW *view = nullptr;
public:
    explicit Widget(OSVIEW *v);
    explicit operator OSVIEW*() const;
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<Widget, T>>>
    static T from_id(const char *v) {
        return T {widget_map[v]};
    }
    DECLARE_STYLES(Widget)
};

class Button: public Widget {
    Callback *cb_ = nullptr;
public:
    explicit Button(OSVIEW *b);
    explicit Button(NSString *label);
    Button &action(std::function<void()> &&f);
    Button &action(::id target, SEL s);
#if TARGET_OS_IPHONE
    Button &config(UIButtonConfiguration *conf);
#endif
    Button &foreground(OSCOLOR *c);
    explicit operator OSBUTTON *() const;
    DECLARE_STYLES(Button)
};

class Text: public Widget {
public:
    explicit Text(OSVIEW *b);
    explicit Text(NSString *s);
    Text &center();
    Text &text(NSString *s);
    Text &font(OSFONT *f);
    Text &foreground(OSCOLOR *c);
    explicit operator OSLABEL *() const;
    DECLARE_STYLES(Text)
};

class Spacer: public Widget {
public:
    explicit Spacer(OSVIEW *b);
    Spacer();
    DECLARE_STYLES(Spacer)
};

class MainView: public Widget {
    std::vector<OSVIEW *> children = {};
    int margins = 10;
public:
    explicit MainView(OSVIEW *);
    MainView(OSVIEWCONTROLLER *vc, std::initializer_list<Widget> l);
    explicit operator OSVIEW *() const;
    DECLARE_STYLES(MainView)
};

class VStack: public Widget {
public:
    explicit VStack(OSVIEW *v);
    explicit VStack(std::initializer_list<Widget> l);
    explicit operator OSSTACKVIEW *() const;
    DECLARE_STYLES(VStack)
};

class HStack: public Widget {
public:
    explicit HStack(OSVIEW *v);
    explicit HStack(std::initializer_list<Widget> l);
    explicit operator OSSTACKVIEW *() const;
    DECLARE_STYLES(HStack)
};

#ifdef FLOUI_IMPL

#if TARGET_OS_IPHONE

#define DEFINE_STYLES(widget)\
widget &widget::background(UIColor *col) { view.backgroundColor = col; return *this; } \
widget &widget::tint(UIColor *col) { view.tintColor = col; return *this; } \
widget &widget::id(const char *val) { widget_map[val] = view; return *this; }

@implementation Callback
-(id)initWithCb:(const std::function<void()> &)f {
    self = [super init];
    fn_ = new std::function<void()>(f);
    return self;
}
-(void)invoke {
    (*fn_)();
}
-(void)dealloc {
    delete fn_;
    fn_ = nil;
}
@end

Widget::Widget(UIView *v): view(v) {}

Widget::operator OSVIEW*()  const {
    return view;
}

DEFINE_STYLES(Widget)

Button::Button(OSVIEW *b): Widget(b) {}

Button::Button(NSString *label): Widget([UIButton buttonWithType:UIButtonTypeCustom]) {
    [(UIButton *)view setTitle:label forState:UIControlStateNormal];
    [(UIButton *)view setTitleColor:UIColor.blueColor forState:UIControlStateNormal];
}

Button &Button::config(UIButtonConfiguration *conf) {
    ((UIButton *)view).configuration = [UIButtonConfiguration filledButtonConfiguration];
    return *this;
}

Button &Button::action(std::function<void()> &&f) {
    cb_ = [[Callback alloc]initWithCb:f];
    [(UIButton *)view addTarget:cb_ action:@selector(invoke) forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    [(UIButton *)view addTarget:target action:s forControlEvents:UIControlEventTouchUpInside];
    return *this;
}

Button::operator UIButton *()  const {
    return (UIButton *)view;
}

Button &Button::foreground(UIColor *c) {
    [(UIButton *)view setTitleColor:c forState:UIControlStateNormal];
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(UIView *b): Widget(b) {}

Text::Text(NSString *s): Widget([UILabel new]) {
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

Text::operator UILabel *()  const {
    return (UILabel *)view;
}

DEFINE_STYLES(Text)

Spacer::Spacer(UIView *b): Widget(b) {}

Spacer::Spacer(): Widget([UIView new]) {}

DEFINE_STYLES(Spacer)

MainView::MainView(UIView *v): Widget(v) {}

MainView::MainView(UIViewController *vc, std::initializer_list<Widget> l): Widget([UIView new]) {
    [vc.view addSubview:view];
    view.frame = vc.view.frame;
    for (auto e: l) {
        children.push_back((UIView *)e);
    }
    auto frame = view.frame;
    auto x = 0;
    auto y = 0;
    auto count = children.size();
    auto w = frame.size.width;
    auto h = (frame.size.height) / count;
    auto i = 0;
    for (auto e: children) {
        [view addSubview:e];
        e.frame = CGRectMake(x, y + ((h+margins) * i), w, h);
        i += 1;
    }
}

MainView::operator UIView *()  const {
    return view;
}

DEFINE_STYLES(MainView)

VStack::VStack(UIView *v): Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l): Widget([[UIStackView alloc]initWithFrame:[[UIScreen mainScreen]bounds]]) {
    [(UIStackView *)view setAxis:UILayoutConstraintAxisVertical];
    [(UIStackView *)view setDistribution:UIStackViewDistributionFillEqually];
    [(UIStackView *)view setAlignment:UIStackViewAlignmentCenter];
    [(UIStackView *)view setSpacing:10];
    for (auto e: l) {
        [(UIStackView *)view addArrangedSubview:(UIView *)e];
    }
}

VStack::operator UIStackView *()  const {
    return (UIStackView *)view;
}

DEFINE_STYLES(VStack)

HStack::HStack(UIView *v): Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l): Widget([[UIStackView alloc]initWithFrame:[[UIScreen mainScreen]bounds]]) {
    [(UIStackView *)view setAxis:UILayoutConstraintAxisHorizontal];
    [(UIStackView *)view setDistribution:UIStackViewDistributionFillEqually];
    [(UIStackView *)view setAlignment:UIStackViewAlignmentCenter];
    [(UIStackView *)view setSpacing:10];
    for (auto e: l) {
        [(UIStackView *)view addArrangedSubview:(UIView *)e];
    }
}

HStack::operator UIStackView *()  const {
    return (UIStackView *)view;
}

DEFINE_STYLES(HStack)

#else
// cococa stuff
#define DEFINE_STYLES(widget)\
widget &widget::background(OSCOLOR *col) { view.layer.backgroundColor = col.CGColor; return *this; } \
widget &widget::tint(OSCOLOR *col) { (void)col; return *this; } \
widget &widget::id(const char *val) { widget_map[val] = view; return *this; }

@implementation MyView
-(id)init {
    self = [super init];
    return self;
}
-(id)initWithFrame:(NSRect)rect {
    self = [super initWithFrame:rect];
    return self;
}
-(BOOL)isFlipped {
    return YES;
}
@end

@implementation Callback
-(id)initWithCb:(const std::function<void()> &)f {
    self = [super init];
    fn_ = new std::function<void()>(f);
    return self;
}
-(void)invoke {
    (*fn_)();
}
-(void)dealloc {
    delete fn_;
    fn_ = nil;
}
@end

Widget::Widget(OSVIEW *v): view(v) {
    view.wantsLayer = YES;
}

Widget::operator OSVIEW*()  const {
    return view;
}

DEFINE_STYLES(Widget)

Button::Button(OSVIEW *b): Widget(b) {}

Button::Button(NSString *label): Widget([NSButton new]) {
    [(NSButton *)view setTitle:label];
}

Button &Button::action(std::function<void()> &&f) {
    cb_ = [[Callback alloc]initWithCb:f];
    [(NSButton *)view setTarget:cb_];
    [(NSButton *)view setAction:@selector(invoke)];
    return *this;
}

Button &Button::action(::id target, SEL s) {
    [(NSButton *)view setTarget:target];
    [(NSButton *)view setAction:s];
    return *this;
}

Button::operator OSBUTTON *()  const {
    return (OSBUTTON *)view;
}

Button &Button::foreground(OSCOLOR *c) {
    ((NSButton *)view).contentTintColor = c;
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(OSVIEW *b): Widget(b) {}

Text::Text(NSString *s): Widget([NSTextField new]) {
    [(NSTextField *)view setBezeled:NO];
    [(NSTextField *)view setDrawsBackground:NO];
    [(NSTextField *)view setEditable:NO];
    [(NSTextField *)view setSelectable:NO];
    [(NSTextField *)view setStringValue:s];
}

Text &Text::foreground(OSCOLOR *c) {
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

Text &Text::font(OSFONT *font) {
    [(NSTextField *)view setFont:font];
    return *this;
}

Text::operator OSLABEL *()  const {
    return (NSTextField *)view;
}

DEFINE_STYLES(Text)

Spacer::Spacer(OSVIEW *b): Widget(b) {}

Spacer::Spacer(): Widget([NSView new]) {}

DEFINE_STYLES(Spacer)

MainView::MainView(OSVIEW *v): Widget(v) {}

MainView::MainView(OSVIEWCONTROLLER *vc, std::initializer_list<Widget> l): Widget([MyView new]) {    [vc.view addSubview:view];
    view.frame = vc.view.frame;
    for (auto e: l) {
        children.push_back((NSView *)e);
    }
    auto frame = view.frame;
    auto x = 0;
    auto y = 0;
    auto count = children.size();
    auto w = frame.size.width;
    auto h = (frame.size.height) / count;
    auto i = 0;
    for (auto e: children) {
        [view addSubview:e];
        e.frame = CGRectMake(x, y + ((h+margins) * i), w, h);
        i += 1;
    }
}

MainView::operator OSVIEW *()  const {
    return view;
}

DEFINE_STYLES(MainView)

VStack::VStack(OSVIEW *v): Widget(v) {}

VStack::VStack(std::initializer_list<Widget> l): Widget([NSStackView new]) {
    [(NSStackView *)view setOrientation:NSUserInterfaceLayoutOrientationVertical];
    [(NSStackView *)view setDistribution:NSStackViewDistributionFillEqually];
    [(NSStackView *)view setAlignment:NSLayoutAttributeCenterX];
    [(NSStackView *)view setSpacing:10];
    for (auto e: l) {
        [(NSStackView *)view addArrangedSubview:(NSView *)e];
    }
}

VStack::operator OSSTACKVIEW *()  const {
    return (NSStackView *)view;
}

DEFINE_STYLES(VStack)

HStack::HStack(OSVIEW *v): Widget(v) {}

HStack::HStack(std::initializer_list<Widget> l): Widget([NSStackView new]) {
    [(NSStackView *)view setOrientation:NSUserInterfaceLayoutOrientationHorizontal];
    [(NSStackView *)view setDistribution:NSStackViewDistributionFillEqually];
    [(NSStackView *)view setAlignment:NSLayoutAttributeCenterY];
    [(NSStackView *)view setSpacing:10];
    for (auto e: l) {
        [(NSStackView *)view addArrangedSubview:(NSView *)e];
    }
}

HStack::operator OSSTACKVIEW *()  const {
    return (NSStackView *)view;
}

DEFINE_STYLES(HStack)

#endif

#endif

#endif /* floui_h */
