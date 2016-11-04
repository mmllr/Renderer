#import <Cocoa/Cocoa.h>
#include "Framebuffer.hpp"

typedef void (^RenderBlock)(Renderer::Framebuffer& framebuffer);

@interface FramebufferView : NSView

@property (nonatomic, copy) RenderBlock renderHandler;

@end
