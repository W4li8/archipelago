#include <gtkmm/application.h>

#include "archipelago-ui.hpp"

auto main(int argc, char *argv[]) -> int {
    auto app = Gtk::Application::create(argc, argv);
    ArchipelagoUI window;
	return app->run(window);
}

