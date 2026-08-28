@import Metal;
@import MetalKit;

#include "bited.h"

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

@interface POCViewDelegate : MTKView<MTKViewDelegate>
@property (nonatomic,strong) NSMutableArray * objects;
@property (nonatomic,strong) id<MTLCommandQueue> queue;
@property (nonatomic,strong) id<MTLRenderPipelineState> pipeline;
+ (id)newWithDevice:(id<MTLDevice>)device;
@end
static void * new_buffer(void * ptr, int sz) {
  POCViewDelegate * d = ptr;
  id<MTLBuffer> res = [d.device newBufferWithLength:sz options:MTLResourceStorageModeShared];
  [d.objects addObject:res];
  return res.contents;
}
static g3d_sampler_t * new_sampler(void * ptr) {
  POCViewDelegate * d = ptr;

  MTLSamplerDescriptor * sd = [MTLSamplerDescriptor new];
  sd.minFilter = sd.magFilter = MTLSamplerMinMagFilterNearest;
  id<MTLSamplerState> res = [d.device newSamplerStateWithDescriptor:sd];
  [d.objects addObject:res];
  return res;
}
static g3d_texture_t * new_texture(void * ptr, int w, int h) {
  POCViewDelegate * d = ptr;

  MTLTextureDescriptor * td = [MTLTextureDescriptor new];
  td.pixelFormat = MTLPixelFormatR8Unorm;
  td.width       = w;
  td.height      = h;
  id<MTLTexture> res = [d.device newTextureWithDescriptor:td];
  [d.objects addObject:res];
  return res;
}
static void load_texture(g3d_texture_t * t, void * data) {
  id<MTLTexture> txt = t;

  MTLRegion r = { {0,0,0}, {txt.width,txt.height,1} };
  [txt replaceRegion:r mipmapLevel:0 withBytes:data bytesPerRow:txt.width];
}
@implementation POCViewDelegate
+ (id)newWithDevice:(id<MTLDevice>)device {
  POCViewDelegate * d = [POCViewDelegate new];
  d.objects = [NSMutableArray new];
  d.device = device;
  d.queue = [device newCommandQueue];

  id<MTLLibrary> vert = load_library(device, @"bited.vert");
  id<MTLLibrary> frag = load_library(device, @"bited.frag");
  if (!vert || !frag) return nil;

  MTLRenderPipelineDescriptor * pd = [MTLRenderPipelineDescriptor new];
  pd.vertexFunction   = [vert newFunctionWithName:@"main0"];
  pd.fragmentFunction = [frag newFunctionWithName:@"main0"];
  pd.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  NSError * err;
  d.pipeline = [device newRenderPipelineStateWithDescriptor:pd error:&err];
  if (err) return (NSLog(@"Error creating pipeline: %@", err), nil);

  g3d_api_t api = {
    .ptr          = d,
    .new_buffer   = new_buffer,
    .new_sampler  = new_sampler,
    .new_texture  = new_texture,
    .load_texture = load_texture,
  };
  btd_init(&api);

  return d;
}
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
}
- (void)drawInMTKView:(MTKView *)view {
  MTLRenderPassDescriptor * rpd = view.currentRenderPassDescriptor;
  if (rpd == nil) return;

  id<MTLCommandBuffer> cb = [self.queue commandBuffer];

  id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rpd];
  [enc setRenderPipelineState:self.pipeline];
  [enc setVertexBuffer:self.objects[0] offset:0 atIndex:0];
  [enc setFragmentBuffer:self.objects[0] offset:0 atIndex:0];
  [enc setFragmentTexture:self.objects[2] atIndex:0];
  [enc setFragmentSamplerState:self.objects[1] atIndex:0];
  [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
  [enc endEncoding];

  [cb presentDrawable:view.currentDrawable];
  [cb commit];
}

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

@interface POCViewController : NSViewController
@end
@implementation POCViewController
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
  id<MTLDevice> device = MTLCreateSystemDefaultDevice();

  POCViewDelegate * v = [POCViewDelegate newWithDevice:device];
  v.delegate = v;

  POCViewController * vc = [POCViewController new];
  vc.view = v;

  NSWindow * w = [NSWindow new];
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
