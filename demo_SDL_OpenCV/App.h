#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

class App
{
public:
	App();
	virtual ~App();

	void setFPS(uint32_t fps);
	virtual bool init(const char *pTitle, uint32_t width, uint32_t height);
	virtual void finalize();

	virtual void update();
	virtual void draw();
	virtual bool pollingEvent();

protected:
	bool m_skipDraw;
	uint32_t m_interval;	// interval to draw
	uint32_t m_tickToDraw;
	SDL_Window* m_pWindow;
	SDL_GLContext m_context;
};

