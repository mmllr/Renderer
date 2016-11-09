#ifndef demo_hpp
#define demo_hpp

#include "Renderer.hpp"
#include <glm/glm.hpp>

using namespace glm;
using namespace renderlib;

void renderScene01(renderlib::Renderer& render);
Vertex basicVertexShader(const mat4& mvp, const Vertex& vertex);
vec4 basicPixelShader(const Vertex& fragment, const Sampler& sampler);
vec4 desaturationPixelShader(const Vertex& fragment, const Sampler& sampler);
vec4 colorPixelShader(const Vertex& fragment, const Sampler& sampler);
vec4 texturedPixelShader(const Vertex& fragment, const Sampler& sampler);
vec4 textureColorPixelShader(const Vertex& fragment, const Sampler& sampler);


#endif /* demo_hpp */
