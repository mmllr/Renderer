#include "Renderer.hpp"
#include "LinearInterpolator.hpp"
#include <tuple>
#include <algorithm>
#include <cassert>
#undef GLM_LEFT_HANDED
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <iostream>


using namespace renderlib;
using namespace glm;
using namespace std;

Renderer::Renderer(unsigned int width, unsigned int height) : _x(0), _y(0), _width(width), _height(height), _nearZ(0), _farZ(1), _buffer(width, height), _clearColor({0, 0, 0, 255}), _projection(glm::perspective(glm::radians(60.0f), float(width)/height, 0.1f, 1000.f)) {
	
}

const Framebuffer& Renderer::frameBuffer() const {
	return _buffer;
}

void Renderer::setClearColor(const renderlib::Pixel &clearColor) {
	_clearColor = clearColor;
}

void Renderer::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	_x = x;
	_y = y;
	_width = width;
	_height = height;
	_buffer.resize(width, height);
	setProjection(glm::perspective(glm::radians(60.0f), float(width)/height, 0.1f, 1000.f));
}

void Renderer::setDepthRange(float nearZ, float farZ) {
	_nearZ = nearZ;
	_farZ = farZ;
}

void Renderer::setRenderFunc(std::function<void (Renderer&)> handler) {
	_renderFunction = handler;
}

void Renderer::setVertexShader(std::function<Vertex (const mat4& mvp, const Vertex& vertex)> vertexShader) {
	_vertexShader = vertexShader;
}

void Renderer::setPixelShader(std::function<Pixel (const vec4& fragCoord)> pixelShader) {
	_pixelShader = pixelShader;
}

void Renderer::setVertexBuffer(const vector<Vertex>& vertexBuffer) {
	_vertexBuffer = vertexBuffer;
	_clipVertexes.resize(vertexBuffer.size());
	_ndcVertexes.resize(vertexBuffer.size());
}

void Renderer::setIndexBuffer(const vector<uint32_t>& indexBuffer) {
	_indexBuffer = indexBuffer;
}

void Renderer::setModelView(const glm::mat4& modelView) {
	_modelView = modelView;
}

void Renderer::setProjection(const glm::mat4& projection) {
	_projection = projection;
}

void Renderer::render(void) {
	_buffer.fill(_clearColor);
	if (_renderFunction) {
		_renderFunction(*this);
	}
}

vector<Vertex> Renderer::transformAndClipTriangle(int startIndex) {
	mat4 mvp = _projection*_modelView;

	uint32_t first = _indexBuffer[startIndex];
	uint32_t second = _indexBuffer[startIndex+1];
	uint32_t third = _indexBuffer[startIndex+2];
	_clipVertexes[first] = _vertexShader(mvp, _vertexBuffer[first]);
	_clipVertexes[second] = _vertexShader(mvp, _vertexBuffer[second]);
	_clipVertexes[third] = _vertexShader(mvp, _vertexBuffer[third]);
	
	return clipTriangleToFrustum({_clipVertexes[first], _clipVertexes[second], _clipVertexes[third]});
}

void Renderer::drawTriangles(uint32_t firstVertexIndex, uint32_t count) {
	Vertex verts[3];
	Pixel colors[] = {{255, 255, 255, 255}, {255, 0, 0, 255}};
	
	for (unsigned int i = 0; i < count*3; i += 3) {
		vector<Vertex> clippedPoly = transformAndClipTriangle(firstVertexIndex+i);
		
		if (clippedPoly.size() < 3) {
			continue;
		}
		_ndcVertexes.resize(clippedPoly.size());
		for (int p = 0; p < clippedPoly.size(); ++p) {
			float oneOverW = 1./clippedPoly[p].position.w;
			_ndcVertexes[p].position = clippedPoly[p].position*oneOverW;
			_ndcVertexes[p].position.w = oneOverW;
		}

		verts[0].position = convertNormalizedDeviceCoordateToWindow(_ndcVertexes[0].position, _x, _y, _width, _height, _nearZ, _farZ);
		for (int p = 1; p < clippedPoly.size()-1; ++p) {
			verts[1].position = convertNormalizedDeviceCoordateToWindow(_ndcVertexes[p].position, _x, _y, _width, _height, _nearZ, _farZ);
			verts[2].position = convertNormalizedDeviceCoordateToWindow(_ndcVertexes[p+1].position, _x, _y, _width, _height, _nearZ, _farZ);
			
			rasterizeTriangle(verts, colors[1]);
		}
	}
}

void Renderer::rasterizeLine(const glm::vec2& start, const glm::vec2 &end, const Pixel& color) {
	vec2 drawStart(start), drawEnd(end);
	if (start.y > end.y) {
		swap(drawStart, drawEnd);
	}
	vec2 delta = drawEnd-drawStart;
	bool interpolateVertically = fabs(delta.x) > fabs(delta.y);

	if (interpolateVertically) {
		if (drawStart.x > drawEnd.x) {
			std::swap(drawStart, drawEnd);
			delta = drawEnd-drawStart;
		}
		int linearValue = floor(drawStart.x);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.y, fract(drawStart.x));
		
		do {
			_buffer.setPixel(color, linearValue++, floor(lerp.interpolatedValue()));
		} while(lerp.interpolate());
	}
	else {
		int linearValue = floor(drawStart.y);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.x, fract(drawStart.y));
		do {
			_buffer.setPixel(color, floor(lerp.interpolatedValue()), linearValue++);
		} while(lerp.interpolate());
	}
}

void Renderer::rasterizeTriangle(const Vertex (&verts)[3], const Pixel& color) {
	triangle t = triangleFromVerts(verts);
	
	if (t.leftAndRightOnTop) {
		edgeLoop(verts[t.topIndex], verts[t.midIndex], verts[t.bottomIndex], verts[t.bottomIndex], t.heightOfC);
		return;
	}
	float a = ((float)t.heightOfA)/t.heightOfC;
	Vertex vOnC = {verts[t.topIndex].position*(1.f-a) + verts[t.bottomIndex].position*a, {0.f,0.f,0.f,0.f}};
	edgeLoop(verts[t.topIndex], verts[t.topIndex], t.leftSideIsC ? vOnC : verts[t.midIndex], t.leftSideIsC ? verts[t.midIndex] : vOnC, t.heightOfA);
	edgeLoop(t.leftSideIsC ? vOnC : verts[t.midIndex], t.leftSideIsC ? verts[t.midIndex] : vOnC, verts[t.bottomIndex], verts[t.bottomIndex], t.heightOfB);
}

void Renderer::drawSpan(int leftX, int rightX, int y, const Pixel& color) {
	//assert(leftX <= rightX);
	if (leftX > rightX) {
		std::swap(leftX, rightX);
	}
	if (y < 0 || y >= _height) {
		return;
	}
	while (leftX++ <= rightX) {
		if (_pixelShader != nullptr) {
			_buffer.setPixel(_pixelShader(vec4(leftX, y, 0, 0)), leftX, y);
		}
	}
}

void Renderer::drawSpan(const Vertex& left, const Vertex& right, int y) {
	drawSpan(left.position.x, right.position.x, y, {255, 255, 0, 255});
}

void Renderer::edgeLoop(const Vertex& leftStart, const Vertex& rightStart, const Vertex& leftDest, const Vertex&rightDest, int numSteps) {
	for (int i = 0; i < numSteps; ++i) {
		float a = ((float)i)/numSteps;
		Vertex left = {(1.f-a)*leftStart.position + a*leftDest.position};
		Vertex right = {(1.f-a)*rightStart.position + a*rightDest.position};
		drawSpan(left, right, leftStart.position.y - i);
	}
}


void Renderer::edgeLoop(int numberOfSteps, int y, float leftX, float rightX, float leftStep, float rightStep, const Pixel& color) {
	assert(numberOfSteps >= 0);

	int yPos = y;
	while (numberOfSteps--) {
		int left = ceil(leftX);
		int right = ceil(rightX);
		drawSpan(floor(left), ceil(right), yPos--, color);
		leftX += leftStep;
		rightX += rightStep;
	}
}

bool Renderer::isTrianglePotentialVisible(const glm::vec2 (&verts)[3], const triangle& t) const {
	if (verts[t.rightIndex].x < _x ||
		verts[t.bottomIndex].y < _y ||
		verts[t.leftIndex].x >= _width ||
		verts[t.topIndex].y >= _height) {
		return false;
	}
	return true;
}
