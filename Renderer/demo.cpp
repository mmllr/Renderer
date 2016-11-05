#include "demo.hpp"
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

using namespace renderlib;
using namespace std;
using namespace glm;

void renderScene01(renderlib::Renderer& renderer) {
	renderer.rasterizeLine({70, 50}, {40, 500}, {255, 255, 255, 255});
	renderer.rasterizeLine({70, 50}, {400, 600}, {255, 255, 255, 255});
	renderer.rasterizeLine({400, 600}, {40, 500}, {255, 255, 255, 255});
	
	glm::vec2 verts[] = {
		{150, -10}, {500, 51}, {250, 1300}
	};
	renderer.rasterizeTriangle(verts);

	renderer.rasterizeLine(verts[0], verts[1], {255, 0, 255, 255});
	renderer.rasterizeLine(verts[1], verts[2], {255, 0, 255, 255});
	renderer.rasterizeLine(verts[2], verts[0], {255, 0, 255, 255});
}

void renderScene02(renderlib::Renderer& renderer) {
	static float angle = 50;

	vector<Vertex> vertexes = {
		{{100,100,40, 1}},
		{{200,200,40, 1}},
		{{100,200,40, 1}}
	};
	vector<unsigned int> indices = {0, 1, 2};
	
	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);
	
	mat4 modelView;
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 0.f, -1.f});
	renderer.setModelView(modelView);
	
	renderer.drawTriangles(0, 1);
	//angle = angle >= 360 ? 0 : angle + 1;
}
