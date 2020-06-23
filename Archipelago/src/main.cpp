#include "archipelago.hpp"
#include <gtkmm.h>

void setMargin(Gtk::Widget& widget, uint right, uint top, uint left, uint bottom) {
	widget.set_margin_right(right);
	widget.set_margin_top(top);
	widget.set_margin_left(left);
	widget.set_margin_bottom(bottom);
}
std::string StripPathFromFileName(std::string filename) {
	for(auto it = find(filename.begin(), filename.end(), '/');
		it < filename.end(); it = find(filename.begin(), filename.end(), '/')) {
		filename.erase(filename.begin(), it+1); // strip directory
	}
    filename.erase(find(filename.begin(), filename.end(), '.'), filename.end()); // strip extension
	return filename;
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

    Gtk::Label  status{"Add/Remove"}; // add/rm text
    Gtk::ComboBoxText elements;
    Gtk::Button modifier{"+/–"}; // md status

    Gtk::Label rating{"ENJ: XXX, CI: XXX, MTA: XXX !"};
	Gtk::Button view{"Reset View"};


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
	// virtual bool on_scroll_event(GdkEventScroll *ev) override;
	// virtual bool on_button_press_event(GdkEventButton *ev) override;
	// virtual bool on_button_release_event(GdkEventButton *ev) override;


  private: // Archipelago
    Archipelago model;
	Glib::RefPtr<Gtk::GestureSwipe> m_GestureSwipe;
  	Glib::RefPtr<Gtk::GestureZoom> m_GestureZoom;

  private: // UI console
    Gtk::Label  messages;


	void OpenFile_cb(void);
	void SaveFile_cb(void);
	void ResetView_cb(void);
};

void ArchipelagoUI::OpenFile_cb(void) {
	model.OpenFile(getFilename());
	setWindowTitle("Archipelago - " + StripPathFromFileName(getFilename()));
}
void ArchipelagoUI::SaveFile_cb(void) {
	model.SaveFile(getFilename());
	messages.set_text("file saved");
}
void ArchipelagoUI::ResetView_cb(void) {
	model.ResetView();
}

ArchipelagoUI::ArchipelagoUI() {

    setWindowTitle("Archipelago");
    setWindowSize(400,400);

	controls.set_vexpand(0);
	controls.set_margin_bottom(10);
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
    // controls.pack_start(rating);
    controls.pack_start(view);
	view.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::ResetView_cb));

    model.set_size_request(200,200);
    model.set_vexpand();
    model.set_hexpand();
    // drawing.pack_start(model);

  	messages.set_vexpand(0);
    messages.set_label("msg");
    messages.set_hexpand();
	messages.set_margin_top(10);
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

/*
bool ArchipelagoUI::on_button_press_event(GdkEventButton *ev) {
    Gtk::Allocation allocation = get_allocation();
    double width = allocation.get_width();
    double height = allocation.get_height();
    scale = CLAMP(zoom*MIN(width, height)/1000*2, 0.3, 3);
    Coord2D center{width/2 + xoffset, height/2 + yoffset};
    if (ev->button == 1) {
        Coord2D pos{ev->x-center.x, -ev->y+center.y}; pos = pos*(1/scale);
        static uint i=6531;
        CreateZone({i++,TransportHub, pos,200});
        queue_draw();
        // pos = pos*scale + center;
		// Memorize pointer position
		// lastXMouse=ev->x;
		// lastYMouse=ev->y;
		// Start moving the view
		// moveFlag=true;
        std::cout << pos.x<<' '<<pos.y<<'\n';
		// Event has been handled
		return true;
	}
    if(ev->button == 3) {
        Coord2D pos{ev->x-center.x, -ev->y+center.y}; pos = pos*(1/scale);
        for(auto& [key, zone] : zones) {
            if(DistancePoint2Point(pos, zone.getCenter()) < zone.getRadius()) {
                newlink.first = zone.id;
                break;
            }
        }
        return true;
    }

    return false;
}

bool ArchipelagoUI::on_button_release_event(GdkEventButton *ev) {
        Gtk::Allocation allocation = get_allocation();
    double width = allocation.get_width();
    double height = allocation.get_height();
    scale = CLAMP(zoom*MIN(width, height)/1000*2, 0.3, 3);
    Coord2D center{width/2 + xoffset, height/2 + yoffset};
    if(ev->button == 3) {
        Coord2D pos{ev->x-center.x, -ev->y+center.y}; pos = pos*(1/scale);
        for(auto& [key, zone] : zones) {
            if(DistancePoint2Point(pos, zone.getCenter()) < zone.getRadius()) {
                newlink.second = zone.id;
                ConnectZones(zones.at(newlink.first), zones.at(newlink.second));
                queue_draw();
                break;
            }
        }
        return true;
    }
}

bool ArchipelagoUI::on_scroll_event(GdkEventScroll *ev)
{
    // Update scale according to mouse scroll
    yoffset -= ev->delta_y/5.;
	// if (yoffset<0.1) zoom=0.1;
    xoffset -= ev->delta_x/5.;
	// if (yoffset<0.1) zoom=0.1;
    // std::cout << zoom << std::endl;
    // std::cout.flush();
    // Update drawing
    queue_draw();
    // There is probably a good reason to do this
    return true;
}
*/