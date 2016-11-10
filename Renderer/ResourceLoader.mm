#import <Cocoa/Cocoa.h>
#import "ResourceLoader.h"

using namespace renderlib;

renderlib::Texture loadTexture() {
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
