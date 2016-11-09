#include "Sampler.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

using namespace renderlib;
using namespace glm;

Sampler::Sampler(const Texture& t) : _texture(t) {
}

glm::vec4 Sampler::lookup(const glm::vec2 &texCoord) const {
	vec2 st(glm::clamp(texCoord.s, 0.f, 1.f), glm::clamp(texCoord.t, 0.f, 1.f));
	
	Pixel p = _texture.pixelAt(st.s * (_texture.getHeight()-1), st.t * (_texture.getWidth()-1));
	return vec4(float(p.r)/255,float(p.b)/255, float(p.g)/255, float(p.a)/255);
}
