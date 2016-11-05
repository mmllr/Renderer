#import "WindowController.h"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include <glm/glm.hpp>
#include "demo.hpp"

using namespace renderlib;
using namespace glm;

typedef CGImageRef (^RenderBlock)(const renderlib::Framebuffer& framebuffer);

@interface WindowController ()
@property (weak) IBOutlet NSView *frameBufferView;
@property (nonatomic, copy) RenderBlock renderHandler;

@end

@implementation WindowController {
	Renderer *_renderer;
}

- (void)dealloc {
	delete _renderer;
	_renderer = nullptr;
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)awakeFromNib {
	[super awakeFromNib];
	self.renderHandler = ^CGImageRef(const Framebuffer& framebuffer) {
		CGContextRef offscreen = CGBitmapContextCreate((void*)framebuffer.pixelData(), framebuffer.getWidth(), framebuffer.getHeight(), framebuffer.getBitsPerComponent(), framebuffer.getBytesPerRow(), [NSColorSpace deviceRGBColorSpace].CGColorSpace, kCGImageAlphaPremultipliedLast);
		// draw stuff into offscreen
		CGImageRef image = CGBitmapContextCreateImage(offscreen);
		CFRelease(offscreen);
		return image;
	};
	NSRect rect = [self.frameBufferView convertRectToBacking:self.frameBufferView.bounds];
	_renderer = new Renderer(rect.size.width, rect.size.height);
	_renderer->setRenderFunc(renderScene02);
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(frameBufferViewBoundsChanged:) name:NSViewFrameDidChangeNotification object:self.frameBufferView];
	[self updateView];
	[NSTimer scheduledTimerWithTimeInterval:.01 repeats:YES block:^(NSTimer * _Nonnull timer) {
		[self updateView];
	}];
}

- (void)frameBufferViewBoundsChanged:(NSNotification*)notification {
	NSRect backingRect = [self.frameBufferView convertRectToBacking:self.frameBufferView.bounds];
	_renderer->resize(backingRect.size.width, backingRect.size.height);
	[self updateView];
}

- (void)updateView {
	_renderer->render();
	if (self.renderHandler) {
		self.frameBufferView.layer.contents = (__bridge id)self.renderHandler(_renderer->frameBuffer());
	}
}

@end
