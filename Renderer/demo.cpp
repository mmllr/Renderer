#include "demo.hpp"
#include <vector>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <iostream>
#include "Sampler.hpp"
#include "ResourceLoader.h"

using namespace renderlib;
using namespace std;
using namespace glm;

Texture tex = loadTexture();
vector<Vertex> vertexes = {
	{{-.25f, .25f, 0.f, 1}, {1.f, 0, 0, 1},{.0f, .0f}},
	{{.25f, .25f, 0.f, 1}, {0, 1.f, 0, 1},{1.f, 0.f}},
	{{.25f, -.25f, 0.f, 1}, {0, 0, 1.f, 1},{1.f, 1.f}},
	{{-.25f, -.25f, 0.f, 1}, {1.f, 0, 1.f, 1}, {0.f, 1.f}}
};
vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

vec4 basicPixelShader(const Vertex& fragment) {
	return {1.f,0.f,1.f,1.f};
}

vec4 desaturationPixelShader(const Vertex& fragment) {
	return {glm::saturation(.0f, vec3(fragment.color)), 1};
}

vec4 colorPixelShader(const Vertex& fragment) {
	return fragment.color;
}


mat4 modelView() {
	static float angle = 0;

	mat4 modelView;
	modelView = glm::translate(modelView, {0,0,-.6f});
	modelView = glm::rotate(modelView, glm::radians(angle), {1.f, 0.f, 0.f});
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 1.f, 0.f});
	modelView = glm::rotate(modelView, glm::radians(angle), {0.f, 0.f, 1.f});
	
	angle = angle >= 360 ? 0 : angle + 1;
	return modelView;
}

void setupCommonRendering(renderlib::Renderer& renderer) {
	renderer.setVertexBuffer(vertexes);
	renderer.setIndexBuffer(indices);

	mat4 projection = glm::perspective(glm::radians(60.0f), renderer.aspectRatio(), 0.1f, 1000.f);
	mat4 mvp = projection*modelView();
	
	renderer.setVertexShader([=](const Vertex& vertex)-> Vertex {
		return {mvp * vertex.position, vertex.color, vertex.texCoords};
	});
}

void renderSceneBasic(renderlib::Renderer& renderer) {
	setupCommonRendering(renderer);
	
	renderer.setPixelShader(basicPixelShader);
	
	renderer.drawTriangles(0, 2);
}

void renderSceneGouraud(renderlib::Renderer& renderer) {
	setupCommonRendering(renderer);
	
	renderer.setPixelShader(colorPixelShader);
	
	renderer.drawTriangles(0, 2);
}

void renderSceneTextured(renderlib::Renderer& renderer) {
	setupCommonRendering(renderer);
	Sampler sampler = Sampler(tex);
	renderer.setPixelShader([&](const Vertex& fragment) {
		return sampler.lookup(fragment.texCoords);
	});
	
	renderer.drawTriangles(0, 2);
}

void renderSceneTexturedAndColor(renderlib::Renderer& renderer) {
	setupCommonRendering(renderer);

	Sampler sampler = Sampler(tex);
	renderer.setPixelShader([&](const Vertex& fragment) {
		return sampler.lookup(fragment.texCoords) * fragment.color;
	});
	
	renderer.drawTriangles(0, 2);
}
