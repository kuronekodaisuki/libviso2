// demo_SDL_OpenCV.cpp : Defines the entry point for the console application.
//


#pragma warning(push)

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <viso_stereo.h>

#pragma warning(disable: 4819)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#pragma warning(pop)

#pragma comment(lib, "SDL2")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

static const std::string WINDOW_CAPTION = "SDL with OpenGL : projection test";
static const Uint32 WINDOW_WIDTH = 640;
static const Uint32 WINDOW_HEIGHT = 480;
static const Uint32 WINDOW_BPP = 32;

static const Uint32 FPS = 60;

static SDL_Window* window;
static SDL_GLContext context;

typedef enum {
	PROJ_2D,
	PROJ_3D
} eProjType;

bool init(eProjType projType = PROJ_3D);
bool finalize();

void update();
void draw();
bool pollingEvent();

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
	// initialize
	if (!init(PROJ_3D)) {
		std::cerr << "ERROR: failed to initialize SDL" << std::endl;
		exit(1);
	}

	// mainloop
	static const Uint32 interval = 1000 / FPS;
	static Uint32 nextTime = SDL_GetTicks() + interval;
	bool skipDraw = false;
	while (true) {
		// check event
		if (!pollingEvent()) break;

		// update and draw
		update();
		if (!skipDraw) {
			draw();
			SDL_GL_SwapWindow(window);
		}

		int delayTime = (int)(nextTime - SDL_GetTicks());
		if (delayTime > 0) {
			SDL_Delay((Uint32)delayTime);
			skipDraw = false;
		}
		else {
			// skip next draw step because of no time
			skipDraw = true;
		}

		nextTime += interval;
	}

	// finalize
	finalize();

	return 0;
}

bool init(eProjType projType) {
	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

	// enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// create indow
	// set SDL_WINDOW_OPENGL to use opengl for drawing
	window = SDL_CreateWindow(WINDOW_CAPTION.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	if (!window) return false;

	// create OpenGL context
	context = SDL_GL_CreateContext(window);
	if (!context) return false;

	// setup viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	// setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)WINDOW_WIDTH / (GLdouble)WINDOW_HEIGHT;
	if (projType == PROJ_2D) {
		glOrtho(-20.0*aspect, 20.0*aspect, -20.0, 20.0, 2.0, 200.0);
	}
	else {
		gluPerspective(45.0, aspect, 2.0, 200.0);
	}

	// setup light
	/*
	static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
	static GLfloat ambient [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat diffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	*/

	return true;
}

bool finalize() {
	// finalize SDL
	SDL_Quit();

	return true;
}

void update() {
}

void draw() {
	// clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, -30.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f);

	// draw polugon
	// near (z=10)
	glBegin(GL_POLYGON);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-17.5f, -2.5f, 10.0f);
		glVertex3f(-17.5f, 2.5f, 10.0f);
		glVertex3f(-12.5f, 2.5f, 10.0f);
		glVertex3f(-12.5f, -2.5f, 10.0f);
	}
	glEnd();

	// mid (z=50)
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-2.5f, -2.5f, 50.0f);
		glVertex3f(-2.5f, 2.5f, 50.0f);
		glVertex3f(2.5f, 2.5f, 50.0f);
		glVertex3f(2.5f, -2.5f, 50.0f);
	}
	glEnd();

	// near (z=90)
	glBegin(GL_POLYGON);
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(12.5f, -2.5f, 90.0f);
		glVertex3f(12.5f, 2.5f, 90.0f);
		glVertex3f(17.5f, 2.5f, 90.0f);
		glVertex3f(17.5f, -2.5f, 90.0f);
	}
	glEnd();
}

// polling event and execute actions
bool pollingEvent()
{
	SDL_Event ev;
	SDL_Keycode key;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type){
		case SDL_QUIT:
			// raise when exit event is occur
			return false;
			break;
		case SDL_KEYDOWN:
			// raise when key down
		{
			key = ev.key.keysym.sym;
			// ESC
			if (key == SDLK_ESCAPE){
				return false;
			}
		}
		break;
		}
	}
	return true;
}


