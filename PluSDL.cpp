#include "PluSDL.h"


AppRect::AppRect(int w, int h, int x, int y) {
	rect.w = w;
	rect.h = h;
	rect.x = x;
	rect.y = y;
}

void AppRect::resize(int w, int h) {
	rect.w = w;
	rect.h = h;
}

void AppRect::updatePosition(int x, int y) {
	rect.x = x;
	rect.y = y;
}


AppSurface::AppSurface() {
	surface = NULL;
}

AppSurface::AppSurface(int width, int height, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask) {
	createSurface(width, height, depth, rmask, gmask, bmask, amask);
}

AppSurface::~AppSurface() {
	SDL_FreeSurface(surface);
}

AppSurface::operator bool() const {
	if (surface != NULL) {
		return true;
	}
	return false;
}

void AppSurface::set(SDL_Surface* s) {
	if (s == NULL) {
		printf("Attempted to assign NULL surface to AppSurface\n");
	}
	else {
		surface = s;
	}
}

void AppSurface::createSurface(int width, int height, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask) {
	surface = SDL_CreateRGBSurface(0, width, height, depth, rmask, gmask, bmask, amask);
	if (surface == NULL) {
		printf("SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
	}
}


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


AppWindow::AppWindow(int width, int height, Uint32 flags) : surface(){
	createWindow(width, height, flags);
	surface.set(SDL_GetWindowSurface(this->get()));
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

void AppWindow::createWindow(int width, int height, Uint32 flags) {
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
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

App::App(int width, int height, Uint32 flags) : initializer(), window(width, height, flags), renderer(window), event_manager(){
	event_manager.registerSimpleFunction(SDL_QUIT, []() {App::instance()->stop();});
}

void App::init(int width, int height, Uint32 flags) {
	if (!app_instance) {
		app_instance = new App(width, height, flags);
	}
}

App* App::instance(){
	if (app_instance) {
		return app_instance;
	}
	else {
		printf("App not initialized, call App::init(int, int, Uint32)\n");
		return NULL;
	}
}