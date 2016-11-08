#include "demo.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

using namespace renderlib;
using namespace std;
using namespace glm;

void renderScene01(renderlib::Renderer& renderer) {
	glm::vec2 verts[] = {
		{105,107},
		{213,255},
		{87,227}
	};
}

void renderScene02(renderlib::Renderer& renderer) {
	static float angle = 0;

	vector<Vertex> vertexes = {
		{{.25f, .25f, -1.f, 1}},
		{{2.25f, .25f, -1.f, 1}},
		{{0.f, -.25f, -1.f, 1}}
	};
	vector<unsigned int> indices = {0, 1, 2};

	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);
	
	mat4 modelView;
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 0.f, 1.f});
	
	renderer.setModelView(modelView);
	
	renderer.drawTriangles(0, 1);
	//distance += 10;
	angle = angle >= 360 ? 0 : angle + 1;
}
