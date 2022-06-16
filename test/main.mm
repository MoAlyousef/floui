// This is just for testing on both iOS and macOS on CI, without storyboards and outside xcode.
// Don't structure your code this way!

#define FLOUI_IMPL
#include "../floui.hpp"

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

#if TARGET_OS_IPHONE
#define FONT UIFont
#define COLOR UIColor
#else
#define FONT NSFont
#define COLOR NSColor
#endif

@implementation ViewController
Button dec_btn(@"Decrement");
int val;
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
    MainView(self, {
                       Text(@"Counter")
                           .center()
                           .foreground(COLOR.whiteColor)
                           .font([FONT boldSystemFontOfSize:30])
                           .background(COLOR.purpleColor),
                       Spacer(),
                       VStack({
                           Button(@"Increment")
                               .action(self, @selector(increment))
                               .filled()
                               .background(COLOR.blueColor)
                               .foreground(COLOR.whiteColor),
                               Text(@"0").id("mytext"),
                               dec_btn.foreground(COLOR.whiteColor)
                                   .filled()
                                   .background(COLOR.blueColor)
                                   .action([=] {
                                       val--;
                                       Widget::from_id<Text>("mytext").text(
                                           [NSString stringWithFormat:@"%d", val]);
                                   }),
                       }),
                       Spacer(),
                       Spacer(),
                   });
}
- (void)increment {
    val++;
    Widget::from_id<Text>("mytext").text([NSString stringWithFormat:@"%d", val]);
}
@end

#if TARGET_OS_IPHONE
#define ARGV_T char
#else
#define ARGV_T const char
#endif

int main(int argc, ARGV_T *argv[]) {
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