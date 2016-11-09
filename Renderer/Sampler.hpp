#ifndef Sampler_hpp
#define Sampler_hpp

#include "Texture.hpp"

namespace renderlib {
	class Sampler {
	public:
		Sampler(const Texture& t);
		glm::vec4 lookup(const glm::vec2& texCoord) const;
	private:
		const Texture& _texture;
	};
}

#endif /* Sampler_hpp */
