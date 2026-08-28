@import Metal;
@import MetalKit;

#include "mtl.h"
#include "bited.h"

@interface POCViewController : NSViewController
@end
@implementation POCViewController
@end

@interface POCWindow : NSWindow
@end
@implementation POCWindow
- (BOOL)acceptsFirstResponder {
  return YES;
}
- (void)keyDown:(NSEvent *)event {
  NSString * chrs = event.charactersIgnoringModifiers;
  if (chrs.length != 1) return;

  unichar c = [chrs characterAtIndex:0];
  switch (c) {
    case NSLeftArrowFunctionKey:  return btd_cursor(-1,  0);
    case NSRightArrowFunctionKey: return btd_cursor( 1,  0);
    case NSUpArrowFunctionKey:    return btd_cursor( 0, -1);
    case NSDownArrowFunctionKey:  return btd_cursor( 0,  1);

    case ' ': return btd_toggle();

    case 'e': return btd_load();
    case 'w': return btd_save();
  }
}
@end

@interface POCAppDelegate : NSObject<NSApplicationDelegate>
@end
@implementation POCAppDelegate
- (void)applicationWillTerminate:(NSApplication *)app {
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
@end

static void run() {
  POCViewController * vc = [POCViewController new];
  vc.view = [POCViewDelegate new];

  POCWindow * w = [POCWindow new];
  w.contentViewController = vc;
  w.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

  NSRect crect = NSMakeRect(0, 0, BTD_W * BTD_MULT, BTD_H * BTD_MULT);
  NSRect frect = [w frameRectForContentRect:crect];
  [w setFrame:frect display:YES];
  [w center];
  [w makeKeyAndOrderFront:w];

  // Apple menu
  NSMenu * menu = [NSMenu new];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Bited"
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
