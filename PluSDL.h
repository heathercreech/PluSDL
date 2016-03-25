#pragma once

#include <vector>
#include <map>
#include <functional>

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>

#include "refc.hpp"

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
	AppSurface(SDL_Window*);
	AppSurface(std::string); //IMG_Load constructor
	AppSurface(int, int, int = 32, Uint32 = 0, Uint32 = 0, Uint32 = 0, Uint32 = 0);

	operator bool();
	explicit operator SDL_Surface*() { return this->get(); };

	SDL_Surface* get() { return surface; };

private:
	ReferenceCounter<SDL_Surface>::Pointer surface;
};

//Wrapper for SDL_Texture
class AppTexture {
public:
	AppTexture(std::string);
	
	operator bool();
	explicit operator SDL_Texture*() { return this->get(); };

	SDL_Texture* get() { return texture; };

private:
	ReferenceCounter<SDL_Texture>::Pointer texture;
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

	operator bool();
	explicit operator SDL_Window*() { return this->get(); };

	SDL_Window* get() { return window; };
	AppSurface getSurface() { return surface; };

	int getWidth() { return width; };
	int getHeight() { return height; };

	void updateDimensions(int w, int h) { width = w; height = h; };

private:
	int width, height;

	ReferenceCounter<SDL_Window>::Pointer window;
	AppSurface surface;
};

//Wrapper for SDL_Renderer
class AppRenderer {
public:
	AppRenderer(AppWindow&);

	operator bool();
	explicit operator SDL_Renderer*() { return this->get(); };

	ReferenceCounter<SDL_Renderer>::Pointer get() { return renderer; };

	void clear();
	void update();

	void copy(SDL_Texture*, SDL_Rect* = NULL);

private:
	ReferenceCounter<SDL_Renderer>::Pointer renderer;
};

//Handles calling of functions in response to SDLs events
class AppEventManager {
public:
	AppEventManager();

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