#ifndef Framebuffer_hpp
#define Framebuffer_hpp

#include <cstdint>
#include <vector>
#include "renderlib.hpp"

namespace renderlib {

	struct Framebuffer {
	public:
		Framebuffer(size_t width, size_t height);
		void setPixel(const Pixel& pixel, size_t x, size_t y);
		const void* pixelData() const { return static_cast<const void*>(_pixels.data()); }
		size_t getWidth(void) const { return _width; }
		size_t getHeight(void) const { return _height; }
		size_t getBitsPerComponent(void) const { return 8; }
		size_t getBytesPerRow(void) const { return sizeof(Pixel)*_width; }
		void resize(size_t width, size_t height);
		void fill(const Pixel& fillElement);
	private:
		size_t _width;
		size_t _height;
		std::vector<Pixel> _pixels;
	};
}


#endif /* Framebuffer_hpp */
