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


AppSurface::AppSurface(int width, int height, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask) : 
	surface(ReferenceCounter<SDL_Surface>::create(
		SDL_CreateRGBSurface(0, width, height, depth, rmask, gmask, bmask, amask),
		SDL_FreeSurface))
{}

AppSurface::AppSurface(SDL_Window* w) : 
	surface(ReferenceCounter<SDL_Surface>::create(SDL_GetWindowSurface(w), SDL_FreeSurface))
{}

AppSurface::AppSurface(std::string filepath) :
	surface(ReferenceCounter<SDL_Surface>::create(IMG_Load(filepath.c_str()), SDL_FreeSurface))
{}

AppSurface::operator bool(){
	if (surface.good()) {
		return true;
	}
	return false;
}


AppTexture::AppTexture(std::string filepath) : 
	texture(ReferenceCounter<SDL_Texture>::create(
		SDL_CreateTextureFromSurface(static_cast<SDL_Renderer*>(App::instance()->getRenderer()), static_cast<SDL_Surface*>(AppSurface(filepath))), 
		SDL_DestroyTexture))
{}

AppTexture::operator bool(){
	if (texture.good()) {
		return true;
	}
	return false;
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


AppWindow::AppWindow(int w, int h, Uint32 flags) : 
	window(ReferenceCounter<SDL_Window>::create(
		SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags),
		SDL_DestroyWindow
		)),
	surface(window)
{
	width = w;
	height = h;
}

AppWindow::operator bool() {
	if (window.good()) {
		return true;
	}
	return false;
}


AppRenderer::AppRenderer(AppWindow &window) : 
	renderer(ReferenceCounter<SDL_Renderer>::create(
		SDL_CreateRenderer(static_cast<SDL_Window*>(window), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer)
		)
{}

AppRenderer::operator bool() {
	if (renderer.good()) {
		return true;
	}
	return false;
}

void AppRenderer::clear() {
	SDL_RenderClear(this->get());
}

void AppRenderer::update() {
	SDL_RenderPresent(this->get());
}

void AppRenderer::copy(SDL_Texture* texture, SDL_Rect* rect) {
	SDL_RenderCopy(this->get(), texture, NULL, rect);
}


AppEventManager::AppEventManager() {
	default_function = [](SDL_Event) {};
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
	event_manager.registerEventFunction(SDL_WINDOWEVENT, [](SDL_Event e) {if (e.window.event == SDL_WINDOWEVENT_RESIZED) { App::instance()->getWindow().updateDimensions(e.window.data1, e.window.data2); }});
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