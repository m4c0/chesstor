#import <AppKit/AppKit.h>

#include "mtl.h"

@interface POCView : MTKView
@end
@implementation POCView
- (BOOL)acceptsFirstResponder {
  return YES;
}
- (void) mouseDown:(NSEvent *)event {
  CGPoint liw = [event locationInWindow];
  CGPoint p = [self convertPoint:liw fromView:nil];
  glu_mouse_down(p.x, self.frame.size.height - p.y);
}
- (void) mouseMoved:(NSEvent *)event {
  CGPoint liw = [event locationInWindow];
  CGPoint p = [self convertPoint:liw fromView:nil];
  glu_mouse_move(p.x, self.frame.size.height - p.y);
}
- (void) mouseDragged:(NSEvent *)event {
  CGPoint liw = [event locationInWindow];
  CGPoint p = [self convertPoint:liw fromView:nil];
  glu_mouse_move(p.x, self.frame.size.height - p.y);
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
  MTKView * v = [POCView new];
  v.device = MTLCreateSystemDefaultDevice();
  v.clearColor = MTLClearColorMake(0.01, 0.02, 0.03, 1.0);
  v.delegate = [POCViewDelegate newWithDevice:v.device];

  NSWindow * w = [NSWindow new];
  w.acceptsMouseMovedEvents = YES;
  w.contentView = v;
  w.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

  NSRect crect = NSMakeRect(0, 0, 800, 600);
  NSRect frect = [w frameRectForContentRect:crect];
  [w setFrame:frect display:YES];
  [w center];
  [w makeKeyAndOrderFront:w];

  // Apple menu
  NSMenu * menu = [NSMenu new];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Sokoban"
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
