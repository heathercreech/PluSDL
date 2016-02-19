#pragma once

#include "PluSDL.h"


int main(int argc, char* argv[]) {

	App::init(640, 480);

	while (App::instance()->isRunning()) {
		App::instance()->getEventManager().checkForEvent();
		App::instance()->getRenderer().clear();
		App::instance()->getRenderer().update();
	}

	return 0;
}