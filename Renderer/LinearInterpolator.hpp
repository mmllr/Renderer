#ifndef LinearInterpolator_hpp
#define LinearInterpolator_hpp

#include <algorithm>
#include <cmath>

namespace renderlib {
	struct LinearInterpolator {
	public:
		LinearInterpolator(float deltaA, float deltaB, float startValue, float offset=0) : _value(0) {
			float relation = std::min(fabs(deltaA), fabs(deltaB)) / std::max(fabs(deltaA), fabs(deltaB));
			_startValue = startValue + relation*offset;
			_step = 1.f / std::max(fabs(deltaA), fabs(deltaB));
			_delta = std::min(deltaA, deltaB);
			_numberOfSteps = floor(std::max(fabs(deltaA), fabs(deltaB)));
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
}


#endif /* LinearInterpolator_hpp */
