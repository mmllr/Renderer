#ifndef Sampler_hpp
#define Sampler_hpp

#include "Texture.hpp"

namespace renderlib {
	class Sampler {
	public:
		Sampler(const Texture& t);
		glm::vec4 lookup(const glm::vec2& texCoord) const;
	private:
		glm::vec4 pointSample(const glm::vec2 &texCoord) const;
		glm::vec4 bilinearSample(const glm::vec2& texCoord) const;
		const Texture& _texture;
	};
}

#endif /* Sampler_hpp */
