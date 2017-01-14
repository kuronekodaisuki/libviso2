// demo_SDL_OpenCV.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <viso_stereo.h>

#pragma warning(push)
#pragma warning(disable: 4819)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

#include "VisualOdometryApp.h"

static const char WINDOW_CAPTION[] = "SDL with OpenGL : projection test";
static const Uint32 WINDOW_WIDTH = 640;
static const Uint32 WINDOW_HEIGHT = 480;
static const Uint32 WINDOW_BPP = 32;
static const Uint32 FPS = 60;

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
	VisualOdometryApp app;
	// initialize
	if (!app.init(WINDOW_CAPTION, WINDOW_WIDTH, WINDOW_HEIGHT)) {
		std::cerr << "ERROR: failed to initialize SDL" << std::endl;
		exit(1);
	}
	app.setFPS(FPS);
	// mainloop
	while (true) {
		// check event
		if (!app.pollingEvent()) break;

		// update and draw
		app.update();
	}
	// finalize
	app.finalize();

	return 0;
}



