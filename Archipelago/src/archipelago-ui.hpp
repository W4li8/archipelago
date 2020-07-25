
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
// #include <gtkmm/messagedialog.h>
// #include <gtkmm/dialog.h>
#include <gtkmm/box.h>
// #include <gtkmm/separator.h>
#include <gtkmm/frame.h>



#include <gtkmm/gesturezoom.h>
#include <gtkmm/gesturerotate.h>
#include <gtkmm/gesturedrag.h>

// #include "archipelago-bg.hpp"
#include "archipelago-bg.hpp"

class ArchipelagoUI : public Gtk::Window {

  public: // constructors etc.
	ArchipelagoUI(void);
   ~ArchipelagoUI(void) = default;

// #################################### WIDGETS ################################### //
  private: // UI Widgets
    // Gtk::Window helps;
    // Gtk::MessageDialog helper{"HELP", 1, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK};

	Gtk::Grid layout;

// ==================================== HEADER ==================================== //
	Gtk::Button btn_info;

	Gtk::HBox   filesystem;
    Gtk::Entry  filepath;
    Gtk::Button btn_open;
    Gtk::Button btn_save;

	Gtk::Button btn_view;

    Gtk::Label spacer; // widget to fill space

	Gtk::Image img_info { Gdk::Pixbuf::create_from_file("./img/ui/info.png") };
	Gtk::Image img_view {Gdk::Pixbuf::create_from_file("./img/ui/view.png")  };

// =============================== ARCHIPELAGO CITY =============================== //
	Gtk::Label   city_rating;
    Archipelago  city_canvas;
    Archipelago& city = city_canvas;

// ==================================== EDITOR ==================================== //
	Gtk::Frame editor_frame;
	Gtk::HBox  editor;
    Gtk::Label tmpstuff;
	Gtk::ToggleButton btn_zone;
	Gtk::ToggleButton btn_link;
	Gtk::MenuButton   btn_add_menu; // mostly hidden button
	Gtk::ToggleButton btn_add;
	Gtk::ToggleButton btn_edit;
	Gtk::ToggleButton btn_remove;
    Gtk::Label mouse_pos;

	Gtk::Menu 		add_menu;
	Gtk::MenuItem 	ResidentialArea;
	Gtk::MenuItem 	TransportHub;
	Gtk::MenuItem 	ProductionZone;

	Gtk::Image img_zone { Gdk::Pixbuf::create_from_file("./img/ui/zone.png") };
	Gtk::Image img_link { Gdk::Pixbuf::create_from_file("./img/ui/link.png") };
	Gtk::Image img_add_menu { Gdk::Pixbuf::create_from_file("./img/ui/add.png") };
	Gtk::Image img_add  { Gdk::Pixbuf::create_from_file("./img/ui/add.png")  };
	Gtk::Image img_edit { Gdk::Pixbuf::create_from_file("./img/ui/edit.png") };
	Gtk::Image img_remove { Gdk::Pixbuf::create_from_file("./img/ui/remove.png") };

// #################################### SIGNALS ################################### //
  private: // Signals
  	bool KeyboardShortcuts_cb(GdkEventKey*);// { return 1; }

// ==================================== HEADER ==================================== //
	void DisplayInfo_cb(void);

	void OpenFile_cb(void);
	void SaveFile_cb(void);

	void ResetView_cb(void);

// =============================== ARCHIPELAGO CITY =============================== //
	virtual bool on_motion_notify_event(GdkEventMotion*event) override;
	virtual bool on_scroll_event(GdkEventScroll *ev) override;

	Glib::RefPtr<Gtk::GestureRotate> gesture_rotate;
	void Rotate_cb(float vx, float vy);

	Glib::RefPtr<Gtk::GestureZoom> gesture_zoom;
	void Zoom_cb(float change);

	// virtual bool on_button_press_event(GdkEventButton *ev) override;

  	Glib::RefPtr<Gtk::GestureDrag> gesture_drag;
	void Drag_cb(float x, float y, Archipelago::EditState state);
	// void DragBegin_cb(float x, float y);
	// void DragUpdate_cb(float dx, float dy);
	// void DragEnd_cb(float dx, float dy);

// ==================================== EDITOR ==================================== //
	void EditZone_cb(void);
	void EditLink_cb(void);
	void Add_cb(void);
	void AddMenu_cb(void);
	void AddMenuSelect_cb(ZoneType choice);
	void Edit_cb(void);
	void Remove_cb(void);

// ##################################### OTHER #################################### //
  private: // Methods
    void RefreshMouseCoordinates(void);
	std::string getFilename(void) { return filepath.get_text(); }

	void PositionWidgets(void);
	void SetupWidgets(void);
	void ConnectSignals(void);

  private:
	enum Archipelago::EditorOptions editor_action;
	void UpdateEditorAction(void);
	void ResetEditor(void);
	bool dispinfo{0};

	ZoneType add_menu_choice;

};
