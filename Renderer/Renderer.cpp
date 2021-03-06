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

Renderer::Renderer(unsigned int width, unsigned int height) : _x(0), _y(0), _width(width), _height(height), _nearZ(0), _farZ(1), _buffer(width, height), _depthBuffer(width*height), _clearColor({0, 0, 0, 255}), _shouldPerformPerspectiveCorrection(true), _shouldPerformDepthTest(true), _shouldPerformCulling(true) {
	
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
	_depthBuffer.resize(width*height);
}

void Renderer::setDepthRange(float nearZ, float farZ) {
	_nearZ = nearZ;
	_farZ = farZ;
}

void Renderer::setRenderFunc(std::function<void (Renderer&)> handler) {
	_renderFunction = handler;
}

void Renderer::setVertexShader(std::function<Vertex (const Vertex& vertex)> vertexShader) {
	_vertexShader = vertexShader;
}

void Renderer::setPixelShader(std::function<vec4 (const Vertex& fragment)> pixelShader) {
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

void Renderer::setTexture(const Texture& texture) {
	_texture = texture;
}

void Renderer::enablePerspectiveCorrection(void) {
	_shouldPerformPerspectiveCorrection = true;
}

void Renderer::disablePerspectiveCorrection(void) {
	_shouldPerformPerspectiveCorrection = false;
}

void Renderer::render(void) {
	_buffer.fill(_clearColor);
	std::fill_n(_depthBuffer.begin(), _depthBuffer.size(), 1);
	if (_renderFunction) {
		_renderFunction(*this);
	}
}

vector<Vertex> Renderer::transformAndClipTriangle(int startIndex) {
	uint32_t first = _indexBuffer[startIndex];
	uint32_t second = _indexBuffer[startIndex+1];
	uint32_t third = _indexBuffer[startIndex+2];
	_clipVertexes[first] = _vertexShader(_vertexBuffer[first]);
	_clipVertexes[second] = _vertexShader(_vertexBuffer[second]);
	_clipVertexes[third] = _vertexShader(_vertexBuffer[third]);
	
	return clipTriangleToFrustum({_clipVertexes[first], _clipVertexes[second], _clipVertexes[third]});
}

void Renderer::drawTriangles(uint32_t firstVertexIndex, uint32_t count) {
	for (unsigned int i = 0; i < count*3; i += 3) {
		// transforming to clip space
		vector<Vertex> clippedPoly = transformAndClipTriangle(firstVertexIndex+i);
		
		if (clippedPoly.size() < 3) {
			continue;
		}
		// perspective projection &
		// transform from normalized device coordinates to window coordiates and render triangle strip after clipping
		_ndcVertexes.resize(clippedPoly.size());
		for (int p = 0; p < clippedPoly.size(); ++p) {
			float oneOverW = 1./clippedPoly[p].position.w;
			_ndcVertexes[p].position = convertNormalizedDeviceCoordateToWindow(clippedPoly[p].position*oneOverW, _x, _y, _width, _height, _nearZ, _farZ);
			_ndcVertexes[p].position.w = oneOverW;
			_ndcVertexes[p].color = _shouldPerformPerspectiveCorrection ? clippedPoly[p].color*oneOverW : clippedPoly[p].color;
			_ndcVertexes[p].texCoords = _shouldPerformPerspectiveCorrection ? clippedPoly[p].texCoords*oneOverW : clippedPoly[p].texCoords;
		}

		if (_shouldPerformCulling && cullFace(_ndcVertexes[0].position, _ndcVertexes[1].position, _ndcVertexes[2].position)) {
			continue;
		}
		for (int p = 1; p < clippedPoly.size()-1; ++p) {
			rasterizeTriangle({_ndcVertexes[0], _ndcVertexes[p], _ndcVertexes[p+1]});
		}
	}
}

void Renderer::rasterizeTriangle(const Vertex (&verts)[3]) {
	triangle t = triangleFromVerts(verts);
	
	if (t.leftAndRightOnTop) {
		edgeLoop(verts[t.topIndex], verts[t.midIndex], verts[t.bottomIndex], verts[t.bottomIndex], t.heightOfC);
		return;
	}
	Vertex vOnC = clipVertex(verts[t.topIndex], verts[t.bottomIndex], ((float)t.heightOfA)/t.heightOfC);
	edgeLoop(verts[t.topIndex], verts[t.topIndex], t.leftSideIsC ? vOnC : verts[t.midIndex], t.leftSideIsC ? verts[t.midIndex] : vOnC, t.heightOfA);
	edgeLoop(t.leftSideIsC ? vOnC : verts[t.midIndex], t.leftSideIsC ? verts[t.midIndex] : vOnC, verts[t.bottomIndex], verts[t.bottomIndex], t.heightOfB);
}

void Renderer::edgeLoop(const Vertex& leftStart, const Vertex& rightStart, const Vertex& leftDest, const Vertex&rightDest, int numSteps) {
	for (int i = 0; i < numSteps; ++i) {
		float a = ((float)i)/numSteps;
		drawSpan(clipVertex(leftStart, leftDest, a), clipVertex(rightStart, rightDest, a), leftStart.position.y - i);
	}
}

void Renderer::drawSpan(const Vertex& left, const Vertex& right, float y) {
	Vertex drawLeft(left);
	Vertex drawRight(right);
	if (left.position.x > right.position.x) {
		std::swap(drawLeft, drawRight);
	}
	if (y < 0 || y >= _height) {
		return;
	}
	int startX = std::max(floor(drawLeft.position.x), 0.f);
	int drawY = floor(y);
	int width = ceil(drawRight.position.x) - startX;
	for (int i = 0; i < width; ++i) {
		int drawX = startX+i;
		float a = ((float)i)/width;
		if (_pixelShader != nullptr) {
			Vertex fragment = clipVertex(drawLeft, drawRight, a);
			if (_shouldPerformDepthTest && !performDepthTest(drawX, drawY, fragment.position.z)) {
				continue;
			}
			if (_shouldPerformPerspectiveCorrection) {
				// apply perspective correction (interpolation in screen space is done with texCoords/w and color/w
				fragment.color /= fragment.position.w;
				fragment.texCoords /= fragment.position.w;
			}
			vec4 color = _pixelShader(fragment);
			_buffer.setPixel({static_cast<uint8_t>(color.r*255), static_cast<uint8_t>(color.g*255), static_cast<uint8_t>(color.b*255), static_cast<uint8_t>(color.a*255)}, drawX, drawY);
		}
	}
}

bool Renderer::performDepthTest(int x, int y, float zPosition) {
	assert(x < _width);
	assert(y < _height);
	
	if (_depthBuffer[y*_width+x] >= zPosition) {
		_depthBuffer[y*_width+x] = zPosition;
		return true;
	}
	return false;
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
