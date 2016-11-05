#ifndef renderlib_h
#define renderlib_h

#include <cstdint>
#include <glm/glm.hpp>

namespace renderlib {
	struct Pixel {
		uint8_t r, g, b, a;
	};
	
	struct Vertex {
		glm::vec4 position;
	};
	
}

#endif /* renderlib_h */
