#include "demo.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

using namespace renderlib;
using namespace std;
using namespace glm;

Vertex basicVertexShader(const mat4& mvp, const Vertex& vertex) {
	return {mvp * vertex.position};
}

Pixel basicPixelShader(const vec4& fragCoord) {
	return {0, 255, 0, 255};
}

void renderScene01(renderlib::Renderer& renderer) {
	static float angle = 0;

	vector<Vertex> vertexes = {
		{{-.25f, .25f, -1.f, 1}, {1.f, 0, 0, 1}},
		{{.25f, .25f, -1.f, 1}, {0, 1.f, 0, 1}},
		{{.25f, -.25f, -1.f, 1}, {0, 0, 1.f, 1}},
		{{-.25f, -.25f, -1.f, 1}, {1.f, 0, 1.f, 1}}
	};
	vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);
	
	mat4 modelView;
	modelView = glm::translate(modelView, {0,0,-1.2f});
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 1.f, 0.f});
	
	renderer.setModelView(modelView);
	
	renderer.drawTriangles(0, 2);

	angle = angle >= 360 ? 0 : angle + 1;
}
