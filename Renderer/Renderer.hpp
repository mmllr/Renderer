#ifndef Renderer_hpp
#define Renderer_hpp

#include <cstddef>
#include <functional>
#include <glm/glm.hpp>
#include "Framebuffer.hpp"
#include "renderlib.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"

using namespace std;
using namespace glm;

namespace renderlib {

	class Renderer {
	public:
		Renderer(unsigned int width, unsigned int height);
		void setClearColor(const Pixel& clearColor);
		void setRenderFunc(std::function<void (Renderer&)> handler);
		void setVertexShader(std::function<Vertex (const Vertex& vertex)> vertexShader);
		void setPixelShader(std::function<vec4 (const Vertex& fragment)> pixelShader);
		void render(void);
		const Framebuffer& frameBuffer(void) const;
		void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		void setDepthRange(float nearZ, float farZ);
		void setVertexBuffer(const vector<Vertex>& vertexBuffer);
		void setIndexBuffer(const vector<uint32_t>& indexBuffer);
		void drawTriangles(uint32_t firstVertexIndex, uint32_t count);
		void setTexture(const Texture& t);
		void enablePerspectiveCorrection(void);
		void disablePerspectiveCorrection(void);
		void enableDepthTesting(void) { _shouldPerformDepthTest = true; }
		void disableDepthTesting(void) { _shouldPerformDepthTest = false; }
		float aspectRatio(void) const { return ((float)_width)/_height; };
	private:
		bool performDepthTest(int x, int y, float zPosition);
		vector<Vertex> transformAndClipTriangle(int startIndex);
		void rasterizeLine(const glm::vec2& start, const glm::vec2 &end, const Pixel& color);
		void rasterizeTriangle(const Vertex (&verts)[3]);
		void edgeLoop(const Vertex& leftStart, const Vertex& rightStart, const Vertex& leftDest, const Vertex&rightDest, int numSteps);
		std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> categorizedIndices(const Vertex (&verts)[3]) const;
		void drawSpan(int leftX, int rightX, int y, const Pixel& color);
		void drawSpan(const Vertex& left, const Vertex& right, float y);
		unsigned int _x, _y, _width, _height;
		float _nearZ, _farZ;
		Pixel _clearColor;
		Framebuffer _buffer;
		std::function<void (Renderer&)> _renderFunction;
		std::function<Vertex (const Vertex& vertex)> _vertexShader;
		std::function<vec4 (const Vertex& fragment)> _pixelShader;
		vector<Vertex> _vertexBuffer;
		vector<uint32_t> _indexBuffer;
		vector<Vertex> _clipVertexes;
		vector<Vertex> _ndcVertexes;
		vector<float> _depthBuffer;
		Texture _texture;
		bool _shouldPerformPerspectiveCorrection;
		bool _shouldPerformDepthTest;
	};
}

#endif /* Renderer_hpp */
