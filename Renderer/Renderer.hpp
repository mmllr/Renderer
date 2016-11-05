#ifndef Renderer_hpp
#define Renderer_hpp

#include <cstddef>
#include <functional>
#include <glm/glm.hpp>
#include "Framebuffer.hpp"
#include "renderlib.hpp"

using namespace std;
using namespace glm;

namespace renderlib {

	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height);
		void setClearColor(const Pixel& clearColor);
		void setRenderFunc(std::function<void (Renderer&)> handler);
		void render(void);
		const Framebuffer& frameBuffer(void) const;
		void resize(unsigned int width, unsigned int height);
		void rasterizeLine(const glm::vec2& start, const glm::vec2 &end, const Pixel& color);
		void rasterizeTriangle(const glm::vec2 (&verts)[3]);
		void setVertexBuffer(const vector<Vertex>& vertexBuffer);
		void setIndexBuffer(const vector<uint32_t>& indexBuffer);
		void drawTriangles(uint32_t firstVertexIndex, uint32_t count);
	private:
		std::tuple<float, float> edgeLoop(unsigned int numberOfSteps, int y, float leftX, float rightX, float leftStep, float rightStep);
		void drawSpan(int leftX, int rightX, int y);
		unsigned int _width;
		unsigned int _height;
		Pixel _clearColor;
		Framebuffer _buffer;
		std::function<void (Renderer&)> _renderFunction;
		vector<Vertex> _vertexBuffer;
		vector<uint32_t> _indexBuffer;
	};
}

#endif /* Renderer_hpp */
