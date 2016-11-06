#ifndef renderlib_h
#define renderlib_h

#include <cstdint>
#include <tuple>
#include <glm/glm.hpp>

namespace renderlib {
	struct Pixel {
		uint8_t r, g, b, a;
	};
	
	struct Vertex {
		glm::vec4 position;
	};
	
	struct triangle {
		unsigned int leftIndex;
		unsigned int rightIndex;
		unsigned int topIndex;
		unsigned int bottomIndex;
		unsigned int midIndex;
		glm::vec2 deltaA;
		glm::vec2 deltaB;
		glm::vec2 deltaC;
		bool leftSideIsC;
		bool leftAndRightOnTop;
		float stepOnC;
		float stepOnA;
		float stepOnB;
	};
	
	inline float subpixelAdjust(float x) {
		return std::ceil(x)-x;
	}

	inline unsigned int nextIndexInTriangle(unsigned int theIndex) {
		return theIndex < 2 ? theIndex+1 : 0;
	}
	
	inline unsigned int previousIndexInTriangle(unsigned int theIndex) {
		return theIndex > 0 ? theIndex-1 : 2;
	}
	
	inline std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> categorizedIndices(const glm::vec2 (&verts)[3]) {
		unsigned int top, bottom, left, right;
		float minX = FLT_MAX;
		float maxX = FLT_MIN;
		float minY = FLT_MAX;
		float maxY = FLT_MIN;
		
		for (unsigned int i = 0; i < 3; ++i) {
			if (verts[i].x < minX) {
				left = i;
				minX = verts[i].x;
			}
			if (verts[i].x > maxX) {
				right = i;
				maxX = verts[i].x;
			}
			if (verts[i].y < minY) {
				top = i;
				minY = verts[i].y;
			}
			if (verts[i].y > maxY) {
				bottom = i;
				maxY = verts[i].y;
			}
		}
		return std::make_tuple(left, top, right, bottom);
	}
	
	inline triangle triangleFromVerts(const glm::vec2 (&verts)[3]) {
		triangle tri;
		
		std::tie(tri.leftIndex, tri.topIndex, tri.rightIndex, tri.bottomIndex) = categorizedIndices(verts);
		
		tri.deltaC = verts[tri.bottomIndex] - verts[tri.topIndex];
		tri.leftSideIsC = previousIndexInTriangle(tri.topIndex) == tri.bottomIndex;
		tri.midIndex = tri.leftSideIsC ? nextIndexInTriangle(tri.topIndex) : previousIndexInTriangle(tri.topIndex);
		tri.leftAndRightOnTop = floor(verts[tri.topIndex].y) == floor(verts[tri.midIndex].y);
		tri.deltaA = tri.leftAndRightOnTop ? verts[tri.topIndex]: verts[tri.midIndex] - verts[tri.topIndex];
		tri.deltaB = verts[tri.bottomIndex] - verts[tri.midIndex];
		tri.stepOnC = tri.deltaC.x / tri.deltaC.y;
		tri.stepOnA = tri.deltaA.x / tri.deltaA.y;
		tri.stepOnB = tri.deltaB.x / tri.deltaB.y;
		return tri;
	}
	
	inline std::tuple<glm::vec2, glm::vec2> clipLine(const glm::vec2& start, const glm::vec2& end, unsigned int width, unsigned int height) {
		assert(start.y <= end.y);
		
		glm::vec2 clippedStart(start), clippedEnd(end);
		glm::vec2 direction = end-start;
		float stepX = direction.x / direction.y;
		float stepY = direction.y / direction.x;
		if (start.y < 0) {
			float offset = fabs(start.y);
			clippedStart.x = start.x + stepX*offset;
			clippedStart.y = 0;
		}
		if (clippedStart.x < 0) {
			float offset = fabs(start.x);
			clippedStart.x = 0;
			clippedStart.y = start.y + stepY*offset;
		}
		if (stepX >= stepY && end.x >= width) {
			float offset = (width-1) - start.x;
			clippedEnd.x = width-1;
			clippedEnd.y = start.y + offset*stepY;
		}
		if (stepX < stepY && clippedEnd.y >= height) {
			float offset = (height-1)-start.y;
			clippedEnd.x = start.x + stepX*offset;
			clippedEnd.y = height-1;
		}
		return std::make_tuple(clippedStart, clippedEnd);
	}
}

#endif /* renderlib_h */
