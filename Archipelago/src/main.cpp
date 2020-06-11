#include "archipelago.hpp"
#include <gtkmm.h>

  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_id); add(m_col_name); }

    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

  ModelColumns m_Columns;

ModelColumns m_columns;

class ArchipelagoUI : public Gtk::Window {

  public: // constructors etc.
	ArchipelagoUI(void);
   ~ArchipelagoUI(void) = default;

  private: // UI settings
  	void setWindowTitle(const std::string& title) { Gtk::Window::set_title(title); }
    void setWindowSize(uint width, uint height) { Gtk::Window::resize(width, height); }

	Gtk::Grid layout;
	Gtk::HBox controls, drawing, console;

  struct FileIO { // UI controls
    Gtk::Label  file{"File:"};
    Gtk::Entry  filename;
    Gtk::Button open{"Open"};
    Gtk::Button save{"Save"};
  } file_io;

  struct Editor {
    Gtk::Label    status; // add/rm text
    Gtk::ComboBox elements;
    Gtk::Button   modifier; // md status
  } editor;

  struct Rating {
    Gtk::Label  performance;
  } rating;

  private: // Archipelago
    Archipelago model;

  private: // UI console
    Gtk::Label  messages;
};

ArchipelagoUI::ArchipelagoUI() {

    setWindowTitle("Archipelago - Empty");
    setWindowSize(400,400);

    file_io.file.set_margin_left(10);
	controls.set_vexpand(0);
    // file.set_label("File: ");
    file_io.file.set_hexpand(); //set_valign also
    controls.pack_start(file_io.file);
    controls.pack_start(file_io.filename);
    // open.set_label("Open");
    controls.pack_start(file_io.open);
    // save.set_label("Save");
    controls.pack_start(file_io.save);
    editor.status.set_label("add/remove");
    controls.pack_start(editor.status);
    editor.elements.set_model(Gtk::ListStore::create(m_Columns));
    controls.pack_start(editor.elements);
    editor.modifier.set_label("+/–");
    controls.pack_start(editor.modifier);
    // performance.set_label("label");
    controls.pack_start(rating.performance);

    model.set_size_request(400,400);
    model.set_vexpand();
    model.set_hexpand();
    drawing.pack_start(model);

  	console.set_vexpand(0);
    messages.set_label("msg");
    messages.set_hexpand();
    console.pack_start(messages);

	layout.attach(controls, 0, 0);
    layout.attach(drawing, 0, 1);
	layout.attach(console, 0, 2);

	Gtk::Window::add(layout);
	Gtk::Window::show_all();
}




auto main(int argc, char *argv[]) -> int {
    auto app = Gtk::Application::create(argc, argv);
    ArchipelagoUI window;
    return app->run(window);
}