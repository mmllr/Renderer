#include "demo.hpp"
#include <vector>

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
	vector<Vertex> vertexes = {
		{{100,100,40, 1}},
		{{200,200,40, 1}},
		{{100,200,40, 1}}
	};
	vector<unsigned int> indices = {0, 1, 2};
	
	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);
	renderer.drawTriangles(0, 1);
}
