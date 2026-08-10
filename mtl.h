#import <CoreFoundation/CoreFoundation.h>
#import <MetalKit/MetalKit.h>

#include "glu.h"

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

+ (id)newWithDevice:(id<MTLDevice>)device;
@end

@implementation POCViewDelegate
+ (id)newWithDevice:(id<MTLDevice>)device {
  POCViewDelegate * d = [POCViewDelegate new];
  d.queue = [device newCommandQueue];
  d.grid = [device newBufferWithLength:GLU_BUF_SIZE options:MTLResourceStorageModeShared];

  id<MTLLibrary> vert = load_library(device, @"shader.vert");
  id<MTLLibrary> frag = load_library(device, @"shader.frag");
  if (!vert || !frag) return nil;

  MTLRenderPipelineDescriptor * pd = [MTLRenderPipelineDescriptor new];
  pd.vertexFunction   = [vert newFunctionWithName:@"main0"];
  pd.fragmentFunction = [frag newFunctionWithName:@"main0"];
  pd.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  NSError * err;
  d.pipeline = [device newRenderPipelineStateWithDescriptor:pd error:&err];
  if (err) return (NSLog(@"Error creating pipeline: %@", err), nil);

  return d;
}
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
