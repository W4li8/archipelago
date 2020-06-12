#include "archipelago.hpp"
#include <gtkmm.h>

void setMargin(Gtk::Widget& widget, uint top, uint right, uint bottom, uint left) {
	widget.set_margin_top(top);
	widget.set_margin_right(right);
	widget.set_margin_bottom(bottom);
	widget.set_margin_left(left);
}
class ArchipelagoUI : public Gtk::Window {

  public: // constructors etc.
	ArchipelagoUI(void);
   ~ArchipelagoUI(void) = default;

  private: // UI settings
  	void setWindowTitle(const std::string& title) { Gtk::Window::set_title(title); }
    void setWindowSize(uint width, uint height) { Gtk::Window::resize(width, height); }

	Gtk::Grid layout;
	Gtk::HBox controls;

    Gtk::Label  file{"File"};
    Gtk::Entry  filename;
    Gtk::Button open{"Open"};
    Gtk::Button save{"Save"};

	std::string getFilename(void) { return filename.get_text(); }

	// FileIO(Gtk::HBox& layout) {
	// 	layout.pack_start(file);
	// 	layout.pack_start(filename);
	// 	layout.pack_start(open);
	// 	layout.pack_start(save);
	// 	open.set_focus_on_click(1);
	// 	setMargin(file, 0, 10, 0, 10);
	// 	setMargin(filename, 1, 0, 1, 0);
	// 	setMargin(open, 1, 2, 1, 2);
	// 	setMargin(save, 1, 0, 1, 0);
	// }

    Gtk::Label  status{"Add/Remove"}; // add/rm text
    Gtk::ComboBoxText elements;
    Gtk::Button modifier{"+/–"}; // md status

    Gtk::Label rating{"ENJ: XXX, CI: XXX, MTA: XXX !"};

  private: // Archipelago
    Archipelago model;

  private: // UI console
    Gtk::Label  messages;


  void OpenFile_cb(void);
  void SaveFile_cb(void);
};

void ArchipelagoUI::OpenFile_cb(void) {
	model.OpenFile(getFilename());
	setWindowTitle("Archipelago - " + getFilename());
}
void ArchipelagoUI::SaveFile_cb(void) {
	model.SaveFile(getFilename());
}

ArchipelagoUI::ArchipelagoUI() {

    setWindowTitle("Archipelago - Empty");
    setWindowSize(400,400);

	controls.set_vexpand(0);
    // file.set_label("File: ");
    file.set_hexpand(); //set_valign also
    controls.pack_start(file);
    controls.pack_start(filename); filename.set_text("debug/success/S02.txt");
    controls.pack_start(open);
    controls.pack_start(save);
	open.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::OpenFile_cb));
	save.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::SaveFile_cb));

    status.set_label("add/remove");
    controls.pack_start(status);
    // elements.set_model(Gtk::ListStore::create(m_Columns));
    elements.append("A");
    elements.append("B");
    elements.append("C");
    controls.pack_start(elements);
    modifier.set_label("+/–");
    controls.pack_start(modifier);
    // performance.set_label("label");
    controls.pack_start(rating);

    model.set_size_request(400,400);
    model.set_vexpand();
    model.set_hexpand();
    // drawing.pack_start(model);

  	messages.set_vexpand(0);
    messages.set_label("msg");
    messages.set_hexpand();
    // console.pack_start(messages);

	layout.attach(controls, 0, 0);
	layout.attach(model, 0, 1);
	layout.attach(messages, 0, 2);

	Gtk::Window::add(layout);
	Gtk::Window::show_all();
}




auto main(int argc, char *argv[]) -> int {
    auto app = Gtk::Application::create(argc, argv);
    ArchipelagoUI window;
    return app->run(window);
}