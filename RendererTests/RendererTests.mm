
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



@end
