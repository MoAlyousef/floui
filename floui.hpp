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

#import <UIKit/UIKit.h>
#include <functional>
#include <initializer_list>
#include <unordered_map>
#include <vector>

#define DECLARE_STYLES(widget)\
widget &background(UIColor *col);\
widget &tint(UIColor *col);\
widget &id(const char *val);

@interface Callback: NSObject {
    std::function<void()> *fn_;
}
-(id)initWithCb:(const std::function<void()> &)f;
-(void)invoke;
-(void)dealloc;
@end

class Widget {
protected:
    static inline std::unordered_map<const char *, UIView *> widget_map {};
    UIView *view = nullptr;
public:
    explicit Widget(UIView *v);
    explicit operator UIView*() const;
    template<typename T, typename = std::enable_if_t<std::is_base_of_v<Widget, T>>>
    static T from_id(const char *v) {
        return T {widget_map[v]};
    }
    DECLARE_STYLES(Widget)
};

class Button: public Widget {
    Callback *cb_ = nullptr;
public:
    explicit Button(UIView *b);
    explicit Button(NSString *label);
    Button &action(std::function<void()> &&f);
    Button &action(::id target, SEL s);
    Button &config(UIButtonConfiguration *conf);
    Button &foreground(UIColor *c);
    explicit operator UIButton *() const;
    DECLARE_STYLES(Button)
};

class Text: public Widget {
public:
    explicit Text(UIView *b);
    explicit Text(NSString *s);
    Text &center();
    Text &text(NSString *s);
    Text &font(UIFont *f);
    Text &foreground(UIColor *c);
    explicit operator UILabel *() const;
    DECLARE_STYLES(Text)
};

class Spacer: public Widget {
public:
    explicit Spacer(UIView *b);
    Spacer();
    DECLARE_STYLES(Spacer)
};

class MainView: public Widget {
    std::vector<UIView *> children = {};
    int margins = 10;
public:
    explicit MainView(UIView *);
    MainView(UIViewController *vc, std::initializer_list<Widget> l);
    explicit operator UIView *() const;
    DECLARE_STYLES(MainView)
};

class VStack: public Widget {
public:
    explicit VStack(UIView *v);
    explicit VStack(std::initializer_list<Widget> l);
    explicit operator UIStackView *() const;
    DECLARE_STYLES(VStack)
};

class HStack: public Widget {
public:
    explicit HStack(UIView *v);
    explicit HStack(std::initializer_list<Widget> l);
    explicit operator UIStackView *() const;
    DECLARE_STYLES(HStack)
};

#ifdef FLOUI_IMPL

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

Widget::operator UIView*()  const {
    return view;
}

DEFINE_STYLES(Widget)

Button::Button(UIView *b): Widget(b) {}

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
    auto frame = [[UIScreen mainScreen]bounds];
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

#endif

#endif /* floui_h */
