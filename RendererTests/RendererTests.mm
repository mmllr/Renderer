
#import <XCTest/XCTest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tuple>
#include <vector>
#include "renderlib.hpp"

using namespace glm;
using namespace renderlib;
using namespace std;

@interface RendererTests : XCTestCase

@end

@implementation RendererTests

- (void)testLineClippingWhenLineIsVisible {
	vec2 start = {10,10};
	vec2 end = {100, 100};
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	XCTAssertEqual(startClipped, start);
	XCTAssertEqual(endClipped, end);
}

- (void)testClipLineWithStartAboveTopBound {
	vec2 start = {10,-10};
	vec2 end = {100, 100};
	vec2 direction = end-start;
	float stepX = direction.x / direction.y;

	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedStart = {start.x + stepX*fabs(start.y), 0};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, end);	
}

- (void)testClipLineWithStartOutsideLeftBoound {
	vec2 start = {-10,10};
	vec2 end = {100, 100};
	vec2 direction = end-start;
	float stepY = direction.y / direction.x;

	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);

	vec2 expectedStart = {0, start.y + stepY*fabs(start.x)};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, end);	
}

- (void)testClipLineWithEndsRightToClippedWidth {
	vec2 start = {10,10};
	vec2 end = {210, 100};
	vec2 direction = end-start;
	float stepY = direction.y / direction.x;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedEnd = {199.f, start.y + stepY*(199.f-start.x)};
	XCTAssertEqual(startClipped, start);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testClipLineWithEndOutsideLowerBoound {
	vec2 start = {10,10};
	vec2 end = {100, 300};
	vec2 direction = end-start;
	float stepX = direction.x / direction.y;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedEnd = {start.x + stepX*(199.f-start.y), 199};
	XCTAssertEqual(startClipped, start);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testClipLineWithStartAndEndClippedToTopAndBottom {
	vec2 start = {10,-10};
	vec2 end = {100, 300};
	vec2 direction = end-start;
	float stepX = direction.x / direction.y;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedStart = {start.x + stepX*fabs(start.y), 0};
	vec2 expectedEnd = {start.x + stepX*(199.f-start.y), 199};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testClipLineWithStartAndEndClippedToLeftAndRight {
	vec2 start = {-40,10};
	vec2 end = {270, 100};
	vec2 direction = end-start;
	float stepY = direction.y / direction.x;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedStart = {0, start.y + stepY*fabs(start.x)};
	vec2 expectedEnd = {199.f, start.y + stepY*(199.f-start.x)};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testClippingLineStartingOutsideLeftAndEndingOutsideBottom {
	vec2 start = {-40,-10};
	vec2 end = {270, 230};
	vec2 direction = end-start;
	float stepY = direction.y / direction.x;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedStart = {0, start.y + stepY*fabs(start.x)};
	vec2 expectedEnd = {199.f, start.y + stepY*(199.f-start.x)};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testClippingLineStartingOutsideTopAndEndingOutsideRight {
	vec2 start = {-10,-20};
	vec2 end = {1530, 210};
	vec2 direction = end-start;
	float stepX = direction.x / direction.y;
	float stepY = direction.y / direction.x;
	
	vec2 startClipped, endClipped;
	std::tie(startClipped, endClipped) = clipLine(start, end, 200, 200);
	
	vec2 expectedStart = {start.x + stepX*fabs(start.y), 0};
	vec2 expectedEnd = {199.f, start.y + stepY*(199.f-start.x)};
	XCTAssertEqual(startClipped, expectedStart);
	XCTAssertEqual(endClipped, expectedEnd);
}

- (void)testIndexCategorizationWithLongEdgeLeft {
	glm::vec2 verts[] = {
		{10, 10}, {320, 20}, {250, 130}
	};
	
	triangle t = triangleFromVerts(verts);
	
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaA = verts[1]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];
	
	XCTAssertEqual(t.leftIndex, 0);
	XCTAssertEqual(t.topIndex, 0);
	XCTAssertEqual(t.rightIndex, 1);
	XCTAssertEqual(t.bottomIndex, 2);
	XCTAssertEqual(t.midIndex, 1);
	XCTAssertTrue(t.leftSideIsC);
	XCTAssertFalse(t.leftAndRightOnTop);
	XCTAssertEqual(expectedDeltaC, t.deltaC);
	XCTAssertEqual(expectedDeltaA, t.deltaA);
	XCTAssertEqual(expectedDeltaB, t.deltaB);
	XCTAssertEqualWithAccuracy(expectedDeltaC.x / expectedDeltaC.y, t.stepOnC, .0001);
	XCTAssertEqualWithAccuracy(expectedDeltaA.x / expectedDeltaA.y, t.stepOnA, .0001);
	XCTAssertEqualWithAccuracy(expectedDeltaB.x / expectedDeltaB.y, t.stepOnB, .0001);
}

- (void)testIndexCategorizationWithLongEdgeRight {
	glm::vec2 verts[] = {
		{100, 50}, {50, 10}, {180, 130}
	};
	
	triangle t = triangleFromVerts(verts);
	
	vec2 expectedDeltaC = verts[2]-verts[1];
	vec2 expectedDeltaA = verts[0]-verts[1];
	vec2 expectedDeltaB = verts[2]-verts[0];
	
	XCTAssertEqual(t.leftIndex, 1);
	XCTAssertEqual(t.topIndex, 1);
	XCTAssertEqual(t.rightIndex, 2);
	XCTAssertEqual(t.bottomIndex, 2);
	XCTAssertEqual(t.midIndex, 0);
	XCTAssertFalse(t.leftSideIsC);
	XCTAssertFalse(t.leftAndRightOnTop);
	XCTAssertEqual(expectedDeltaC, t.deltaC);
	XCTAssertEqual(expectedDeltaA, t.deltaA);
	XCTAssertEqual(expectedDeltaB, t.deltaB);
	XCTAssertEqualWithAccuracy(expectedDeltaC.x / expectedDeltaC.y, t.stepOnC, .0001);
	XCTAssertEqualWithAccuracy(expectedDeltaA.x / expectedDeltaA.y, t.stepOnA, .0001);
	XCTAssertEqualWithAccuracy(expectedDeltaB.x / expectedDeltaB.y, t.stepOnB, .0001);
}

- (void)testIndexCategorizationWithLeftAndRightOnTop {
	glm::vec2 verts[] = {
		{50, 10}, {100, 10}, {150, 120}
	};
	
	triangle t = triangleFromVerts(verts);
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];

	XCTAssertEqual(t.leftIndex, 0);
	XCTAssertEqual(t.topIndex, 0);
	XCTAssertEqual(t.midIndex, 1);
	XCTAssertEqual(t.rightIndex, 1);
	XCTAssertEqual(t.bottomIndex, 2);
	XCTAssertTrue(t.leftAndRightOnTop);
	XCTAssertTrue(t.leftSideIsC);
	XCTAssertEqual(expectedDeltaC, t.deltaC);
	XCTAssertEqual(expectedDeltaB, t.deltaB);
	XCTAssertEqualWithAccuracy(expectedDeltaC.x / expectedDeltaC.y, t.stepOnC, .0001);
	XCTAssertEqualWithAccuracy(expectedDeltaB.x / expectedDeltaB.y, t.stepOnB, .0001);
}

- (void)testClipTriangleWhichIsCompletlyVisible {
	glm::vec2 verts[] = {
		{100, 10}, {50, 100}, {150, 120}
	};
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaA = verts[1]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];
	
	float expectedAStep = expectedDeltaA.x / expectedDeltaA.y;
	float expectedBStep = expectedDeltaB.x / expectedDeltaB.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	float expectedMidXOnC = 100.f + (expectedDeltaA.x / expectedDeltaC.x)*90;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 2);
	XCTAssertEqual(v[0].y, 10);
	XCTAssertEqual(v[0].count, 90);
	XCTAssertEqualWithAccuracy(v[0].leftX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedAStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
	XCTAssertEqual(v[1].y, 100);
	XCTAssertEqual(v[1].count, 20);
	XCTAssertEqualWithAccuracy(v[1].leftX, 50, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightX, expectedMidXOnC, .0001);
	XCTAssertEqualWithAccuracy(v[1].leftStep, expectedBStep, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightStep, expectedCStep, .0001);
}

- (void)testClipTriangleWhichIsClippedOnTopVertex {
	glm::vec2 verts[] = {
		{100, -10}, {50, 100}, {150, 120}
	};
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaA = verts[1]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];
	
	float expectedAStep = expectedDeltaA.x / expectedDeltaA.y;
	float expectedBStep = expectedDeltaB.x / expectedDeltaB.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	float expectedLeftX = 100.f + expectedAStep*10;
	float expectedRightX = 100.f + expectedCStep*10;
	float expectedMidXOnC = 100.f + (expectedDeltaA.x / expectedDeltaC.x)*110;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 2);
	XCTAssertEqual(v[0].y, 0);
	XCTAssertEqual(v[0].count, 100);
	XCTAssertEqualWithAccuracy(v[0].leftX, expectedLeftX, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, expectedRightX, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedAStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
	XCTAssertEqual(v[1].y, 100);
	XCTAssertEqual(v[1].count, 20);
	XCTAssertEqualWithAccuracy(v[1].leftX, 50, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightX, expectedMidXOnC, .0001);
	XCTAssertEqualWithAccuracy(v[1].leftStep, expectedBStep, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightStep, expectedCStep, .0001);
}

- (void)testClipTriangleWhereTheTopAndTheMidVertexIsClippedOnTop {
	glm::vec2 verts[] = {
		{100, -30}, {50, -10}, {150, 120}
	};
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];
	
	float expectedBStep = expectedDeltaB.x / expectedDeltaB.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	float expectedLeftX = 50.f + expectedBStep*10;
	float expectedRightX = 100.f + expectedCStep*30;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 1);
	XCTAssertEqual(v[0].y, 0);
	XCTAssertEqual(v[0].count, 120);
	XCTAssertEqualWithAccuracy(v[0].leftX, expectedLeftX, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, expectedRightX, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedBStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
}

- (void)testClipTriangleWhereTheBottomVertexIsClipped {
	glm::vec2 verts[] = {
		{100, 10}, {50, 100}, {150, 220}
	};
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaA = verts[1]-verts[0];
	vec2 expectedDeltaB = verts[2]-verts[1];
	
	float expectedAStep = expectedDeltaA.x / expectedDeltaA.y;
	float expectedBStep = expectedDeltaB.x / expectedDeltaB.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	float expectedMidXOnC = 100.f + (expectedDeltaA.x / expectedDeltaC.x)*90;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 2);
	XCTAssertEqual(v[0].y, 10);
	XCTAssertEqual(v[0].count, 90);
	XCTAssertEqualWithAccuracy(v[0].leftX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedAStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
	XCTAssertEqual(v[1].y, 100);
	XCTAssertEqual(v[1].count, 100);
	XCTAssertEqualWithAccuracy(v[1].leftX, 50, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightX, expectedMidXOnC, .0001);
	XCTAssertEqualWithAccuracy(v[1].leftStep, expectedBStep, .0001);
	XCTAssertEqualWithAccuracy(v[1].rightStep, expectedCStep, .0001);
}

- (void)testClipTriangleWhereTheMidAndTheBottomVertexIsClipped {
	glm::vec2 verts[] = {
		{100, 10}, {50, 210}, {150, 250}
	};
	vec2 expectedDeltaC = verts[2]-verts[0];
	vec2 expectedDeltaA = verts[1]-verts[0];
	
	float expectedAStep = expectedDeltaA.x / expectedDeltaA.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 1);
	XCTAssertEqual(v[0].y, 10);
	XCTAssertEqual(v[0].count, 190);
	XCTAssertEqualWithAccuracy(v[0].leftX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedAStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
}

- (void)testClipTriangleWhichIsCompletlyVisibleWithTopAndMidVertexOnTopEdge {
	glm::vec2 verts[] = {
		{50, 10}, {100, 10}, {150, 120}
	};
	vec2 expectedDeltaC = verts[2]-verts[1];
	vec2 expectedDeltaA = verts[2]-verts[0];
	
	float expectedAStep = expectedDeltaA.x / expectedDeltaA.y;
	float expectedCStep = expectedDeltaC.x / expectedDeltaC.y;
	
	std::vector<clipped_triangle> v = clipTriangleToBounds(verts, 200, 200);
	XCTAssertEqual(v.size(), 1);
	XCTAssertEqual(v[0].y, 10);
	XCTAssertEqual(v[0].count, 110);
	XCTAssertEqualWithAccuracy(v[0].leftX, 50, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightX, 100, .0001);
	XCTAssertEqualWithAccuracy(v[0].leftStep, expectedAStep, .0001);
	XCTAssertEqualWithAccuracy(v[0].rightStep, expectedCStep, .0001);
}

glm::mat4 adjoint(const glm::mat4& m) {
	mat4 a;
	float d = glm::determinant(m);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			a[i][j] = pow(-1, (i)+(j))*d*m[i][j];
		}
	}
	return a;
}

- (void)testTransposeMatrix {
	glm::mat4 m = glm::rotate(mat4(), glm::radians(30.f), {0.f,1.f, 0.f});
	
	glm::mat4 n = transpose(inverse(m));
	mat4 a = adjoint(m);
	XCTAssertEqual(a, n);
}



vector<vec4> givenClippedVerts(const vector<vec4>& verts, const mat4& m) {
	vector<vec4> newVerts;
	for (auto&& v : verts) {
		newVerts.push_back(m*v);
	}
	return newVerts;
}

- (void)testClipPolygonWhichIsInsideViewFrustum {
	vector<vec4> verts = givenClippedVerts({
		{-.25f, .25f, -1, 1},
		{.25f, .25f, -1, 1},
		{0.f, -.25f, -1, 1}
	}, glm::perspective(glm::radians(60.0f), 4.f/3.f, 1.f, 100.f));

	vector<vec4> clipped = clipTriangleToFrustum(verts);
	XCTAssertEqual(clipped, verts);
	
	/*
	vec3 left = {-1,0,0};
	float d0 = dot(vec3(clipped[0]), left);
	float d1 = dot(vec3(clipped[1]), left);
	float a = d0 / (d0-d1);
	vec4 newPoint = (1.f-a) * clipped[0] + a * clipped[1];*/
}

- (void)testClippingAPolygoneWhichIsOutsideTheViewFrustum {
	vector<vec4> verts = givenClippedVerts({
		{-1.25f, .25f, -1, 1},
		{.25f, 1.25f, -1, 1},
		{0.f, -1.25f, -1, 1}
	},glm::perspective(glm::radians(60.0f), 4.f/3.f, 1.f, 100.f));
	
	vector<vec4> clipped = clipTriangleToFrustum(verts);

	XCTAssertEqual(clipped, vector<vec4>({}));
}


@end
