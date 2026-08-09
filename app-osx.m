#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <MetalKit/MetalKit.h>

#include "glu.h"

@interface POCViewDelegate : NSObject<MTKViewDelegate>
@property (nonatomic,strong) id<MTLCommandQueue> queue;
@property (nonatomic,strong) id<MTLRenderPipelineState> pipeline;
@property (nonatomic,strong) id<MTLBuffer> grid;
@property (nonatomic) BOOL ready;
@end
@implementation POCViewDelegate
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
  glu_resize(size.width, size.height);
}
- (void)drawInMTKView:(MTKView *)view {
  if (!self.ready) {
    glu_init(view.frame.size.width, view.frame.size.height);
    self.ready = YES;
  }
  glu_load(self.grid.contents);
  glu_frame();

  MTLRenderPassDescriptor * rpd = view.currentRenderPassDescriptor;
  if (rpd == nil) return;

  id<MTLCommandBuffer> cb = [self.queue commandBuffer];

  id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rpd];
  [enc setRenderPipelineState:self.pipeline];
  [enc setVertexBytes:&glu_pc length:sizeof(glu_upc_t) atIndex:0];
  [enc setFragmentBytes:&glu_pc length:sizeof(glu_upc_t) atIndex:0];
  [enc setFragmentBuffer:self.grid offset:0 atIndex:1];
  [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
  [enc endEncoding];

  [cb presentDrawable:view.currentDrawable];
  [cb commit];
}
@end

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

static id<MTLLibrary> load_library(id<MTLDevice> device, NSString * name) {
  NSString * path = [[NSBundle mainBundle] pathForResource:name ofType:@"metal"];
  NSString * src = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
  MTLCompileOptions * opts = [MTLCompileOptions new];
  NSError * err;
  id<MTLLibrary> lib = [device newLibraryWithSource:src options:opts error:&err];
  if (err) {
    NSLog(@"Error compiling shader: %@", err);
    return nil;
  }
  return lib;
}
static void run() {
  MTKView * v = [POCView new];
  v.device = MTLCreateSystemDefaultDevice();
  v.clearColor = MTLClearColorMake(0.01, 0.02, 0.03, 1.0);

  POCViewDelegate * vc = [POCViewDelegate new];
  v.delegate = vc;
  vc.queue = [v.device newCommandQueue];

  id<MTLLibrary> vert = load_library(v.device, @"shader.vert");
  id<MTLLibrary> frag = load_library(v.device, @"shader.frag");
  if (!vert || !frag) return;

  MTLRenderPipelineDescriptor * pd = [MTLRenderPipelineDescriptor new];
  pd.vertexFunction   = [vert newFunctionWithName:@"main0"];
  pd.fragmentFunction = [frag newFunctionWithName:@"main0"];
  pd.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  NSError * err;
  vc.pipeline = [v.device newRenderPipelineStateWithDescriptor:pd error:&err];
  if (err) {
    NSLog(@"Error creating pipeline: %@", err);
    return;
  }

  vc.grid = [v.device newBufferWithLength:GLU_BUF_SIZE options:MTLResourceStorageModeShared];

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
