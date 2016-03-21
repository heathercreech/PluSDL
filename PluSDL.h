#pragma once

#include <vector>
#include <map>
#include <functional>

#include <SDL2\SDL.h>


typedef std::function<void()> SimpleFunction;
typedef std::function<void(SDL_Event)> EventFunction;
typedef Uint32 EventType;

const int PLUSDL_DEFAULT_WINDOW_WIDTH = 640;
const int PLUSDL_DEFAULT_WINDOW_HEIGHT = 480;
const Uint32 PLUSDL_DEFAULT_FLAGS = SDL_WINDOW_SHOWN;


//Wrapper for SDL_Rect
class AppRect {
public:
	AppRect(int, int, int = 0, int = 0);

	void resize(int, int);
	void updatePosition(int, int);

	int getX() { return rect.x; };
	int getY() { return rect.y; };

	int getWidth() { return rect.w; };
	int getHeight() { return rect.h; };

	explicit operator SDL_Rect*() { return this->get(); };

	SDL_Rect* get() { return &rect; };

private:
	SDL_Rect rect;
};

//Wrapper for SDL_Surface
class AppSurface {
public:
	AppSurface();
	AppSurface(SDL_Surface* s) { surface = s; };
	~AppSurface();

	operator bool() const;
	explicit operator SDL_Surface*() { return this->get(); };

	SDL_Surface* get() { return surface; };
	void set(SDL_Surface* s) { surface = s; };

private:
	SDL_Surface* surface;
};

//Wrapper for SDL_Texture
class AppTexture {

};

//Controls initialization and destruction of SDL's subsystems
class AppInitializer {
public:
	AppInitializer();
	AppInitializer(std::initializer_list<int>);
	~AppInitializer();

	void initializeVideo() { initializeSystem(SDL_INIT_VIDEO); };
	void initializeAudio() { initializeSystem(SDL_INIT_AUDIO); };

	operator bool() const;

private:
	void initializeSystem(int);
	void quitSystems();

	std::vector<int> initialized;

	bool good = true;
};

//Wrapper for SDL_Window
class AppWindow {
public:
	AppWindow(int = PLUSDL_DEFAULT_WINDOW_WIDTH, int = PLUSDL_DEFAULT_WINDOW_HEIGHT, Uint32 = PLUSDL_DEFAULT_FLAGS);

	~AppWindow();

	operator bool() const;
	explicit operator SDL_Window*() { return this->get(); };

	SDL_Window* get() { return window; };
	SDL_Surface* getSurface() { return surface.get(); };

private:
	void createWindow(int, int, Uint32); //simple wrapper for SDL_CreateWindow
	SDL_Window* window;
	AppSurface surface;
};

//Wrapper for SDL_Renderer
class AppRenderer {
public:
	AppRenderer(AppWindow&);

	~AppRenderer();

	operator bool() const;
	explicit operator SDL_Renderer*() { return this->get(); };

	SDL_Renderer* get() { return renderer; };

	void clear();
	void update();

private:
	SDL_Renderer* renderer;
};

//Handles calling of functions in response to SDLs events
class AppEventManager {
public:
	AppEventManager();
	~AppEventManager();

	void registerEventFunction(EventType, EventFunction);
	void registerSimpleFunction(EventType, SimpleFunction);
	void callFunction(SDL_Event);
	void checkForEvent();

private:
	bool isHandled(EventType);

	std::map<EventType, EventFunction> event_functions;
	std::map<EventType, SimpleFunction> simple_functions;
	EventFunction default_function;
};

//Container for the elements of the most basic of graphics application
class App {
public:
	static void init(int = PLUSDL_DEFAULT_WINDOW_WIDTH, int = PLUSDL_DEFAULT_WINDOW_HEIGHT, Uint32 = PLUSDL_DEFAULT_FLAGS);
	static App* instance();

	AppInitializer& getInitializer() { return initializer; };
	AppWindow& getWindow() { return window; };
	AppRenderer& getRenderer() { return renderer; };
	AppEventManager& getEventManager() { return event_manager; };
	
	bool isRunning() { return running; };

	void stop() { running = false; };

private:
	bool running = true;

	AppInitializer initializer;
	AppWindow window;
	AppRenderer renderer;
	AppEventManager event_manager;

	static App* app_instance;
	App(int, int, Uint32);
};