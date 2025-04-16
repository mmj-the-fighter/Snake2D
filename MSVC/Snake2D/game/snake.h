#ifndef _SNAKE_H_
#define _SNAKE_H_
#include <vector>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"
#include "segment.h"

class Snake
{
public:
	Snake(spn::SpinachCore* psc);
	void Init();
	void Update();
	void Render(spn::Canvas* canvas);
	/*void Grow();*/
	void ChangeHeadDirection(MoveDirection md);
	bool IsAlive() {
		return isAlive;
	};
private:
	std::vector<Segment> segments;
	bool isAlive;
	spn::SpinachCore* sc;
};
#endif