#include "PluSDL.h"


AppInitializer::AppInitializer() {
	initializeVideo();
}

AppInitializer::AppInitializer(std::initializer_list<int> subsystem_flags) {
	for (auto it = subsystem_flags.begin(); it != subsystem_flags.end(); it++) {
		initializeSystem(*it);
	}
}

AppInitializer::~AppInitializer() {
	quitSystems();
}

void AppInitializer::initializeSystem(int flag) {
	if (SDL_InitSubSystem(flag) != 0) {
		printf("SDL_InitSubSystem error: %s\n", SDL_GetError());
		good = false;
	}
	else {
		initialized.push_back(flag);
	}
}

void AppInitializer::quitSystems() {
	for (auto it = initialized.begin(); it != initialized.end(); it++) {
		SDL_QuitSubSystem(*it);
	}
}

AppInitializer::operator bool() const {
	return good;
}



AppWindow::AppWindow() {
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
}

AppWindow::AppWindow(int width, int height) {
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
}

AppWindow::~AppWindow() {
	SDL_DestroyWindow(window);
}

AppWindow::operator bool() const {
	if (window != NULL) {
		return true;
	}
	return false;
}



AppRenderer::AppRenderer(AppWindow &window) {
	renderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);
}

AppRenderer::~AppRenderer() {
	SDL_DestroyRenderer(renderer);
}

AppRenderer::operator bool() const {
	if (renderer != NULL) {
		return true;
	}
	return false;
}

void AppRenderer::clear() {
	SDL_RenderClear(get());
}

void AppRenderer::update() {
	SDL_RenderPresent(get());
}



AppEventManager::AppEventManager() {
	default_function = [](SDL_Event) {};
}

AppEventManager::~AppEventManager() {
}

bool AppEventManager::isHandled(EventType etype) {
	if (event_functions.find(etype) != event_functions.end() ||
			simple_functions.find(etype) != simple_functions.end()) {
		return true;
	}
	return false;
}

void AppEventManager::registerEventFunction(EventType etype, EventFunction efunc) {
	if (!isHandled(etype)) {
		event_functions[etype] = efunc;
	}
}

void AppEventManager::registerSimpleFunction(EventType etype, SimpleFunction sfunc) {
	if (!isHandled(etype)) {
		simple_functions[etype] = sfunc;
	}
}

void AppEventManager::callFunction(SDL_Event e) {
	if (isHandled(e.type)) {
		std::map<EventType, EventFunction>::iterator event_it = event_functions.find(e.type);
		if (event_it != event_functions.end()) {
			event_it->second(e);
		}
		else {
			simple_functions[e.type]();
		}
	}
	else {
		default_function(e);
	}
}

void AppEventManager::checkForEvent() {
	SDL_Event e;
	if (SDL_PollEvent(&e)) {
		callFunction(e);
	}
}



App *App::app_instance = 0;

App::App(int width, int height) : initializer(), window(width, height), renderer(window), event_manager() {
	event_manager.registerSimpleFunction(SDL_QUIT, []() {App::instance()->stop();});
}

void App::init(int width, int height) {
	if (!app_instance) {
		app_instance = new App(width, height);
	}
}

App* App::instance(){
	if (app_instance) {
		return app_instance;
	}
	else {
		printf("App not initialized, call App::init(int, int)\n");
		return NULL;
	}
}