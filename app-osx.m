#import <AppKit/AppKit.h>

#include "mtl.h"
#include "glu.h"

@interface POCWindow : NSWindow
@end
@implementation POCWindow
- (void)mouseEvent:(NSEvent *)event callback:(void(*)(int, int))cb {
  NSView * v = self.contentViewController.view;
  CGPoint liw = [event locationInWindow];
  CGPoint p = [v convertPoint:liw fromView:nil];
  cb(p.x, v.frame.size.height - p.y);
}
- (void)mouseDown:(NSEvent *)event {
  [self mouseEvent:event callback:glu_mouse_down];
}
- (void) mouseUp:(NSEvent *)event {
  [self mouseEvent:event callback:glu_mouse_up];
}
- (void) mouseMoved:(NSEvent *)event {
  [self mouseEvent:event callback:glu_mouse_move];
}
- (void) mouseDragged:(NSEvent *)event {
  [self mouseEvent:event callback:glu_mouse_move];
}
@end

@interface POCAppDelegate : NSObject<NSApplicationDelegate>
@end
@implementation POCAppDelegate
- (void)applicationWillTerminate:(NSApplication *)app {
  glu_deinit();
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
@end

static void run() {
  NSViewController * vc = [NSViewController new];
  vc.view = [POCViewDelegate new];

  POCWindow * w = [POCWindow new];
  w.acceptsMouseMovedEvents = YES;
  w.contentViewController = vc;
  w.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

  NSRect crect = NSMakeRect(0, 0, 800, 600);
  NSRect frect = [w frameRectForContentRect:crect];
  [w setFrame:frect display:YES];
  [w center];
  [w makeKeyAndOrderFront:w];

  // Apple menu
  NSMenu * menu = [NSMenu new];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Chesstor"
               action:@selector(terminate:)
        keyEquivalent:@"q"]];

  NSMenuItem * item = [NSMenuItem new];
  item.submenu = menu;

  NSMenu * bar = [NSMenu new];
  [bar addItem:item];

  NSApplication * a = [NSApplication sharedApplication];
  a.delegate = [POCAppDelegate new];
  a.mainMenu = bar;
  [a activateIgnoringOtherApps:YES];
  [a run];
}

int main() {
  @autoreleasepool {
    run();
  }
}
