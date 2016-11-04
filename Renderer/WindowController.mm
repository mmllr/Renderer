#import "WindowController.h"
#import "FramebufferView.h"
#include "Framebuffer.hpp"
#include <glm/glm.hpp>

using namespace Renderer;
using namespace glm;

@interface WindowController ()
@property (weak) IBOutlet FramebufferView *frameBufferView;

@end

@implementation WindowController

- (void)awakeFromNib {
	[super awakeFromNib];
	
	self.frameBufferView.renderHandler = ^(Framebuffer &framebuffer) {
		DrawLine({70, 50}, {40, 500}, {255, 255, 255, 255}, framebuffer);
		DrawLine({70, 50}, {400, 600}, {255, 255, 255, 255}, framebuffer);
		DrawLine({400, 600}, {40, 500}, {255, 255, 255, 255}, framebuffer);
	};

}

void DrawLine(const glm::vec2& start, const glm::vec2 &end, const Pixel& color, Framebuffer& framebuffer) {
	vec2 drawStart(start), drawEnd(end);
	vec2 delta = end-start;
	bool interpolateVertically = fabs(delta.x) > fabs(delta.y);
	
	if (interpolateVertically) {
		if (start.x > end.x) {
			std::swap(drawStart, drawEnd);
			delta = drawEnd-drawStart;
		}
		int linearValue = ceil(drawStart.x);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.y, adjustForSubPixels(drawStart.x));
		
		do {
			framebuffer.setPixel(color, linearValue++, ceil(lerp.interpolatedValue()));
		} while(lerp.interpolate());
	}
	else {
		if (start.y > end.y) {
			std::swap(drawStart, drawEnd);
			delta = drawEnd-drawStart;
		}
		int linearValue = ceil(start.y);
		LinearInterpolator lerp(delta.x, delta.y, drawStart.x, adjustForSubPixels(drawStart.y));
		
		do {
			framebuffer.setPixel(color, ceil(lerp.interpolatedValue()), linearValue++);
		}
		while (lerp.interpolate());
	}
}

struct LinearInterpolator {
public:
	LinearInterpolator(float deltaA, float deltaB, float startValue, float offset=0) : _value(0) {
		float relation = std::min(fabs(deltaA), fabs(deltaB)) / std::max(fabs(deltaA), fabs(deltaB));
		_startValue = startValue + relation*offset;
		_step = 1.f / std::max(fabs(deltaA), fabs(deltaB));
		_delta = std::min(deltaA, deltaB);
		_numberOfSteps = ceil(std::max(fabs(deltaA), fabs(deltaB)));
	}
	float t(void) const { return _value; }
	float interpolatedValue(void) const { return _startValue + _delta*t(); }
	unsigned int interpolate(void) {
		_value += _step;
		return _numberOfSteps--;
	}
private:
	unsigned int _numberOfSteps;
	float _step;
	float _value;
	float _startValue;
	float _delta;
};

inline float adjustForSubPixels(float value) {
	return ceil(value) - value;
}

@end
