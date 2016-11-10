#ifndef demo_hpp
#define demo_hpp

#include "Renderer.hpp"
#include <glm/glm.hpp>

using namespace glm;
using namespace renderlib;

void renderSceneBasic(renderlib::Renderer& render);
void renderSceneGouraud(renderlib::Renderer& render);
void renderSceneTextured(renderlib::Renderer& render);
void renderSceneTexturedAndColor(renderlib::Renderer& render);

#endif /* demo_hpp */
