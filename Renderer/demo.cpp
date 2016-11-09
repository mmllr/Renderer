#include "demo.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <iostream>
#include "Sampler.hpp"

using namespace renderlib;
using namespace std;
using namespace glm;

Vertex basicVertexShader(const mat4& mvp, const Vertex& vertex) {
	return {mvp * vertex.position, vertex.color, vertex.texCoords};
}

vec4 basicPixelShader(const Vertex& fragment, const Sampler& sampler) {
	return {1.f,0.f,1.f,1.f};
}

vec4 desaturationPixelShader(const Vertex& fragment, const Sampler& sampler) {
	return {glm::saturation(.0f, vec3(fragment.color)), 1};
}

vec4 colorPixelShader(const Vertex& fragment, const Sampler& sampler) {
	return fragment.color;
}

vec4 texturedPixelShader(const Vertex& fragment, const Sampler& sampler) {
	return sampler.lookup(fragment.texCoords);
}

vec4 textureColorPixelShader(const Vertex& fragment, const Sampler& sampler) {
	return sampler.lookup(fragment.texCoords) * fragment.color;
}

void renderScene01(renderlib::Renderer& renderer) {
	static float angle = 0;

	vector<Vertex> vertexes = {
		{{-.25f, .25f, 0.f, 1}, {1.f, 0, 0, 1},{.0f, .0f}},
		{{.25f, .25f, 0.f, 1}, {0, 1.f, 0, 1},{1.f, 0.f}},
		{{.25f, -.25f, 0.f, 1}, {0, 0, 1.f, 1},{1.f, 1.f}},
		{{-.25f, -.25f, 0.f, 1}, {1.f, 0, 1.f, 1}, {0.f, 1.f}}
	};
	vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);
	
	mat4 modelView;
	modelView = glm::translate(modelView, {0,0,-.6f});
	modelView = glm::rotate(modelView, glm::radians(angle), {1.f, 0.f, 0.f});
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 1.f, 0.f});
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 0.f, 1.f});
	
	
	renderer.setModelView(modelView);
	
	renderer.drawTriangles(0, 2);

	angle = angle >= 360 ? 0 : angle + 1;
}
