#ifndef Texture_hpp
#define Texture_hpp

#include <vector>
#include "renderlib.hpp"

namespace renderlib {
	class Texture {
	public:
		Texture() : _width(0), _height(0), _borderColor({0,0,0,0}) {};
		Texture(const std::vector<Pixel>& pixelData, unsigned int width, unsigned int height);
		unsigned int getWidth(void) const { return _width; }
		unsigned int getHeight(void) const { return _height; }
		Pixel pixelAt(unsigned int x, unsigned int y) const;
		void setBorderColor(const Pixel& p);
	private:
		unsigned int _width;
		unsigned int _height;
		std::vector<Pixel> _pixelData;
		Pixel _borderColor;
	};
}

#endif /* Texture_hpp */
