#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_opengl.h>
#include <gl/GLU.h>

#pragma comment(lib, "SDL2")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

class App
{
public:
	App();
	virtual ~App();

	enum MOUSE_BUTTON {LEFT, MIDDLE, RIGHT};

	void setFPS(uint32_t fps);
	virtual bool init(const char *pTitle, uint32_t width, uint32_t height);
	virtual void finalize();

	virtual void update();
	virtual void preDraw();
	virtual void postDraw();
	virtual void draw();

	virtual bool pollingEvent();
	virtual bool keyHandler(int32_t key);
	virtual bool mouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, MOUSE_BUTTON button);
	virtual bool mouseButton(int32_t x, int32_t y, MOUSE_BUTTON button);
	virtual bool mouseWheel(int32_t x, int32_t y);
	virtual void resize(uint32_t width, uint32_t height);

protected:
	bool m_skipDraw;
	uint32_t m_interval;	// interval to draw
	uint32_t m_tickToDraw;
	SDL_Window* m_pWindow;
	SDL_GLContext m_context;
};

