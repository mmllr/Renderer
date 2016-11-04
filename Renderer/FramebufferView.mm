#import "FramebufferView.h"
#include "Framebuffer.hpp"

using namespace Renderer;

@implementation FramebufferView

- (void)drawRect:(NSRect)dirtyRect {
	CGFloat scaleFactor = [NSScreen mainScreen].backingScaleFactor;
	Framebuffer framebuffer(CGRectGetWidth(self.bounds)*scaleFactor, CGRectGetHeight(self.bounds)*scaleFactor);

	if (self.renderHandler) {
		self.renderHandler(framebuffer);
	}
	CGContextRef offscreen = CGBitmapContextCreate((void*)framebuffer.pixelData(), framebuffer.getWidth(), framebuffer.getHeight(), framebuffer.getBitsPerComponent(), framebuffer.getBytesPerRow(), [NSColorSpace deviceRGBColorSpace].CGColorSpace, kCGImageAlphaPremultipliedLast);
	// draw stuff into offscreen
	CGImageRef image = CGBitmapContextCreateImage(offscreen);
	CGContextDrawImage((CGContextRef)[[NSGraphicsContext currentContext] graphicsPort], dirtyRect, image);
	CFRelease(offscreen);
}

@end
