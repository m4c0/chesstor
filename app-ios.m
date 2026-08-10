#include "pch.h"
#include "mtl.h"

#import <UIKit/UIKit.h>

@interface POCViewController : UIViewController
@end
@implementation POCViewController
- (BOOL)canBecomeFirstResponder {
  return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *) touches withEvent:(UIEvent *) event {
  CGPoint p = [[touches anyObject] locationInView:[self view]];
  glu_mouse_move(p.x, p.y);
  glu_mouse_down(p.x, p.y);
}
@end

@interface POCWindowSceneDelegate : NSObject<UIWindowSceneDelegate>
@property (nonatomic, strong) UIWindow * window;
@end
@implementation POCWindowSceneDelegate
- (void) scene:(UIScene *) scene willConnectToSession:(UISceneSession *) session options:(UISceneConnectionOptions *) connectionOptions
{
  MTKView * view = [MTKView new];
  view.device = MTLCreateSystemDefaultDevice();
  view.clearColor = MTLClearColorMake(0.01, 0.02, 0.03, 1.0);
  view.delegate = [POCViewDelegate newWithDevice:view.device];

  POCViewController * vc = [POCViewController new];
  vc.view = view;

  UIWindowScene * windowScene = (UIWindowScene *)scene;
  self.window = [[UIWindow alloc] initWithWindowScene:windowScene];
  self.window.rootViewController = vc;
  [self.window makeKeyAndVisible];
}
@end

@interface POCAppDelegate : NSObject<UIApplicationDelegate>
@end
@implementation POCAppDelegate
- (UISceneConfiguration *) application:(UIApplication *) application 
configurationForConnectingSceneSession:(UISceneSession *) connectingSceneSession 
                               options:(UISceneConnectionOptions *) options
{
  application.applicationSupportsShakeToEdit = YES;

  UISceneConfiguration * res = [[UISceneConfiguration alloc] initWithName:@"Default"
                                                              sessionRole:connectingSceneSession.role];
  res.sceneClass = [UIWindowScene class];
  res.delegateClass = [POCWindowSceneDelegate class];
  return res;
}

- (void)applicationWillTerminate:(UIApplication *)app {
  glu_deinit();
}
@end

int main(int argc, char ** argv) {
  @autoreleasepool {
    return UIApplicationMain(argc, argv, nil, @"POCAppDelegate");
  }
}
