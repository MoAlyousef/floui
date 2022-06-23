
#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

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