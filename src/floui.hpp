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

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

#define DECLARE_STYLES(widget)                                                                     \
    widget &background(uint32_t col);                                                              \
    widget &id(const char *val);                                                                   \
    widget &size(int w, int h);

void floui_log(const char *s);

#ifndef __APPLE__
struct FlouiViewControllerImpl;

struct FlouiViewController {
    FlouiViewControllerImpl *impl;
    FlouiViewController(void *, void *, void *);
};

void floui_jni_handle_events(void *view);
#else // Android
#ifdef __OBJC__
#include <CoreFoundation/CoreFoundation.h>
#include <TargetConditionals.h>
#if TARGET_OS_OSX
@interface FlouiView : NSView
- (id)init;
- (id)initWithFrame:(NSRect)rect;
- (BOOL)isFlipped;
@end
#endif

@interface Callback : NSObject {
    std::function<void()> *fn_;
    void *target_;
}
- (id)initWithTarget:(void *)target Cb:(const std::function<void(Widget &)> &)f;
- (void)invoke;
- (void)dealloc;
@end
#endif // __OBJC__
#endif // __APPLE__

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
    static void *init();

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

#ifndef __APPLE__
// Android stuff
#include <jni.h>

#else

#if TARGET_OS_IPHONE
// ios stuff
#import <UIKit/UIKit.h>

#else
// cococa stuff
#import <Cocoa/Cocoa.h>

#endif // TARGET_OS_IPHONE

#endif // __APPLE__

#endif // FLOUI_IMPL

#endif // __FLOUI_H__
