
#import <XCTest/XCTest.h>
#include <glm/glm.hpp>
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
		{10, 10}, {200, 10}, {100, 130}
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

struct clipped_triangle {
	int y;
	int count;
	float leftX, rightX, leftStep, rightStep;
};

vector<clipped_triangle> clipTriangleToBounds(const glm::vec2 (&verts)[3], unsigned int width, unsigned int height) {
	vector<clipped_triangle> v;

	triangle t = triangleFromVerts(verts);

	if (verts[t.midIndex].y < 0) {
		int offset = abs(verts[t.midIndex].y);
		int count = t.deltaB.y - offset;
		float leftX = verts[t.midIndex].x + t.stepOnB*offset;
		float rightX = verts[t.topIndex].x + t.stepOnC*(offset+t.deltaA.y);
		return {{0, count, leftX, rightX, t.stepOnB, t.stepOnC }};
	}
	int y = std::ceil(verts[t.topIndex].y);
	int count = t.deltaA.y;
	float leftX = verts[t.topIndex].x;
	float rightX = verts[t.topIndex].x;
	if (verts[t.midIndex].y >= height) {
		count = height-y;
		return {{y, count, leftX, rightX, t.stepOnA, t.stepOnC }};
	}
	if (y < 0) {
		int offset = abs(y);
		y += offset;
		count -= offset;
		leftX += t.stepOnA*offset;
		rightX += t.stepOnC*offset;
	}
	clipped_triangle top = {y, count, leftX, rightX, t.stepOnA, t.stepOnC };
	v.push_back(top);
	clipped_triangle bottom = {static_cast<int>(ceil(verts[t.midIndex].y)), static_cast<int>(std::min(t.deltaB.y, height-verts[t.midIndex].y)), verts[t.midIndex].x, verts[t.topIndex].x + t.deltaA.y*(t.deltaA.x/t.deltaC.x), t.stepOnB, t.stepOnC };
	v.push_back(bottom);
	return v;
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

@end
