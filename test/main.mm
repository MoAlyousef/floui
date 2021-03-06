// This is just for testing on both iOS and macOS on CI, without storyboards and outside xcode.
// Don't structure your code this way!

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define FLOUI_IOS_WEBVIEW
#define FLOUI_IMPL
#else
#import <Cocoa/Cocoa.h>
@interface FlouiView : NSView
- (id)init;
- (id)initWithFrame:(NSRect)rect;
- (BOOL)isFlipped;
@end

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
#endif
#include "../floui.hpp"

using namespace floui;

@interface AppDelegate :
#if TARGET_OS_OSX
    NSObject <NSApplicationDelegate>
#else
    UIResponder <UIApplicationDelegate>
#endif
@end

#if TARGET_OS_IPHONE
@interface SceneDelegate : UIResponder <UIWindowSceneDelegate>
@property(strong, nonatomic) UIWindow *window;
@end
#endif

@interface ViewController :
#if TARGET_OS_OSX
    NSViewController
#else
    UIViewController
#endif
@end

@implementation AppDelegate
#if TARGET_OS_OSX
NSWindow *win;
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    win =
        [[NSWindow alloc] initWithContentRect:NSMakeRect(40, 40, 800, 600)
                                    styleMask:(NSWindowStyleMaskClosable | NSWindowStyleMaskTitled)
                                      backing:NSBackingStoreBuffered
                                        defer:NO];
    [win makeKeyAndOrderFront:nil];
    [win setContentViewController:[ViewController new]];
}
#else
- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    return YES;
}
#endif
@end

#if TARGET_OS_IPHONE
@implementation SceneDelegate
- (void)scene:(UIScene *)scene
    willConnectToSession:(UISceneSession *)session
                 options:(UISceneConnectionOptions *)connectionOptions {
    auto frame = [[UIScreen mainScreen] bounds];
    self.window = [[UIWindow alloc] initWithFrame:frame];
    [self.window setRootViewController:[ViewController new]];
    [self.window setWindowScene:(UIWindowScene *)scene];
    self.window.backgroundColor = UIColor.whiteColor;
    [self.window makeKeyAndVisible];
}
#pragma mark - UISceneSession lifecycle
- (UISceneConfiguration *)application:(UIApplication *)application
    configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession
                                   options:(UISceneConnectionOptions *)options {
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration"
                                          sessionRole:connectingSceneSession.role];
}
- (void)application:(UIApplication *)application
    didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
}
@end
#endif

@implementation ViewController
Button dec_btn("Decrement");
int val {0};
#if TARGET_OS_OSX
- (void)loadView {
    self.view = [[FlouiView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600)];
}
#endif

- (void)viewDidLoad {
    [super viewDidLoad];
#if TARGET_OS_OSX
    self.view.frame = NSMakeRect(0, 0, 600, 400);
#endif
        FlouiViewController controller((void *)CFBridgingRetain(self));
        MainView(controller, {
            Text("Counter")
                .size(600, 100)
                .center()
                .foreground(Color::White)
                .fontsize(30)
                .bold()
                .background(Color::Magenta),
            Spacer().size(0, 50),
            VStack({
                Button("Increment")
                    .action(self, @selector(increment))
                    .size(0, 40)
                    .filled()
                    .background(Color::rgb(0, 0, 255))
                    .foreground(Color::rgb(255, 255, 255, 255)),
                Text("0").id("mytext").size(0, 50),
                dec_btn.foreground(0xffffffff)
                        .size(0, 40)
                        .filled()
                        .background(0x0000ffff)
                        .action([=](Widget&) {
                            val--;
                            Widget::from_id<Text>("mytext").text(std::to_string(val));
                        }),
            }),
            Spacer()
    });
}
- (void)increment {
    val++;
    Widget::from_id<Text>("mytext").text(std::to_string(val));
}
@end

#if TARGET_OS_IPHONE
using argv_type = char *[];
#else
using argv_type = const char *[];
#endif

int main(int argc, argv_type argv) {
    NSString *appDelegateClassName;
    AppDelegate *delegate;
    @autoreleasepool {
        appDelegateClassName = NSStringFromClass([AppDelegate class]);
        delegate = [AppDelegate new];
    }
#if TARGET_OS_OSX
    [NSApplication sharedApplication].delegate = delegate;
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    return NSApplicationMain(argc, argv);
#else
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
#endif
}