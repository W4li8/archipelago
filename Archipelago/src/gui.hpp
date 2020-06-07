#include "myarea.h"
#ifndef COORD
#define COORD
struct Coord {
	double x, y;

	Coord operator+(const Coord& other) {
		return {x + other.x, y + other.y};
    }

	Coord operator*(double nb) {
        return {x*nb, y*nb};
    }
};
#endif
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>


/*
int main(int argc, char** argv)
{
	 auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

	 Gtk::Window win;
	 win.set_title("DrawingArea");

	 MyArea area;
	 win.add(area);
	 area.show();

	 return app->run(win);
}
*/
#include <string>
class UserInterface : public Gtk::Application {
 public: // constructors etc.
	UserInterface(const std::string& title);	// UserInterface(int argc, char *argv[]);

  public:
	void RenameWindow(const std::string& title) { window.set_title(title); }

	void DrawLine(Coord A, Coord B);
	void DrawCircle(Coord center, double radius);
	void DrawRectangle(Coord BL, Coord TR);


  private:
	Gtk::Window window;
	const Cairo::RefPtr<Cairo::Context> pencil;
	// Gtk::DrawingArea drawing;
};

UserInterface::UserInterface(const std::string& title)
{

	window.set_title(title);
	window.set_default_size(600, 600);


	Gtk::VBox layout;
	window.add(layout);

	Gtk::HBox controls;
	layout.pack_start(controls, Gtk::PACK_EXPAND_WIDGET);

	Gtk::DrawingArea drawing;
	// pencil = drawing.get_window()->create_cairo_context();
	layout.pack_start(drawing, Gtk::PACK_EXPAND_PADDING); // or packexpand_padding

	Gtk::HBox console;
	layout.pack_start(console);

	Gtk::Button btn{"QUIT"};
	console.pack_start(btn);

	window.show_all();
	Gtk::Application::run(window);
}



// UserInterface::UserInterface(int argc, char *argv[]) {

// 	Gtk::Application::create(argc, argv);
// 	// Gtk::Window window;

// 	window.set_title("DrawingArea");

// 	MyArea area;
// 	window.add(area);

// 	area.show();

// 	Gtk::Application::run(window);
// }

void UserInterface::DrawLine(Coord A, Coord B) {
	pencil->move_to(A.x, A.y);
	pencil->line_to(B.x, B.y);
	pencil->stroke();
}
void UserInterface::DrawCircle(Coord center, double radius) {

}
void UserInterface::DrawRectangle(Coord BL, Coord TR) {

}