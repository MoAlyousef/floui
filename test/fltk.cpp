// testing desktop on linux. Checks there are no ODR issues etc

#include "../floui.hpp"

#include <FL/Fl.H>

#include <FL/Enumerations.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Double_Window.H>


using namespace floui;

struct FlouiViewControllerImpl {
    static inline Fl_Window *win = nullptr;
    static inline std::vector<std::function<void(Widget &)> *> callbacks = {};

    FlouiViewControllerImpl(Fl_Window *win, void *, void *) {
        FlouiViewControllerImpl::win = win;
        win->end();
        win->show();
        win->color(FL_WHITE);
    }
};

FlouiViewController::FlouiViewController(void *win, void *, void *)
    : impl(new FlouiViewControllerImpl((Fl_Window *)win, nullptr, nullptr)) {}

void FlouiViewController::handle_events(void *) { return; }

FlouiViewController::~FlouiViewController() {
    for (auto &elem : FlouiViewControllerImpl::callbacks) {
        delete elem;
    }
    delete impl;
}

void floui_log0(const char *s) {
    fputs(s, stderr);
    fputs("\n", stderr);
}

int floui_log(const char *s) {
    floui_log0(s);
    return 0;
}

using c = FlouiViewControllerImpl;

void widget_cb(Fl_Widget *w, void *data) {
    auto f = (std::function<void(Widget &)> *)data;
    auto e = Widget(w);
    (*f)(e);
}

#define DEFINE_STYLES(widget)                                                                      \
    widget &widget::background(uint32_t col) {                                                     \
        auto v = (Fl_Widget *)view;                                                                \
        v->color(col);                                                                             \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::id(const char *val) {                                                          \
        widget_map[val] = view;                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    widget &widget::size(int w, int h) {                                                           \
        auto v = (Fl_Widget *)view;                                                                \
        v->size(w, h);                                                                             \
        return *this;                                                                              \
    }

Widget::Widget(void *v) : view(v) {}

void *Widget::inner() const { return view; }

DEFINE_STYLES(Widget)

void *MainView_init() {
    auto pack = new Fl_Pack(0, 0, c::win->w(), c::win->h());
    c::win->add(pack);
    return pack;
}

MainView::MainView(void *v) : Widget(v) {}

MainView::MainView(const FlouiViewController &controller, std::initializer_list<Widget> l)
    : Widget(MainView_init()) {
    auto v = (Fl_Pack *)view;
    auto count = l.size();
    count = count ? count : c::win->h();
    auto h = v->h() / count;
    for (auto &w : l) {
        auto e = (Fl_Widget *)w.inner();
        if (e->h() == 0) {
            e->size(0, h);
        }
        v->add(e);
    }
}

DEFINE_STYLES(MainView)

Button::Button(void *v) : Widget(v) {}

Button::Button(const std::string &label) : Widget(new Fl_Button(0, 0, 0, 0, 0)) {
    auto v = ((Fl_Button *)view);
    v->copy_label(label.c_str());
    v->clear_visible_focus();
    v->box(FL_FLAT_BOX);
    v->down_box(FL_FLAT_BOX);
    v->color(FL_WHITE);
    v->down_color(FL_WHITE);
}

Button &Button::action(std::function<void(Widget &)> &&f) {
    auto v = ((Fl_Button *)view);
    c::callbacks.push_back(new std::function<void(Widget &)>(f));
    v->callback(widget_cb, c::callbacks.back());
    return *this;
}

DEFINE_STYLES(Button)

Text::Text(void *v) : Widget(v) {}

Text::Text(const std::string &label) : Widget(new Fl_Box(0, 0, 0, 0, 0)) {
    auto v = ((Fl_Box *)view);
    v->copy_label(label.c_str());
}

Text &Text::text(const std::string &label) {
    auto v = ((Fl_Box *)view);
    v->copy_label(label.c_str());
    return *this;
}

DEFINE_STYLES(Text)

static int val = 0;

MainView myview(const FlouiViewController &controller) {
    // clang-format off
    auto main_view = MainView(controller, {
        Button("Increment")
            .action([](auto) { 
                floui_log("incr"); 
                val += 1;
                Widget::from_id<Text>("mytext").text(std::to_string(val));
            }),
        Text("0")
            .id("mytext"), 
        Button("Decrement")
            .action([](auto) {
                floui_log("decr");
                val -= 1;
                Widget::from_id<Text>("mytext").text(std::to_string(val));
            })
    });
    // clang-format on
    return main_view;
}

int main() {
    auto win = new Fl_Double_Window(100, 100, 400, 600, "App");
    FlouiViewController controller(win);
    myview(controller);
    return Fl::run();
}