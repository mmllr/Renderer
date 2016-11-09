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
	_renderer->setPixelShader(texturedPixelShader);
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
	
	int w = (int)img.pixelsWide;
	int h = (int)img.pixelsHigh;
	std::vector<Pixel> pixels;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			NSUInteger pixel[4];
			[img getPixel:pixel atX:x y:y];
			pixels.push_back({static_cast<uint8_t>(pixel[0]), static_cast<uint8_t>(pixel[1]), static_cast<uint8_t>(pixel[2]), 255});
		}
		
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
