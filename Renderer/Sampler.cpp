#include "Sampler.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

using namespace renderlib;
using namespace glm;

Sampler::Sampler(const Texture& t) : _texture(t) {
}

inline glm::vec4 toColor(const Pixel& p) {
	return vec4(float(p.r)/255,float(p.b)/255, float(p.g)/255, float(p.a)/255);
}

glm::vec4 Sampler::lookup(const glm::vec2 &texCoord) const {
	return pointSample(texCoord);
}

glm::vec4 Sampler::pointSample(const glm::vec2 &texCoord) const {
	vec2 st(glm::clamp(texCoord.s, 0.f, 1.f), glm::clamp(texCoord.t, 0.f, 1.f));
	return toColor(_texture.pixelAt(st.s * (_texture.getHeight()-1), st.t * (_texture.getWidth()-1)));
}

glm::vec4 Sampler::bilinearSample(const glm::vec2& texCoord) const {
	int w = _texture.getWidth()-1;
	int h = _texture.getHeight()-1;
	float s = texCoord.s * w - 0.5f;
	float t = texCoord.t * h - 0.5f;
	float s_ratio = glm::fract(s);
	float t_ratio = glm::fract(t);
	float s_opposite = 1 - s_ratio;
	float t_opposite = 1 - t_ratio;

	return {(toColor(_texture.pixelAt(s, t))*s_opposite+toColor(_texture.pixelAt(s+1, t))*s_ratio)*t_opposite + (toColor(_texture.pixelAt(s, t+1))*s_opposite+toColor(_texture.pixelAt(s+1, t+1))*s_ratio)*t_ratio};
}
