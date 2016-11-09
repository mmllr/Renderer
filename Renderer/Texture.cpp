#include "Texture.hpp"

#include <iostream>

using namespace renderlib;
using namespace std;

Texture::Texture(const std::vector<Pixel>& pixelData, unsigned int width, unsigned int height) : _pixelData(pixelData), _width(width), _height(height) {
}

Pixel Texture::pixelAt(unsigned int x, unsigned int y) const {
	assert(x < _width);
	assert(y < _height);
	return _pixelData[x+y*_width];
}
