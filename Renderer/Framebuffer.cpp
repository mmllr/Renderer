#include "Framebuffer.hpp"
#include <algorithm>
#include "renderlib.hpp"

using namespace renderlib;

Framebuffer::Framebuffer(size_t width, size_t height) : _width(width), _height(height), _pixels(width*height, {0,0,0,255})
{
}

void Framebuffer::setPixel(const Pixel& pixel, size_t x, size_t y) {
	if (x >= _width || y >= _height) {
		return;
	}
	_pixels[x+y*getWidth()] = pixel;
}

void Framebuffer::resize(size_t width, size_t height) {
	_width = width;
	_height = height;
	_pixels.resize(width*height, {0,0,0,255});
}

void Framebuffer::fill(const Pixel &fillElement) {
	std::fill_n(_pixels.begin(), _pixels.size(), fillElement);
}
