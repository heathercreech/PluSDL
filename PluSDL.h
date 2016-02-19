#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <SDL2\SDL.h>


typedef std::function<void()> SimpleFunction;
typedef std::function<void(SDL_Event)> EventFunction;
typedef Uint32 EventType;

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
	AppWindow();
	AppWindow(int, int);

	~AppWindow();

	operator bool() const;

	SDL_Window* get() { return window; };

private:
	SDL_Window* window;
};

//Wrapper for SDL_Renderer
class AppRenderer {
public:
	AppRenderer(AppWindow&);

	~AppRenderer();

	operator bool() const;

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
	static void init(int, int);
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
	App(int, int);
};