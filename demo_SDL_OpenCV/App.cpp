//
//
//

#include "App.h"
#include <stdio.h>


#pragma comment(lib, "SDL2")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

App::App()
{
	m_interval = 1000 / 30;
}

App::~App()
{
}

void App::setFPS(uint32_t fps)
{
	m_interval = 1000 / fps;
}

bool App::init(const char *pTitle, uint32_t width, uint32_t height)
{
	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

	// enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// create indow
	// set SDL_WINDOW_OPENGL to use opengl for drawing
	m_pWindow = SDL_CreateWindow(pTitle,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_OPENGL);
	if (!m_pWindow) return false;

	// create OpenGL context
	m_context = SDL_GL_CreateContext(m_pWindow);
	if (!m_context) return false;

	// setup viewport
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	// setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	//if (projType == PROJ_2D) {
	//	glOrtho(-20.0*aspect, 20.0*aspect, -20.0, 20.0, 2.0, 200.0);
	//}
	//else {
		gluPerspective(45.0, aspect, 2.0, 200.0);
	//}

	m_skipDraw = false;
	m_tickToDraw = SDL_GetTicks() + m_interval;
	return true;
}

void App::finalize()
{
	// finalize SDL
	SDL_Quit();
}

void App::update()
{
	if (!m_skipDraw) {
		draw();
	}

	int delayTime = (int)(m_tickToDraw - SDL_GetTicks());
	if (delayTime > 0) {
		SDL_Delay((Uint32)delayTime);
		m_skipDraw = false;
	}
	else {
		// skip next draw step because of no time
		m_skipDraw = true;
	}

	m_tickToDraw += m_interval;
}

void App::preDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void App::postDraw()
{
	SDL_GL_SwapWindow(m_pWindow);
}

void App::draw()
{
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

	SDL_GL_SwapWindow(m_pWindow);
}

bool App::keyHandler(int32_t key)
{
	// ESC
	if (key == SDLK_ESCAPE){
		return false;
	}
	return true;
}

bool App::mouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, MOUSE_BUTTON button)
{
	//printf("%d %d %d %d\n", x, y, dx, dy);
	return false;
}

bool App::mouseButton(int32_t x, int32_t y, MOUSE_BUTTON button)
{
	return false;
}

bool App::mouseWheel(int32_t x, int32_t y)
{
	return false;
}

void App::resize(uint32_t width, uint32_t height)
{

}

bool App::pollingEvent()
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
			return keyHandler(key);
		}

		case SDL_WINDOWEVENT:
			if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				resize(ev.window.data1, ev.window.data2);
				//repaint = true;
			}
			break;

		case SDL_MOUSEMOTION:
			return mouseMove(ev.motion.x, ev.motion.y, ev.motion.xrel, ev.motion.yrel, (MOUSE_BUTTON)ev.button.button);
			break;

		case SDL_MOUSEBUTTONDOWN:
			return mouseButton(ev.motion.x, ev.motion.y, (MOUSE_BUTTON)ev.button.button);

		case SDL_MOUSEWHEEL:
			return mouseWheel(ev.wheel.x, ev.wheel.y);
			break;
		}
	}
	return true;
}