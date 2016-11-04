#include "Framebuffer.hpp"

using namespace Renderer;

Framebuffer::Framebuffer(size_t width, size_t height) : _width(width), _height(height), _pixels(width*height, {0,0,0,255})
{
}

void Framebuffer::setPixel(const Pixel& pixel, size_t x, size_t y) {
	if (x >= _width || y >= _height) {
		return;
	}
	_pixels[x+y*getWidth()] = pixel;
}
