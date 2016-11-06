#include "Renderer.hpp"
#include "LinearInterpolator.hpp"
#include <tuple>
#include <algorithm>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

using namespace renderlib;
using namespace glm;
using namespace std;

Renderer::Renderer(unsigned int width, unsigned int height) : _width(width), _height(height), _buffer(width, height), _clearColor({0, 0, 0, 255}) {
	
}

const Framebuffer& Renderer::frameBuffer() const {
	return _buffer;
}

void Renderer::setClearColor(const renderlib::Pixel &clearColor) {
	_clearColor = clearColor;
}

void Renderer::resize(unsigned int width, unsigned int height) {
	_width = width;
	_height = height;
	_buffer.resize(width, height);
}

void Renderer::setRenderFunc(std::function<void (Renderer&)> handler) {
	_renderFunction = handler;
}

void Renderer::setVertexBuffer(const vector<Vertex>& vertexBuffer) {
	_vertexBuffer = vertexBuffer;
}

void Renderer::setIndexBuffer(const vector<uint32_t>& indexBuffer) {
	_indexBuffer = indexBuffer;
}

void Renderer::setModelView(const glm::mat4& modelView) {
	_modelView = modelView;
}

void Renderer::render(void) {
	_buffer.fill(_clearColor);
	if (_renderFunction) {
		_renderFunction(*this);
	}
}

glm::mat4 camera(float Translate, glm::vec2 const & Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

void Renderer::drawTriangles(uint32_t firstVertexIndex, uint32_t count) {
	vector<glm::vec4> transformedPositions(_vertexBuffer.size());
	mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.f);
	mat4 m = projection * _modelView;
	
	for (unsigned int i = firstVertexIndex; i < firstVertexIndex+count; ++i) {
		uint32_t first = _indexBuffer[i];
		uint32_t second = _indexBuffer[i+1];
		uint32_t third = _indexBuffer[i+2];
		transformedPositions[first] = m * _vertexBuffer[first].position;
		transformedPositions[second] = m * _vertexBuffer[second].position;
		transformedPositions[third] = m * _vertexBuffer[third].position;

		vec2 verts[3] = {
			vec2(transformedPositions[first]),
			vec2(transformedPositions[second]),
			vec2(transformedPositions[third])
		};
		
		rasterizeTriangle(verts);
	}
}

void Renderer::rasterizeLine(const glm::vec2& start, const glm::vec2 &end, const Pixel& color) {
	vec2 drawStart(start), drawEnd(end);
	if (start.y > end.y) {
		swap(drawStart, drawEnd);
	}
	std::tie(drawStart, drawEnd) = clipLine(drawStart, drawEnd, _width, _height);
	vec2 delta = drawEnd-drawStart;
	bool interpolateVertically = fabs(delta.x) > fabs(delta.y);

	if (interpolateVertically) {
		if (drawStart.x > drawEnd.x) {
			std::swap(drawStart, drawEnd);
			delta = drawEnd-drawStart;
		}
		int linearValue = ceil(drawStart.x);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.y, subpixelAdjust(drawStart.x));
		
		do {
			_buffer.setPixel(color, linearValue++, ceil(lerp.interpolatedValue()));
		} while(lerp.interpolate());
	}
	else {
		int linearValue = ceil(drawStart.y);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.x, subpixelAdjust(drawStart.y));
		do {
			_buffer.setPixel(color, ceil(lerp.interpolatedValue()), linearValue++);
		} while(lerp.interpolate());
	}
}

void Renderer::rasterizeTriangle(const glm::vec2 (&verts)[3]) {
	triangle t = triangleFromVerts(verts);
	
	if (!isTrianglePotentialVisible(verts, t)) {
		return;
	}
/*	
	int y = ceil(verts[t.topIndex].y);
	float leftX = verts[t.topIndex].x + subpixelAdjust(verts[leftIndex].y)*t.;
	float rightX = verts[t.topIndex].x + glm::fract(verts[rightIndex].y)*rightStep;
	bool leftEdgeIsShort = leftDelta.y < rightDelta.y;
	float leftStep = leftDelta.x / leftDelta.y;
	float rightStep = rightDelta.x / rightDelta.y;
	
	float shortStep = leftEdgeIsShort ? leftStep : rightStep;
	float longStep = leftEdgeIsShort ? rightStep : leftStep;

	int shortSteps = leftEdgeIsShort ? ceil(leftDelta.y) : ceil(rightDelta.y);
	int longSteps = leftEdgeIsShort ? ceil(rightDelta.y) : ceil(leftDelta.y);

	int y = ceil(verts[leftIndex].y);
	
	if (y < 0) {
		int offset = abs(y);
		if (offset < longSteps) {
			return;	// invisible
		}
		y -= offset;
		longSteps -= offset;
		if (y < 0) {
			// short edge is not visible
			offset = abs(y);

			leftStep = leftDelta.x / leftDelta.y;
			edgeLoop(longSteps, 0, leftX, rightX, leftStep, rightStep);
		}
		leftX += offset*leftStep;
		rightX += offset*rightStep;
	}
	shortSteps = std::min((unsigned int)shortSteps, _height-y);
	
	std::tie(leftX, rightX) = edgeLoop(shortSteps, y, leftX, rightX, leftStep, rightStep);
	y += shortSteps;
	longSteps -= shortSteps;
	if (y >= _height || longSteps <= 0) {
		return;
	}

	longSteps = std::min((unsigned int)longSteps, _height-y);
	
	if (leftEdgeIsShort) {
		leftDelta = verts[nextIndex(rightIndex)] - verts[previousIndex(leftIndex)];
		leftStep = leftDelta.x / leftDelta.y;
	} else {
		rightDelta = verts[previousIndex(leftIndex)] - verts[nextIndex(rightIndex)];
		rightStep = rightDelta.x / rightDelta.y;
	}
	edgeLoop(longSteps, y, leftX, rightX, leftStep, rightStep);*/
}

void Renderer::drawSpan(int leftX, int rightX, int y) {
	assert(y >= 0 && y < _height);
	while (leftX++ <= rightX) {
		_buffer.setPixel({255,255,255,255}, leftX, y);
	}
}

std::tuple<float, float> Renderer::edgeLoop(int numberOfSteps, int y, float leftX, float rightX, float leftStep, float rightStep) {
	assert(numberOfSteps >= 0);

	while (numberOfSteps--) {
		int left = ceil(leftX);
		int right = ceil(rightX);
		drawSpan(std::min(left, right), std::max(left, right), y++);
		leftX += leftStep;
		rightX += rightStep;
	}
	return std::make_tuple(leftX, rightX);
}



bool Renderer::isTrianglePotentialVisible(const glm::vec2 (&verts)[3], const triangle& t) const {
	if (verts[t.rightIndex].x < 0 ||
		verts[t.bottomIndex].y < 0 ||
		verts[t.leftIndex].x >= _width ||
		verts[t.topIndex].y >= _height) {
		return false;
	}
	return true;
}
