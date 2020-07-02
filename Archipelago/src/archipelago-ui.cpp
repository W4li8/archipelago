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

bool ArchipelagoUI::on_key_press_event(GdkEventKey* event) {
    if(!filepath.has_focus())
    switch(gdk_keyval_to_upper(event->keyval)) {
      case GDK_KEY_P: if(event->state & GDK_SHIFT_MASK) AddMenuSelect_cb('P');
        break;
      case GDK_KEY_R: if(event->state & GDK_SHIFT_MASK) AddMenuSelect_cb('R');
        break;
      case GDK_KEY_T: if(event->state & GDK_SHIFT_MASK) AddMenuSelect_cb('T');
        break;
      case GDK_KEY_C:
        break;
      case GDK_KEY_O: if(event->state & GDK_CONTROL_MASK) OpenFile_cb();
        break;
      case GDK_KEY_S: if(event->state & GDK_CONTROL_MASK) SaveFile_cb();
        break;
      case GDK_KEY_W:
      case GDK_KEY_Q: if(event->state & GDK_MOD2_MASK) hide();
        break;
      case GDK_KEY_BackSpace: //removemenu();
        break;
      case GDK_KEY_space: ResetView_cb();
        break;
      case GDK_KEY_K: if(event->state & GDK_MOD2_MASK) std::cout<<"COMMAND\n";
        break;
      default:
        messages.set_text("this key does nothing");

    } else if(event->keyval == GDK_KEY_Escape) btn_open.grab_focus(); // only way to take focus off filepath

    return 1;
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
	editor.pack_start(messages);


	margin(btn_info, 2, 2, 2, 2);

	margin(filesystem, 0, 2, 0, 2);
	margin(filepath,   3, 0, 0, 0);
	margin(btn_open,   2, 0, 0, 0);
	margin(btn_save,   0, 0, 0, 0);

	margin(btn_view, 2, 2, 2, 2);

	margin(city_canvas, 2, 0, 2, 0);

	margin(editor_frame, 2, 2, 2, 6);
	margin(tmpstuff,	 2, 0, 5, 6);
	margin(btn_zone, 	 1, 0, 1, 6);
	margin(btn_link, 	 2, 0, 1, 6);
	margin(btn_add, 	 1, 0, 2, 6);
	margin(btn_edit, 	 1, 0, 1, 6);
	margin(btn_remove, 	 1, 0, 1, 6);
	margin(messages, 	 5, 0, 2, 6);
}

void ArchipelagoUI::SetupWidgets(void) {

	btn_info.set_image(img_info);

	filepath.set_size_request(240,0);
    filepath.set_placeholder_text("File name");

	btn_open.set_label("Open");
	btn_save.set_label("Save");

	btn_view.set_image(img_view);

    city_canvas.set_size_request(300, 300);

	editor_frame.set_label(" Archipelago Editor ");
	editor_frame.set_label_align(0.02, 0.5);

	tmpstuff.set_width_chars(21);
	tmpstuff.set_xalign(0.05);

	btn_zone.set_image(img_zone);
	btn_link.set_image(img_link);
	btn_add_menu.set_menu(add_menu);
	add_menu.append(r);
	add_menu.append(t);
	add_menu.append(p);
	add_menu.show_all();
	btn_add_menu.set_direction(Gtk::ARROW_UP);
	btn_add.set_image(img_add);
	btn_edit.set_image(img_edit);
	btn_remove.set_image(img_remove);

	messages.set_width_chars(21);
	messages.set_xalign(0.95);
}

void ArchipelagoUI::ConnectSignals(void) {

    city.add_events(Gdk::POINTER_MOTION_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::SMOOTH_SCROLL_MASK);
	btn_info.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::DisplayInfo_cb));

	btn_open.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::OpenFile_cb));
	btn_save.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::SaveFile_cb));

	btn_view.signal_clicked().connect(sigc::mem_fun(*this, &ArchipelagoUI::ResetView_cb));

	btn_zone.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::EditZone_cb));
	btn_link.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::EditLink_cb));
	btn_add_menu.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::AddMenu_cb));
    r.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), 'R'));
    t.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), 'T'));
    p.signal_activate()		 .connect(sigc::bind(sigc::mem_fun(*this, &ArchipelagoUI::AddMenuSelect_cb), 'P'));
	btn_add .signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::Add_cb));
	btn_edit.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::Edit_cb));
	btn_remove.signal_toggled().connect(sigc::mem_fun(*this, &ArchipelagoUI::Remove_cb));


	gesture_zoom = Gtk::GestureZoom::create(city_canvas);
	gesture_zoom->signal_scale_changed().connect(sigc::mem_fun(*this, &ArchipelagoUI::Zoom_cb));

	gesture_rotate = Gtk::GestureRotate::create(city_canvas);
	gesture_rotate->signal_angle_changed().connect(sigc::mem_fun(*this, &ArchipelagoUI::Rotate_cb));

	gesture_drag = Gtk::GestureDrag::create(city_canvas);
	gesture_drag->signal_drag_begin()   .connect(sigc::mem_fun(*this, &ArchipelagoUI::DragBegin_cb));
	gesture_drag->signal_drag_update()  .connect(sigc::mem_fun(*this, &ArchipelagoUI::DragUpdate_cb));
	gesture_drag->signal_drag_end()     .connect(sigc::mem_fun(*this, &ArchipelagoUI::DragEnd_cb));
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
}



void ArchipelagoUI::DisplayInfo_cb(void) {
	//TODO
}

void ArchipelagoUI::OpenFile_cb(void) {
	if(city.OpenFile(getFilename())) {
	    Gtk::Window::set_title("Archipelago City - " + StripPathFromfilepath(getFilename()));
    } else {
        Gtk::Window::set_title("Archipelago City");
        messages.set_label("Could not open file "+ getFilename());
    }
}

void ArchipelagoUI::SaveFile_cb(void) {
	city.SaveFile(getFilename());
	messages.set_text("file saved");
}

void ArchipelagoUI::ResetView_cb(void) {
	city_canvas.ResetView();
}

void ArchipelagoUI::EditZone_cb(void) {

	if(btn_zone.get_active()) {
		btn_link.set_active(0);
		btn_add.get_active() ? btn_add_menu.set_active(1) : void();
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
	btn_add_menu.get_active() ? void() : btn_add.set_active(0);
}

void ArchipelagoUI::AddMenuSelect_cb(char zonetype) {
	select = zonetype;
	btn_add.set_active(1);
}

void ArchipelagoUI::Add_cb(void) {
	if(!btn_add.get_active()) { select = 0; return ; }

	btn_edit.set_active(0);
	btn_remove.set_active(0);

	if(!select && btn_zone.get_active()) {
		btn_add_menu.set_active(1);
	}
	UpdateEditorAction();
}

void ArchipelagoUI::Edit_cb(void) {
	if(!btn_edit.get_active()) return ;

	btn_add.set_active(0);
	btn_remove.set_active(0);

	if(btn_link.get_active()) {
		btn_link.set_active(0);
	} else {
		btn_zone.get_active() ? void() : btn_zone.set_active(1);
	}
	UpdateEditorAction();
}

void ArchipelagoUI::Remove_cb(void) {
	if(!btn_remove.get_active()) return ;

	btn_add.set_active(0);
	btn_edit.set_active(0);

	UpdateEditorAction();
}

bool ArchipelagoUI::on_motion_notify_event(GdkEventMotion*event) {
    RefreshMouseCoordinates();
    return 1;
}

bool ArchipelagoUI::on_scroll_event(GdkEventScroll *ev) {
	city_canvas.Swipe(ev->delta_x, -ev->delta_y);
    RefreshMouseCoordinates();
    return 1;
}

void ArchipelagoUI::Rotate_cb(double now, double angle_delta) {
    static double last = 0;
    if(abs(now) > 0.01) {
        city_canvas.Rotate(SIGN(now - last));
        RefreshMouseCoordinates();
    }
    last = now;
}

void ArchipelagoUI::Zoom_cb(double now) {
    static double last = 1;
    city_canvas.tmpzoom = gesture_zoom->get_scale_delta();

    if(abs(now - 1) > 0.01) {
    	city_canvas.Zoom(SIGN(now - last));
        RefreshMouseCoordinates();
    }
	last = now;
}



bool ArchipelagoUI::on_button_press_event(GdkEventButton *ev) {
    if(filepath.has_focus()) btn_open.grab_focus(); // one way to take focus off filepath entry

	if(ev->button == 1) {
		switch(editor_action) {
		  case ADD_ZONE:
			city.AddZone(select, city_canvas.MouseXY_to_ArchipelagoXY({ev->x, ev->y}));
			break;
		  case REMOVE_ZONE:
		  	city.RemoveZone(city_canvas.MouseXY_to_ArchipelagoXY({ev->x, ev->y}));
			break;
		  case REMOVE_LINK:
		  	city.RemoveLink(city_canvas.MouseXY_to_ArchipelagoXY({ev->x, ev->y}));
			break;
		  default: ;
		}
	}
	return 0;
}


void ArchipelagoUI::DragBegin_cb(double x, double y) {

	switch(editor_action) {
	  case EDIT_ZONE:
	  	city.PrepareEditZone(city_canvas.MouseXY_to_ArchipelagoXY({x, y}));
		break;
	  case ADD_LINK:
	  	city.PrepareAddLink(city_canvas.MouseXY_to_ArchipelagoXY({x, y}));
	  	break;
	  default: ;
	}
}

void ArchipelagoUI::DragUpdate_cb(double dx, double dy) {
	double x, y; gesture_drag->get_start_point(x, y);
	switch(editor_action) {
	  case EDIT_ZONE:
	  	city.EditZone(city_canvas.MouseXY_to_ArchipelagoXY({x+dx, y+dy}));
	  	tmpstuff.set_text(city.getEditZoneInfo());
		break;
	  case ADD_LINK:
	  	city.AddLink(city_canvas.MouseXY_to_ArchipelagoXY({x+dx, y+dy}));
		tmpstuff.set_text(city.getAddLinkInfo());
		break;
	  default: ;
	}
}

void ArchipelagoUI::DragEnd_cb(double dx, double dy) {
	double x, y; gesture_drag->get_start_point(x, y);
	switch(editor_action) {
	  case EDIT_ZONE:
	  	city.ValidateEditZone(city_canvas.MouseXY_to_ArchipelagoXY({x+dx, y+dy}));
		break;
	  case ADD_LINK:
	  	city.ValidateAddLink(city_canvas.MouseXY_to_ArchipelagoXY({x+dx, y+dy}));
	  	break;
	  default: ;
	}
}


void ArchipelagoUI::UpdateEditorAction(void) {

	if(btn_zone.get_active()) {
		if(btn_add.get_active()) 	{ editor_action = ADD_ZONE;		return ; }
		if(btn_edit.get_active()) 	{ editor_action = EDIT_ZONE;	return ; }
		if(btn_remove.get_active()) { editor_action = REMOVE_ZONE;	return ; }
	}
	if(btn_link.get_active()) {
		if(btn_add.get_active()) 	{ editor_action = ADD_LINK;		return ; }
		if(btn_remove.get_active()) { editor_action = REMOVE_LINK;	return ; }
	}
	editor_action = NONE;
}


void ArchipelagoUI::RefreshMouseCoordinates() {
    int x, y;
	city_canvas.get_pointer(x, y);
    Coord2D c = city_canvas.MouseXY_to_ArchipelagoXY({double(x), double(y)});
    messages.set_markup("<b>x:</b> "+ bsfn(c.x) +"  <b>y:</b> "+ bsfn(c.y));
}
