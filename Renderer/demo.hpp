#ifndef demo_hpp
#define demo_hpp

#include "Renderer.hpp"
#include <glm/glm.hpp>

using namespace glm;
using namespace renderlib;

void renderScene01(renderlib::Renderer& render);
Vertex basicVertexShader(const mat4& mvp, const Vertex& vertex);
vec4 basicPixelShader(const Vertex& fragment);
vec4 desaturationPixelShader(const Vertex& fragment);

#endif /* demo_hpp */
