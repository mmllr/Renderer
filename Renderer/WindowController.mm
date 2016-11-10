#import "WindowController.h"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include <glm/glm.hpp>
#include "demo.hpp"
#include "Texture.hpp"
#include <vector>
#import "ResourceLoader.h"

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
	_renderer->setPixelShader(texturedPixelShader);
	_renderer->setRenderFunc(renderScene01);
	_renderer->setTexture(loadTexture());

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(frameBufferViewBoundsChanged:) name:NSViewFrameDidChangeNotification object:self.frameBufferView];
	[self updateView];
	[NSTimer scheduledTimerWithTimeInterval:.01 repeats:YES block:^(NSTimer * _Nonnull timer) {
		[self updateView];
	}];
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

- (IBAction)enablePerspectiveCorrection:(id)sender {
	_renderer->enablePerspectiveCorrection();
}

- (IBAction)disablePerspectiveCorrection:(id)sender {
	_renderer->diablePerspectiveCorrection();
}

- (IBAction)switchRenderMode:(id)sender {
	std::vector<std::function<vec4 (const Vertex& fragment, const Sampler& sampler)>> shaders = {
		basicPixelShader,
		colorPixelShader,
		texturedPixelShader,
		textureColorPixelShader
	};
	
	if ([sender tag] < shaders.size()) {
		_renderer->setPixelShader(shaders[[sender tag]]);
	}
}


@end
