#ifndef renderlib_h
#define renderlib_h

#include <cstdint>
#include <tuple>
#include <limits>
#include <glm/glm.hpp>

namespace renderlib {
	struct Pixel {
		uint8_t r, g, b, a;
	};
	
	struct Vertex {
		glm::vec4 position;
		glm::vec4 color;
	};
	
	struct triangle {
		unsigned int leftIndex;
		unsigned int rightIndex;
		unsigned int topIndex;
		unsigned int bottomIndex;
		unsigned int midIndex;
		float deltaA;
		float deltaB;
		float deltaC;
		int heightOfA;
		int heightOfB;
		int heightOfC;
		bool leftSideIsC;
		bool leftAndRightOnTop;
		float stepOnC;
		float stepOnA;
		float stepOnB;
	};
	
	struct clipped_triangle {
		int y;
		int count;
		float leftX, rightX, leftStep, rightStep;
	};


	inline unsigned int nextIndexInTriangle(unsigned int theIndex) {
		return theIndex < 2 ? theIndex+1 : 0;
	}
	
	inline unsigned int previousIndexInTriangle(unsigned int theIndex) {
		return theIndex > 0 ? theIndex-1 : 2;
	}
	
	inline std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> categorizedIndices(const Vertex (&verts)[3]) {
		unsigned int top{0}, bottom{0}, left{0}, right{0};
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		
		for (unsigned int i = 0; i < 3; ++i) {
			if (verts[i].position.x < minX) {
				left = i;
				minX = verts[i].position.x;
			}
			if (verts[i].position.x > maxX) {
				right = i;
				maxX = verts[i].position.x;
			}
			if (verts[i].position.y > maxY) {
				top = i;
				maxY = verts[i].position.y;
			}
			if (verts[i].position.y < minY) {
				bottom = i;
				minY = verts[i].position.y;
			}
		}
		return std::make_tuple(left, top, right, bottom);
	}
	
	inline triangle triangleFromVerts(const Vertex (&verts)[3]) {
		triangle t;

		std::tie(t.leftIndex, t.topIndex, t.rightIndex, t.bottomIndex) = categorizedIndices(verts);
		t.leftSideIsC = previousIndexInTriangle(t.topIndex) == t.bottomIndex;
		t.midIndex = t.leftSideIsC ? nextIndexInTriangle(t.topIndex) : previousIndexInTriangle(t.topIndex);
		t.leftAndRightOnTop = floor(verts[t.topIndex].position.y) == floor(verts[t.midIndex].position.y);
		if (t.leftAndRightOnTop) {
			if (verts[t.topIndex].position.x > verts[t.midIndex].position.x) {
				int tmp = t.topIndex;
				t.topIndex = t.midIndex;
				t.midIndex = tmp;
			}
		}
		glm::vec4 c = verts[t.bottomIndex].position - verts[t.topIndex].position;
		glm::vec4 a = t.leftAndRightOnTop ? verts[t.bottomIndex].position - verts[nextIndexInTriangle(t.topIndex)].position : verts[t.midIndex].position - verts[t.topIndex].position;
		glm::vec4 b = verts[t.bottomIndex].position - verts[t.midIndex].position;

		t.deltaA = a.x;
		t.deltaB = b.x;
		t.deltaC = c.x;
		t.heightOfA = ceil(fabs(a.y));
		t.heightOfB = ceil(fabs(b.y));
		t.heightOfC = ceil(fabs(c.y));
		t.stepOnC = t.deltaC / t.heightOfC;
		t.stepOnA = t.deltaA / t.heightOfA;
		t.stepOnB = t.deltaB / t.heightOfB;
		return t;
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
	
	
	inline glm::vec4 convertNormalizedDeviceCoordateToWindow(const glm::vec4& v, float x, float y, float width, float height, float nearZ, float farZ) {
		width = width-1;
		height = height-1;
		return {
			v.x * (width/2) + (x + (width/2)), v.y * (height/2) + (y + (height/2)), ((farZ-nearZ)/2)*v.z + (farZ+nearZ)/2, v.w
		};
	}
	
	enum ClipPlane {
		left,
		right,
		top,
		bottom,
		near,
		far
	};
	
	inline bool isVertexInsidePlane(const glm::vec4 v, ClipPlane plane) {
		switch (plane) {
			case left: return (v.x >=-v.w);
			case right: return (v.x <= v.w);
			case top: return (v.y>=-v.w);
			case bottom: return (v.y<= v.w);
			case near: return (v.z>=0.0f);
			case far: return (v.z<= v.w);
		}
	}
	
	inline float interpolationFactor(float a, float b) {
		return a / (a-b);
	}
	
	inline Vertex intersectVertex(const Vertex& v0, const Vertex& v1, ClipPlane plane) {
		float a;
		const glm::vec4 & p0 = v0.position;
		const glm::vec4 & p1 = v1.position;
		switch (plane)	{
			case left:
				a = interpolationFactor(p0.w + p0.x, p1.w + p1.x);
				break;
			case right:
				a = interpolationFactor(p0.w - p0.x, p1.w - p1.x);
				break;
			case top:
				a = interpolationFactor(p0.w + p0.y, p1.w + p1.y);
				break;
			case bottom:
				a = interpolationFactor(p0.w - p0.y, p1.w - p1.y);
				break;
			case near:
				a = interpolationFactor(p0.z, p1.z);
				break;
			case far:
				a = interpolationFactor(p0.w - p0.z, p1.w - p1.z);
				break;
		};
		return {{(1.f-a) * p0 + a * p1}, {(1.f-a)*v0.color + a * v1.color}};
	}
	
	inline std::vector<Vertex> clipPolygonToPlane(const std::vector<Vertex>& verts, ClipPlane plane) {
		std::vector<Vertex> clippedPolygon;
		
		for (int i = 0; i < verts.size(); ++i) {
			int nextIndex = i == verts.size()-1 ? 0 : i+1;
			const Vertex& v0 = verts[i];
			const Vertex& v1 = verts[nextIndex];
			bool p0Visible = isVertexInsidePlane(v0.position, plane);
			bool p1Visible = isVertexInsidePlane(v1.position, plane);
			
			if (p0Visible != p1Visible) {
				clippedPolygon.push_back(intersectVertex(v0, v1, plane));
			}
			if (p1Visible) {
				clippedPolygon.push_back(v1);
			}
		}
		return clippedPolygon;
	}
	
	inline std::vector<Vertex> clipTriangleToFrustum(const std::vector<Vertex>& verts) {
		std::vector<Vertex> clippedPolygon = clipPolygonToPlane(verts, left);
		clippedPolygon = clipPolygonToPlane(clippedPolygon, right);
		clippedPolygon = clipPolygonToPlane(clippedPolygon, top);
		clippedPolygon = clipPolygonToPlane(clippedPolygon, bottom);
		clippedPolygon = clipPolygonToPlane(clippedPolygon, near);
		return clipPolygonToPlane(clippedPolygon, far);
	}

}

#endif /* renderlib_h */
