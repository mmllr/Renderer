#include "Texture.hpp"

#include <iostream>

using namespace renderlib;
using namespace std;

Texture::Texture(const std::vector<Pixel>& pixelData, unsigned int width, unsigned int height) : _pixelData(pixelData), _width(width), _height(height) {
}

Pixel Texture::pixelAt(unsigned int x, unsigned int y) const {
	if (x >= _width || y >= _height) {
		return _borderColor;
	}
	return _pixelData[x+y*_width];
}

void Texture::setBorderColor(const Pixel& p) {
	_borderColor = p;
}
