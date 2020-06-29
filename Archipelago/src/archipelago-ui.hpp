
#include <glibmm/main.h> //timeout
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>

#include <gtkmm/gestureswipe.h>
#include <gtkmm/gesturerotate.h>
#include <gtkmm/gesturezoom.h>
#include <gtkmm/gesturedrag.h>
// #include <gtkmm/box.h>
#include "archipelago-bg.hpp"

class ArchipelagoUI : public Gtk::Window {

  public: // constructors etc.
	ArchipelagoUI(void);
   ~ArchipelagoUI(void) = default;

  private: // UI settings
  	void setWindowTitle(const std::string& title) { Gtk::Window::set_title(title); }
    void setWindowSize(uint width, uint height) { Gtk::Window::resize(width, height); }

	Gtk::Grid layout;
	// Gtk::Grid ctrl;
	// Gtk::HBox controls;

    // Gtk::Label  file{"File"};
    Gtk::Entry  filename;
    Gtk::Button open{"Open"};
    Gtk::Button save{"Save"};

    // Gtk::Label  status{"Add/Remove"}; // add/rm text
    // Gtk::ComboBoxText elements;
    // Gtk::Button modifier{"+/–"}; // md status

    // Gtk::Label rating{"ENJ: XXX, CI: XXX, MTA: XXX !"};
	Gtk::Image file{Gdk::Pixbuf::create_from_file("./img/docs_icon.png")};
	Gtk::Image magzoom{Gdk::Pixbuf::create_from_file("./img/zoom_icon.png")};
	Gtk::Button view;//{magzoom, "Reset View"};//{"Reset View"};

	std::string getFilename(void) { return filename.get_text(); }

	Gtk::Menu pop_menu;
	Gtk::MenuItem move{"Move"};
	Gtk::MenuItem resize{"Resize"};
	Gtk::MenuItem connect{"Connect"};
	Gtk::MenuItem add{"Add"};
	bool on_timeout(void);

	Gtk::Menu add_menu;
	Gtk::MenuItem r{"residentialarea"};
	Gtk::MenuItem t{"transporthub"};
	Gtk::MenuItem p{"productionzone"};
	Gtk::MenuItem l{"link"};
	Gtk::MenuItem remove{"Remove"};
	void movemenu(void)    { model.movemenu(); }
	void resizemenu(void)  { model.resizemenu(); }
	void connectmenu(void) { model.connectmenu(); }
	void addmenu(char zone);

	void removemenu(void) { model.removemenu();}

	virtual bool on_scroll_event(GdkEventScroll *ev) override;
	virtual bool on_button_press_event(GdkEventButton *ev) override;
	virtual bool on_button_release_event(GdkEventButton *ev) override;


  private: // Archipelago
    Archipelago model;
	Glib::RefPtr<Gtk::GestureSwipe> m_GestureSwipe;
	Glib::RefPtr<Gtk::GestureRotate> m_GestureRotate;
  	Glib::RefPtr<Gtk::GestureZoom>  m_GestureZoom;
  	// Glib::RefPtr<Gtk::GesturePan>  m_GesturePan;
  	Glib::RefPtr<Gtk::GestureDrag>  m_GestureDrag;
	void Zoom_cb(double change);
	void Swipe_cb(double vx, double vy);
	void Rotate_cb(double vx, double vy);
	// void Pan_cb(double vx, double vy);
	void DragStart_cb(double dx, double dy);
	void Drag_cb(double dx, double dy);
	void DragEnd_cb(double dx, double dy);
    bool on_motion_notify_event(GdkEventMotion*event) override;
    bool on_key_release_event(GdkEventKey* event) override;
    void RefreshMouseCoordinates(void);


  private: // UI console
    Gtk::Label messages;
    Gtk::Label spacer;


	void OpenFile_cb(void);
	void SaveFile_cb(void);
	void ResetView_cb(void);
	double mx, my;
	uint bs = 9839;
};

// template<typename T, typename fn, typename...Args>
// void connection(T& thing, fn f, Args&&...args) {
//     thing.signal_activate().connect(sigc::bind(sigc::mem_fun(*this, f), (args, ...)));
// }