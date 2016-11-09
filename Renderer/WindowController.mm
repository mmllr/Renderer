#import "WindowController.h"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include <glm/glm.hpp>
#include "demo.hpp"
#include "Texture.hpp"
#include <vector>

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
	_renderer->setVertexShader(basicVertexShader);
	_renderer->setPixelShader(basicPixelShader);
	_renderer->setRenderFunc(renderScene01);
	_renderer->setTexture([self loadTexture]);

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(frameBufferViewBoundsChanged:) name:NSViewFrameDidChangeNotification object:self.frameBufferView];
	[self updateView];
	[NSTimer scheduledTimerWithTimeInterval:.01 repeats:YES block:^(NSTimer * _Nonnull timer) {
		[self updateView];
	}];
}

- (renderlib::Texture)loadTexture {
	NSDataAsset *asset = [[NSDataAsset alloc] initWithName:@"Texture"];
	NSBitmapImageRep *img = [[NSBitmapImageRep alloc] initWithData:asset.data];
	
	int w = img.size.width;
	int h = img.size.height;
	std::vector<Pixel> pixels;
	for (int i = 0; i < w*h; ++i) {
		NSUInteger pixel[4];
		[img getPixel:pixel atX:i % w y:i / w];
		pixels.push_back({static_cast<uint8_t>(pixel[0]), static_cast<uint8_t>(pixel[1]), static_cast<uint8_t>(pixel[2]), 255});
	}
	return Texture(pixels, w, h);
}

- (void)frameBufferViewBoundsChanged:(NSNotification*)notification {
	NSRect backingRect = [self.frameBufferView convertRectToBacking:self.frameBufferView.bounds];
	_renderer->setViewport(0, 0, backingRect.size.width, backingRect.size.height);
	[self updateView];
}

- (void)updateView {
	_renderer->render();
	if (self.renderHandler) {
		CGImageRef image = self.renderHandler(_renderer->frameBuffer());
		self.frameBufferView.layer.contents = (__bridge id)image;
		CGImageRelease(image);
	}
}

@end
