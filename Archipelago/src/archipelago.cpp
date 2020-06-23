#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturezoom.h>
#include "zones.hpp"
#include <memory.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "archipelago.hpp"

using uint = unsigned int;

struct Color {
    Color(int red, int green, int blue, double alpha = 1.0)
    : red{double(red)/255}, green{double(green)/255}, blue{double(blue)/255}, alpha{alpha} {}

    double red, green, blue, alpha;
};
void DrawLine(const Cairo::RefPtr<Cairo::Context>& cr, Color rgba, Coord2D A, Coord2D B) {
	cr->save();
    cr->set_source_rgba(rgba.red, rgba.green, rgba.blue, rgba.alpha);
	cr->move_to(A.x, A.y);
    cr->line_to(B.x, B.y);
	cr->stroke();
    cr->restore();
}

void DrawCircle(const Cairo::RefPtr<Cairo::Context>& cr, Color rgba, Coord2D center, double radius) {
    cr->save();
    cr->set_source_rgba(rgba.red, rgba.green, rgba.blue, rgba.alpha);
	cr->arc(center.x, center.y, radius, 0.0, 2*M_PI);
    cr->stroke();
    cr->restore();
}
void FillCircle(const Cairo::RefPtr<Cairo::Context>& cr, Color rgba, Coord2D center, double radius) {
    cr->save();
    cr->set_source_rgba(rgba.red, rgba.green, rgba.blue, rgba.alpha);
	cr->arc(center.x, center.y, radius, 0.0, 2*M_PI);
    cr->fill();
    cr->restore();
}
void DrawRectangle(const Cairo::RefPtr<Cairo::Context>& cr, Color rgba, Coord2D cornerTL, double width, double height) {
	cr->save();
    cr->set_source_rgba(rgba.red, rgba.green, rgba.blue, rgba.alpha);
    cr->rectangle(cornerTL.x, cornerTL.y, width, height);
    cr->stroke();
    cr->restore();
}
void FillRectangle(const Cairo::RefPtr<Cairo::Context>& cr, Color rgba, Coord2D cornerTL, double width, double height) {
	cr->save();
    cr->set_source_rgba(rgba.red, rgba.green, rgba.blue, rgba.alpha);
    cr->rectangle(cornerTL.x, cornerTL.y, width, height);
    cr->fill_preserve();
    cr->restore();
}


Archipelago::Archipelago(void) {
    zoom = 1;
    scale = 1;
    xoffset = 0; yoffset = 0;
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK  | Gdk::SMOOTH_SCROLL_MASK);

      m_GestureZoom = Gtk::GestureZoom::create(*this);
  m_GestureZoom->set_propagation_phase(Gtk::PHASE_CAPTURE);
  m_GestureZoom->signal_scale_changed().connect(sigc::mem_fun(*this, &Archipelago::zoomfn));
}


template<class... T>
bool ParseLineFromFile(std::ifstream& file, T&... args) {
    std::string line;
    getline(file, line);

    line.erase(std::find(line.begin(), line.end(), '#'), line.end()); // strip comment

    std::istringstream stream{line}; // parse line for args
    return (stream >> ... >> args) && !stream.fail();
}

// std::string StripPathFromFileName(std::string filename) {
//     filename.erase(filename.begin(), std::find(filename.begin(), filename.end(), '/')); // strip directory
//     filename.erase(std::find(filename.begin(), filename.end(), '.'), filename.end()); // strip extension
// }

#include <iostream>
void Archipelago::OpenFile(std::string filename) {

    Reset();

    std::ifstream file{filename};
    if(!file.is_open()) return ;
    // std::cout<<"openfile\n";
    uint idtype{0};
    while(!file.eof() && idtype < ZoneType::NbZoneTypes) {
        uint zones2read{0};
        if(ParseLineFromFile(file, zones2read)) {
            uint id, nb_people;
            double x, y;
            while(!file.eof() && zones2read) {
                if(ParseLineFromFile(file, id, x, y, nb_people)) {  // and far from all zones
                    if(SpacePermitsZone({x, y}, sqrt(nb_people))) {
                        CreateZone({id, ZoneType(idtype), {x, y}, nb_people});
                        // std::cout<<"create zone "<<id<<'\n';
                    }
                    zones2read -= 1;
                }
            }
            idtype += 1;
        }
    }
    while(!file.eof() && idtype == ZoneType::NbZoneTypes) {
        uint links2read{0};
        if(ParseLineFromFile(file, links2read)) {
            uint id1, id2;
            while(!file.eof() && links2read) {
                if(ParseLineFromFile(file, id1, id2)) {
                    if(LinkAllowed(id1, id2) and SpacePermitsLink(id1, id2)) {
                        ConnectZones(zones.at(id1), zones.at(id2));
                        // std::cout<<"link zones "<<id1<<' '<<id2<<'\n';
                    }
                    links2read -= 1; // will ignore entry if an id is incorrect
                }
            }
            idtype += 1;
        }
    }
    // std::cout<<zones.size()<<links.size()<<"openend\n";
    file.close();
}
#define watch(x) (#x) // return strin name
void Archipelago::SaveFile(std::string filename) {

    std::ofstream file{filename};
    if(!file.is_open()) return ;

    time_t clock = time(0);
    file <<"# "<< ctime(&clock);

    int idtype{-1};
    for(auto& [key, zone] : zones) {
        if(zone.type != ZoneType(idtype) && idtype < ZoneType::NbZoneTypes) {
            switch(++idtype) {
              case ProductionZone:  file <<"\n# Production Zones\n"; break;
              case ResidentialArea: file <<"\n# Residential Areas\n"; break;
              case TransportHub:    file <<"\n# Transport Hubs\n"; break;
              default:;
            }
            file << nb_zones[idtype] <<'\n';
        }
        file <<'\t'<< zone <<'\n';
    }
    file <<"\n# Links\n" << nb_links <<'\n';
    for(auto& link : links) {
        file <<'\t'<< link.first <<' '<< link.second <<'\n';
    }
    file.close();
}
// put msg in calling fn
bool Archipelago::SpacePermitsZone(Coord2D center, double radius) {

    for(auto& [key, zone] : zones) {
        if(DistancePoint2Point(zone.getCenter(), center) < (zone.getRadius() + radius)) {
            std::cout <<"ERROR - Zones "<< key <<" and "<< center.x<<' '<<center.y <<" overlap."<<DistancePoint2Point(zone.getCenter(), center)<<"\n";
            return false;
        }
    }
    return true;
}

void Archipelago::CreateZone(const Zone& z0) {
    zones.emplace(z0.id, z0);
    nb_zones[z0.type] += 1;
}

void Archipelago::DestroyZone(Zone& z0) {

    for(auto& [key, zone] : zones) {
        zone.RemoveLink(z0.id);
    }
    nb_zones[z0.type] -= 1;
    zones.erase(z0.id);

}


// put msg in calling fn
bool Archipelago::SpacePermitsLink(uint id1, uint id2) {

    for(auto& [key, zone] : zones) {
        if(key != id1 && key != id2 && DistancePoint2Line(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter()) < zone.getRadius()) {
            std::cout <<"ERROR - Link between zones " << id1 <<" and "<< id2
                      <<" passes through zone "<< key <<".\n";
            return false;
        }
    }
    return true;
}

bool Archipelago::LinkAllowed(uint id1, uint id2) {
    return zones.at(id1).LinkAllowed(id2) and zones.at(id2).LinkAllowed(id1);
}

void Archipelago::ConnectZones(Zone& z1, Zone& z2) {
    z1.AddLink(z2.id);
    z2.AddLink(z1.id);
    links.push_back(std::make_pair(z1.id, z2.id)); // check for existence
    nb_links += 1;
}

void Archipelago::DisconnectZones(Zone& z1, Zone& z2) {
    z1.RemoveLink(z2.id);
    z2.RemoveLink(z1.id);
    links.erase(find(links.begin(), links.end(), std::make_pair(z1.id, z2.id)));
    nb_links -= 1;
}





#define BLACK {  0,  0,  0}
#define RED   {255,  0,  0}
#define GREEN {  0,255,  0}
#define BLUE  {  0,  0,255}
// #define GREEN {165, 42, 42}
// #define notGREEN {0, 102, 51}
#define WHITE {255,255,255}

#include <gdkmm/screen.h>

// static Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
// int screenwidth = screen->get_width();
// int screenheight = screen->get_height();

#define SIGN(x) (x < 0 ? (-1) : (1))
void Archipelago::zoomfn(double change) {
    static double last = 1;
    //somehow works, some conditions are to verify
    if(last - change > 0.1 and 0.91 < change && change < 1.10) { // ignore first 10%
        last = 1;
        return ;
    }
    zoom = CLAMP(zoom*(1+0.02*SIGN(change-last)), 0.1326, 7.2446); // /100&x100 zoom
    last = change;
    std::cout<<change<<' '<<zoom<<'\n';
    // CLAMP(zoom, -10,10);
    queue_draw();
}




bool Archipelago::on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) {

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    FillRectangle(pencil, WHITE, {0, 0}, width, height);
    DrawRectangle(pencil, BLACK, {0, 0}, width, height);

    Cairo::Matrix matrix = Cairo::identity_matrix();
    matrix.translate(width/2 + xoffset, height/2 + yoffset);

    scale = CLAMP(zoom*MIN(width, height)/1000*2, 0.3, 3);
    matrix.scale(scale, scale);

    pencil->set_matrix(matrix);

    Coord2D center{width/2 + xoffset, height/2 + yoffset};
    if(zones.empty()) return 1;
    for(auto& link : links) {
        Coord2D c1{zones.at(link.first).getCenter().x, -zones.at(link.first).getCenter().y}; c1 = c1*scale + center;
        Coord2D c2{zones.at(link.second).getCenter().x, -zones.at(link.second).getCenter().y}; c2 = c2*scale + center;
        DrawLine(pencil, BLACK, c1, c2);
    }
    for(auto& [key, zone] : zones) {
        Coord2D c{zone.getCenter().x, -zone.getCenter().y}; c = c*scale + center; double r{zone.getRadius()*scale};
        switch(zone.type) {
          case ResidentialArea:
            FillCircle(pencil, WHITE, c, r);
            FillCircle(pencil, {0,0,255,0.5}, c, r);
            DrawCircle(pencil, BLUE, c, r);
            break;
          case TransportHub:
            FillCircle(pencil, WHITE, c, r);
            FillCircle(pencil, {0,255,0,0.2}, c, r);
            DrawLine(pencil, GREEN, {c.x + r, c.y}, {c.x - r, c.y});
            DrawLine(pencil, GREEN, {c.x + M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x - M_SQRT1_2*r, c.y - M_SQRT1_2*r});
            DrawLine(pencil, GREEN, {c.x, c.y + r}, {c.x, c.y - r});
            DrawLine(pencil, GREEN, {c.x - M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x + M_SQRT1_2*r, c.y - M_SQRT1_2*r});
            DrawCircle(pencil, GREEN, c, r);
            break;
          case ProductionZone:
            FillCircle(pencil, RED, c, r);
            DrawCircle(pencil, BLACK, c, r);
            FillRectangle(pencil, WHITE, {c.x - 0.7*r, c.y - 0.12*r}, 1.4*r, 0.24*r); // change sign after ok cordintes
            break;
          default:;
            // no other
        }
    }

    return 1;
}


void Archipelago::Reset(void) {
    zones.clear();
    links.clear();
    nb_zones[0] = 0;
    nb_zones[1] = 0;
    nb_zones[2] = 0;
    nb_links = 0;
    ResetView();
}

void Archipelago::ResetView(void) {
    zoom = 1;
    scale = 1;
    xoffset = 0;
    yoffset = 0;
}