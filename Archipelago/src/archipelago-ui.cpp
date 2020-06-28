#include "archipelago-ui.hpp"

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

void ArchipelagoUI::addmenu(char zone) {
	std::cout<<"HERE\n";
	switch(zone) {
		case 'r': model.CreateZone({bs++, ZoneType::ResidentialArea, model.MouseXY_to_ArchipelagoXY({mx, my}),1000});
		break;
		case 't': model.CreateZone({bs++, ZoneType::TransportHub, model.MouseXY_to_ArchipelagoXY({mx, my}),100});
		break;
		case 'p': model.CreateZone({bs++, ZoneType::ProductionZone, model.MouseXY_to_ArchipelagoXY({mx, my}), 100});
		break;
	}
	model.resizemenu();
}


void expand(Gtk::Widget& widget, bool h, bool v) {
	widget.set_hexpand(h);
	widget.set_vexpand(v);
}
void align(Gtk::Widget& widget, Gtk::Align h, Gtk::Align v){
	widget.set_halign(h);
	widget.set_valign(v);
}
void margin(Gtk::Widget& widget, int r, int t, int l, int b) {
	widget.set_margin_right(r);
	widget.set_margin_top(t);
	widget.set_margin_left(l);
	widget.set_margin_bottom(b);
}



// template<typename...Args>
// void pack_start(Gtk::Box& box, Args&&...args) {
//    (box.pack_start(args), ...);
// }
template<typename...Args>
void append(Gtk::Menu& menu, Args&&...args) {
   (menu.append(args), ...);
	menu.show_all();
}
bool ArchipelagoUI::on_timeout(void) {
	static int i{10};
	if(i) {
		std::cout << i-- <<"s\n";
		return 1;
	} else {
		std::cout <<"BOOM\n";
		Gtk::Window::close();
		return 0;
	}

}

ArchipelagoUI::ArchipelagoUI() {


    setWindowTitle("Archipelago");
    setWindowSize(400,400);
		// open.set_image_from_icon_name("document-open", Gtk::ICON_SIZE_BUTTON);
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &ArchipelagoUI::on_timeout), 2000 );


	// expand(controls, 1, 0);
	// expand(ctrl, 1, 0);
	// pack_start(controls, file, filename, open, save, view);
	// align(file, Gtk::Align::ALIGN_CENTER, Gtk::Align::ALIGN_CENTER);
	// align(filename, Gtk::Align::ALIGN_FILL, Gtk::Align::ALIGN_CENTER);
	// align(open, Gtk::Align::ALIGN_FILL, Gtk::Align::ALIGN_CENTER);
	// align(save, Gtk::Align::ALIGN_FILL, Gtk::Align::ALIGN_CENTER);
	// align(view, Gtk::Align::ALIGN_CENTER, Gtk::Align::ALIGN_CENTER);
	expand(file, 0, 0);
	expand(filename, 0, 0);
	filename.set_size_request(250,0);
	margin(file, 10, 2, 10, 2);
	margin(filename, 2, 1, 0, 1);
	margin(open, 0, 2, 1, 2);
	margin(save, 1, 2, 1, 2);
	margin(view, 2, 2, 1, 2);
	expand(open, 0, 0);
	expand(save, 0, 0);
	expand(spacer, 1, 0);
	expand(view, 0, 0);
	// margin(controls, 2, 2, 2 , 2);
	layout.set_row_homogeneous(0);
	layout.set_column_homogeneous(0);
	open.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::OpenFile_cb));
	save.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::SaveFile_cb));
/*
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
	*/

	view.set_image(magzoom);
	view.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::ResetView_cb));

    model.set_size_request(200,200);
	expand(model, 1, 1);
	margin(model, 2, 0, 2, 0);


	expand(messages, 1, 0);
    messages.set_label("msg");
	margin(messages, 2, 2, 2, 2);
	// file.set_markup("<b>File</b>");
	// open.add_pixlabel("info.xpm", "cool button");
	// magzoom.set(Gdk::Pixbuf::create_from_file("/Users/fs/Developer/Projects/EPFL MT BA1 2020 Archipelago/Archipelago/img/zoom_icon.png"));
	// view.set_icon_widget(magzoom, "ResetView");
	// view.set_relief(RELIEF_NORMAL);
	// view.set_always_show_image();
	// open.set_icon_name("document-open");
	// open.set_use_underline(1);
	// layout.attach(controls, 0, 0);
	// layout.attach(ctrl, 0, 0);
	layout.attach(file, 0, 0);
	layout.attach(filename, 1, 0);
	layout.attach(open, 2, 0);
	layout.attach(save, 3, 0);
	layout.attach(spacer, 4, 0);
	layout.attach(view, 5, 0);
	layout.attach(model, 0, 1, 6);
	layout.attach(messages, 0, 2, 6);

	Gtk::Window::add(layout);
	Gtk::Window::show_all();

	m_GestureZoom = Gtk::GestureZoom::create(model);
	m_GestureDrag = Gtk::GestureDrag::create(model);

	m_GestureZoom->signal_scale_changed().connect(sigc::mem_fun(*this, &ArchipelagoUI::Zoom_cb));
	m_GestureDrag->signal_drag_update()  .connect(sigc::mem_fun(*this, &ArchipelagoUI::Drag_cb));
	m_GestureDrag->signal_drag_begin()   .connect(sigc::mem_fun(*this, &ArchipelagoUI::DragStart_cb));
	m_GestureDrag->signal_drag_end()     .connect(sigc::mem_fun(*this, &ArchipelagoUI::DragEnd_cb));

	append(pop_menu, move, resize, connect, add, remove);
	add.set_submenu(add_menu);
	append(add_menu, r, t, p);

    move.signal_activate()   .connect(sigc::mem_fun(*this, &ArchipelagoUI::movemenu));
    resize.signal_activate() .connect(sigc::mem_fun(*this, &ArchipelagoUI::resizemenu));
    connect.signal_activate().connect(sigc::mem_fun(*this, &ArchipelagoUI::connectmenu));
    r.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::addmenu), 'r'));
    t.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::addmenu), 't'));
    p.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::addmenu), 'p'));
    remove.signal_activate() .connect(sigc::mem_fun(*this, &ArchipelagoUI::removemenu));
}



bool ArchipelagoUI::on_button_press_event(GdkEventButton *ev) {
	mx = ev->x; my = ev->y;
    model.on_button_press_event2(ev->button, model.MouseXY_to_ArchipelagoXY({mx, my}));
	return 1;
}

bool ArchipelagoUI::on_button_release_event(GdkEventButton *ev) {
	mx = ev->x; my = ev->y;
    model.on_button_release_event2(ev->button, model.MouseXY_to_ArchipelagoXY({mx, my}));
	if(ev->button == 3) {
		pop_menu.popup(ev->button, ev->time);
	}
	return 1;
}

bool ArchipelagoUI::on_scroll_event(GdkEventScroll *ev) {
	model.Swipe(ev->delta_x, -ev->delta_y);
    return 1;
}

#define SIGN(x) (((x) < 0) ? -1 : 1)

void ArchipelagoUI::Zoom_cb(double now) {
    static double last = 1;
    if(0.91 < now && now < 1.10) { // ignore first 10%
        last = now;
        return ;
    }
	model.Zoom(SIGN(now - last));
	last = now;
}

void ArchipelagoUI::DragStart_cb(double x, double y) {
	model.DragStart(model.MouseXY_to_ArchipelagoXY({x, y}));
}
void ArchipelagoUI::DragEnd_cb(double dx, double dy) {
	double x, y; m_GestureDrag->get_start_point(x, y);
	model.DragEnd(model.MouseXY_to_ArchipelagoXY({x+dx, y+dy}));
}
void ArchipelagoUI::Drag_cb(double dx, double dy) {
	double x, y; m_GestureDrag->get_start_point(x, y);
	model.DragUpdate(model.MouseXY_to_ArchipelagoXY({x+dx, y+dy})); //add signal drag begin to reset
}

void ArchipelagoUI::Swipe_cb(double vx, double vy) {
	model.Swipe(0.2*vx, 0.2*vy);
	std::cout<<"here\n";
}