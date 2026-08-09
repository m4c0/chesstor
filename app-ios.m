#include "pch.h"
#include "glu.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <UIKit/UIKit.h>

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

@interface POCViewDelegate : NSObject<MTKViewDelegate>
@property (nonatomic,strong) id<MTLCommandQueue> queue;
@property (nonatomic,strong) id<MTLRenderPipelineState> pipeline;
@property (nonatomic,strong) id<MTLBuffer> grid;
@property (nonatomic) BOOL ready;
@end
@implementation POCViewDelegate
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
  if (self.ready) glu_resize(size.width, size.height);
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
  UIWindowScene * windowScene = (UIWindowScene *)scene;

  id<MTLDevice> d = MTLCreateSystemDefaultDevice();

  POCViewDelegate * vd = [POCViewDelegate new];
  vd.queue = [d newCommandQueue];

  id<MTLLibrary> vert = load_library(d, @"shader.vert");
  id<MTLLibrary> frag = load_library(d, @"shader.frag");
  if (!vert || !frag) return;

  MTLRenderPipelineDescriptor * pd = [MTLRenderPipelineDescriptor new];
  pd.vertexFunction   = [vert newFunctionWithName:@"main0"];
  pd.fragmentFunction = [frag newFunctionWithName:@"main0"];
  pd.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  NSError * err;
  vd.pipeline = [d newRenderPipelineStateWithDescriptor:pd error:&err];
  if (err) {
    NSLog(@"Error creating pipeline: %@", err);
    return;
  }

  vd.grid = [d newBufferWithLength:GLU_BUF_SIZE options:MTLResourceStorageModeShared];

  MTKView * view = [MTKView new];
  view.device = d;
  view.clearColor = MTLClearColorMake(0.01, 0.02, 0.03, 1.0);
  view.delegate = vd;

  POCViewController * vc = [POCViewController new];
  vc.view = view;

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
