#include "archipelago-ui.hpp"




static void expand(Gtk::Widget& widget, bool h, bool v) {
	widget.set_hexpand(h);
	widget.set_vexpand(v);
}

static void margin(Gtk::Widget& widget, int r, int t, int l, int b) {
	widget.set_margin_right(r);
	widget.set_margin_top(t);
	widget.set_margin_left(l);
	widget.set_margin_bottom(b);
}

void ArchipelagoUI::PositionWidgets(void) {

	layout.attach(btn_info, 0, 0);

	layout.attach(filesystem, 1, 0);
	filesystem.pack_start(filepath, Gtk::PACK_SHRINK);
	filesystem.pack_start(btn_open, Gtk::PACK_SHRINK);
	filesystem.pack_start(btn_save, Gtk::PACK_SHRINK);

	layout.attach(spacer, 2, 0);
	expand(spacer, 1, 0);

	layout.attach(btn_view, 3, 0);

	layout.attach(city_rating, 0, 1, 4, 1);
	layout.attach(city_canvas, 0, 2, 4, 1);
	expand(city_canvas, 1, 1);

	layout.attach(editor_frame, 0, 3, 4, 1);
	editor_frame.add(editor);
	editor.pack_start(tmpstuff);
	editor.pack_start(btn_zone,   Gtk::PACK_SHRINK);
	editor.pack_start(btn_link,   Gtk::PACK_SHRINK);
	editor.pack_start(btn_add_menu, Gtk::PACK_SHRINK);
	editor.pack_start(btn_add,    Gtk::PACK_SHRINK);
	editor.pack_start(btn_edit,   Gtk::PACK_SHRINK);
	editor.pack_start(btn_remove, Gtk::PACK_SHRINK);
	editor.pack_start(mouse_pos);


	margin(btn_info, 2, 2, 2, 2);

	margin(filesystem, 0, 2, 0, 2);
	margin(filepath,   3, 0, 0, 0);
	margin(btn_open,   2, 0, 0, 0);
	margin(btn_save,   0, 0, 0, 0);

	margin(btn_view, 2, 2, 2, 2);

	margin(city_rating, 2, 2, 2, 2);
	margin(city_canvas, 2, 0, 2, 0);

	margin(editor_frame, 2, 2, 2, 6);
	margin(tmpstuff,	 5, 0, 5, 6);
	margin(btn_zone, 	 1, 0, 1, 6);
	margin(btn_link, 	 2, 0, 1, 6);
	margin(btn_add_menu, 1, 0, 2, 6);
	margin(btn_add, 	 1, 0, 2, 6);
	margin(btn_edit, 	 1, 0, 1, 6);
	margin(btn_remove, 	 1, 0, 1, 6);
	margin(mouse_pos, 	 5, 0, 5, 6);
}

void ArchipelagoUI::SetupWidgets(void) {

	btn_info.set_image(img_info);

	filepath.set_size_request(240,0);
    filepath.set_placeholder_text("File name");

	btn_open.set_label("Open");
	btn_save.set_label("Save");

	btn_view.set_image(img_view);

	city_rating.set_markup("<u>MTA</u> <u>ENJ</u> <u>CI</u>");
    city_canvas.set_size_request(300, 300);

	editor_frame.set_label(" Archipelago Editor ");
	editor_frame.set_label_align(0.02, 0.5);

	tmpstuff.set_width_chars(24);
	tmpstuff.set_xalign(0.05);

	btn_zone.set_image(img_zone);
	btn_link.set_image(img_link);
	btn_add_menu.set_image(img_add_menu);
	btn_add.set_image(img_add);
	btn_edit.set_image(img_edit);
	btn_remove.set_image(img_remove);

	btn_add_menu.set_popup(add_menu);
	add_menu.append(ResidentialArea);
	add_menu.append(TransportHub);
	add_menu.append(ProductionZone);
	ResidentialArea.set_label("Residential Area");
	TransportHub.set_label("Transport Hub");
	ProductionZone.set_label("Production Zone");
	btn_add_menu.set_direction(Gtk::ARROW_UP);
	add_menu.show_all();

	mouse_pos.set_width_chars(24);
	mouse_pos.set_xalign(0.95);
}

void ArchipelagoUI::ConnectSignals(void) {

    city.add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::SMOOTH_SCROLL_MASK | Gdk::KEY_PRESS_MASK /*| Gdk::KEY_RELEASE_MASK*/);

	btn_info.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::DisplayInfo_cb));

 	signal_key_press_event().connect(sigc::mem_fun(this, &ArchipelagoUI::KeyboardShortcuts_cb));

	btn_open.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::OpenFile_cb));
	btn_save.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::SaveFile_cb));

	btn_view.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::ResetView_cb));

	btn_zone.signal_toggled()    .connect(sigc::mem_fun(*this, &ArchipelagoUI::EditZone_cb));
	btn_link.signal_toggled()    .connect(sigc::mem_fun(*this, &ArchipelagoUI::EditLink_cb));
	btn_add_menu.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::AddMenu_cb));
	btn_add .signal_toggled()    .connect(sigc::mem_fun(*this, &ArchipelagoUI::Add_cb));
	btn_edit.signal_toggled()    .connect(sigc::mem_fun(*this, &ArchipelagoUI::Edit_cb));
	btn_remove.signal_toggled()  .connect(sigc::mem_fun(*this, &ArchipelagoUI::Remove_cb));

    ResidentialArea.signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), ZoneType::RESIDENTIAL));
    TransportHub.signal_activate()	 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), ZoneType::TRANSPORT));
    ProductionZone.signal_activate() .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), ZoneType::PRODUCTION));

	gesture_zoom = Gtk::GestureZoom::create(city_canvas);
	gesture_zoom->signal_scale_changed().connect(sigc::mem_fun(*this, &ArchipelagoUI::Zoom_cb));

	gesture_rotate = Gtk::GestureRotate::create(city_canvas);
	gesture_rotate->signal_angle_changed().connect(sigc::mem_fun(*this, &ArchipelagoUI::Rotate_cb));

	gesture_drag = Gtk::GestureDrag::create(city_canvas);
	gesture_drag->signal_drag_begin() .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::Drag_cb), Archipelago::EditState::INIT));
	gesture_drag->signal_drag_update().connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::Drag_cb), Archipelago::EditState::UPDATE));
	gesture_drag->signal_drag_end()   .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::Drag_cb), Archipelago::EditState::END));
}


ArchipelagoUI::ArchipelagoUI() {

    Gtk::Window::set_title("Archipelago City");
    Gtk::Window::resize(400,400);

	PositionWidgets();
	SetupWidgets();
	ConnectSignals();

	Gtk::Window::add(layout);
	Gtk::Window::show_all();
	btn_add_menu.hide();

	editor_action = Archipelago::EditorOptions::NONE;
	// add_menu_choice = ZoneType::NONE;
}

#define GDK_COMMAND_MASK GDK_MOD2_MASK
bool ArchipelagoUI::KeyboardShortcuts_cb(GdkEventKey* event) {

	if(event->state & GDK_SHIFT_MASK) {
		btn_zone.set_active(1);
		switch(gdk_keyval_to_upper(event->keyval)) {
		  case GDK_KEY_P:
			AddMenuSelect_cb(ZoneType::PRODUCTION); break;
		  case GDK_KEY_R:
			AddMenuSelect_cb(ZoneType::RESIDENTIAL); break;
		  case GDK_KEY_T:
		  	AddMenuSelect_cb(ZoneType::TRANSPORT); break;
		  case GDK_KEY_Return:
		    btn_edit.set_active(1); break;
		  case GDK_KEY_BackSpace:
		    btn_remove.set_active(1); break;
		  default:
		  	btn_zone.set_active(0);
		}
		return 1;
	}
	if(event->state & GDK_CONTROL_MASK) {
		btn_link.set_active(1);
		switch(gdk_keyval_to_upper(event->keyval)) {
		  case GDK_KEY_L:
			btn_add.set_active(1); break;
		  case GDK_KEY_BackSpace:
			btn_remove.set_active(1); break;
		  default:
		  	btn_link.set_active(0);
		}
		return 1;
	}
	if(event->state & GDK_COMMAND_MASK) {
		switch(gdk_keyval_to_upper(event->keyval)) {
		  case GDK_KEY_I:
		  	DisplayInfo_cb();
			break;
		  case GDK_KEY_O:
		  	OpenFile_cb();
			break;
		  case GDK_KEY_S:
		  	SaveFile_cb();
			break;
		  case GDK_KEY_W:
		  case GDK_KEY_Q:
		  	Gtk::Window::hide(); // to close window w\o segfault
			break;
		}
		return 1;
	}
	if(event->keyval == GDK_KEY_Escape) {
		ResetEditor();
	}
	if(event->keyval == GDK_KEY_space) {
		ResetView_cb();
	}
    return 0;
}

void ArchipelagoUI::DisplayInfo_cb(void) {
	std::cout<<"stub?DisplayInfo_cb\n";
}

void ArchipelagoUI::OpenFile_cb(void) {
	if(city.OpenFile(getFilename())) {
	    Gtk::Window::set_title("Archipelago City - " + StripPathFromfilepath(getFilename()));
    } else {
        Gtk::Window::set_title("Archipelago City");
        mouse_pos.set_label("Could not open file "+ getFilename());
    }
}

void ArchipelagoUI::SaveFile_cb(void) {
	city.SaveFile(getFilename());
	mouse_pos.set_text("file saved");
}

void ArchipelagoUI::ResetView_cb(void) {
	city_canvas.ResetViewModifiers();
}

void ArchipelagoUI::EditZone_cb(void) {

	if(btn_zone.get_active()) {
		btn_link.set_active(0);
		if(btn_add.get_active()) {
			btn_add_menu.show();
			btn_add_menu.set_active(1);
			btn_add.hide();
		}
	} else {
		btn_add.set_active(0);
		btn_edit.set_active(0);
		btn_remove.set_active(0);
	}
	UpdateEditorAction();
}

void ArchipelagoUI::EditLink_cb(void) {

	if(btn_link.get_active()) {
		btn_zone.set_active(0);
		btn_edit.set_active(0);
	} else {
		btn_add.set_active(0);
		btn_edit.set_active(0);
		btn_remove.set_active(0);
	}
	UpdateEditorAction();
}

void ArchipelagoUI::AddMenu_cb(void) {
	if(!btn_add_menu.get_active()) {
		btn_add.show();
		btn_add.set_active(0);
		btn_add_menu.hide();
		return ;
	}
}

void ArchipelagoUI::AddMenuSelect_cb(ZoneType choice) {
	add_menu_choice = choice;
	editor_action = Archipelago::EditorOptions::ADD_ZONE;
	btn_add.set_active(1);
}

void ArchipelagoUI::Add_cb(void) {
	if(!btn_add.get_active()) {
		// add_menu_choice = ZoneType::NONE;
		editor_action = Archipelago::EditorOptions::NONE;
		return ;
	}
	btn_edit.set_active(0);
	btn_remove.set_active(0);

	if(btn_zone.get_active() && editor_action == Archipelago::EditorOptions::NONE/*add_menu_choice == ZoneType::NONE*/ ) {
		btn_add_menu.show();
		btn_add_menu.set_active(1);
		btn_add.hide();
	}
	UpdateEditorAction();
}

void ArchipelagoUI::Edit_cb(void) {
	if(!btn_edit.get_active()) {
		return ;
	}
	btn_add.set_active(0);
	btn_remove.set_active(0);

	if(btn_link.get_active()) {
		btn_link.set_active(0);
	} else if(!btn_zone.get_active()) {
		btn_zone.set_active(1);
	}
	UpdateEditorAction();
}

void ArchipelagoUI::Remove_cb(void) {
	if(!btn_remove.get_active()) {
		return ;
	}
	btn_add.set_active(0);
	btn_edit.set_active(0);

	UpdateEditorAction();
}

bool ArchipelagoUI::on_motion_notify_event(GdkEventMotion*event) {
    RefreshMouseCoordinates();
    return 1;
}

bool ArchipelagoUI::on_scroll_event(GdkEventScroll *ev) {
	city_canvas.Translate(ev->delta_x, ev->delta_y);
    RefreshMouseCoordinates();
    return 1;
}

void ArchipelagoUI::Rotate_cb(float now, float angle_delta) {
    static float last{0};
    if(abs(now) > 0.02) {
        city_canvas.Rotate(SIGN(now - last));
        RefreshMouseCoordinates();
    }
    last = now;
}

void ArchipelagoUI::Zoom_cb(float now) {
    static float last{1};
    if(abs(now - 1) > 0.02) {
    	city_canvas.Scale(SIGN(now - last));
        RefreshMouseCoordinates();
    }
	last = now;
}

void ArchipelagoUI::Drag_cb(float dx, float dy, Archipelago::EditState state) {
    if(filepath.has_focus()) btn_open.grab_focus(); // one way to take focus off filepath entry
	std::cout<<int(state)<<'\n';
	int x, y; city_canvas.get_pointer(x, y);
	Coord2D position = city_canvas.Pointer2ArchipelagoXY(x, y);

	if(editor_action != Archipelago::EditorOptions::NONE) {
		switch(editor_action) {
		  case Archipelago::EditorOptions::MODIFY_ZONE:
			city.ModifyZone(position, state);
			break;
		  case Archipelago::EditorOptions::ADD_LINK:
			city.AddLink(position, state);
			break;
		  default: // act as button release and cancel action if release is far from original press
			if(state == Archipelago::EditState::END && abs(dx) < 5 && abs(dy) < 5) {
				switch(editor_action) {
				  case Archipelago::EditorOptions::ADD_ZONE:
					city.AddZone(add_menu_choice, position);
					break;
				  case Archipelago::EditorOptions::REMOVE_ZONE:
					city.RemoveZone(position);
					break;
				  case Archipelago::EditorOptions::REMOVE_LINK:
					city.RemoveLink(position);
					break;
			      default: ;
				}
			}
		}
		city.ComputePerformance();
		city_rating.set_markup(city.performance);
	} else {
		city.ShortestPath(state, position);
	}
}


void ArchipelagoUI::UpdateEditorAction(void) {

	if(btn_zone.get_active()) {
		if(btn_add.get_active()) 	{ editor_action = Archipelago::EditorOptions::ADD_ZONE;		return ; }
		if(btn_edit.get_active()) 	{ editor_action = Archipelago::EditorOptions::MODIFY_ZONE;	return ; }
		if(btn_remove.get_active()) { editor_action = Archipelago::EditorOptions::REMOVE_ZONE;	return ; }
	}
	if(btn_link.get_active()) {
		if(btn_add.get_active()) 	{ editor_action = Archipelago::EditorOptions::ADD_LINK;		return ; }
		if(btn_remove.get_active()) { editor_action = Archipelago::EditorOptions::REMOVE_LINK;	return ; }
	}
	editor_action = Archipelago::EditorOptions::NONE;
}

void ArchipelagoUI::ResetEditor(void) {

	editor_action = Archipelago::EditorOptions::NONE;
	btn_zone.set_active(0);
	btn_link.set_active(0);
	btn_add.set_active(0);
	btn_edit.set_active(0);
	btn_remove.set_active(0);
}



void ArchipelagoUI::RefreshMouseCoordinates() {
    int x, y; city_canvas.get_pointer(x, y);
    Coord2D mouse = city_canvas.Pointer2ArchipelagoXY(double(x), double(y));

	auto dbl2str = [](float a) {
		std::ostringstream out;
		out.precision(2);
		out << (abs(a) < 10 ? "   " : abs(a) < 100 ? "  " : abs(a) < 1000 ? " " : "");
		out << std::fixed << (a < 0 ? "-" : "+") << abs(a);
		return out.str();
	};
	// display coordinates of zone center during edit
    mouse_pos.set_markup("<span font_family=\'Fira Code\' size=\"smaller\"><b>X:</b>"
						+ dbl2str(mouse.x) +" <b>Y:</b>"+ dbl2str(mouse.y) +"</span> ");

	if(!dispinfo)
		tmpstuff.set_markup("<span font_family=\'Fira Code\' size=\"smaller\">" + city.InfoFromCoordinates(mouse) + "</span>");
}
