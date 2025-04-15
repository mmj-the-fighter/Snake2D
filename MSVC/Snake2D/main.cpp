#include <iostream>
#include "spinach/core/spn_canvas.h"
#include "spinach/core/spn_core.h"


int main(int argc, char* argv[])
{
	spn::SpinachCore sc(640, 480);
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetUpdateAndRenderHandler([](spn::Canvas* canvas){});
	sc.SetInputHandler([](const SDL_Event* sdlEvent) {});
	sc.SetWindowTitle("Example Game for Spinach");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.MainLoop();
	return 0;
}
