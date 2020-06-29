#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturezoom.h>
#include "zones.hpp"
#include <memory.h>
#include <sstream>
#include <cmath>
#include <fstream>
#include <vector>
#include "archipelago-bg.hpp"
#include "graphics.hpp"

using uint = unsigned int;

Archipelago::Archipelago(void) {
    m_zoom = 1;
    scale = 1;
    view = Cairo::identity_matrix();
    Tx = 0; Ty = 0;
    selected_zone = 0;
    // pop_menu.append(move);
    // pop_menu.append(resize);
    // pop_menu.append(connect);
    // pop_menu.append(remove);
    // pop_menu.show_all();
    // 	pop_menu.accelerate(*this);


    // add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK  | Gdk::SMOOTH_SCROLL_MASK);
    OpenFile("debug/success/S01.txt");
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
bool Archipelago::OpenFile(std::string filename) {

    Reset();

    std::ifstream file{filename};
    if(!file.is_open()) return 0;
    // std::cout<<"openfile\n";
    uint idtype{0};
    while(!file.eof() && idtype < ZoneType::NbZoneTypes) {
        uint zones2read{0};
        if(ParseLineFromFile(file, zones2read)) {
            uint id, nb_people;
            double x, y;
            while(!file.eof() && zones2read) {
                if(ParseLineFromFile(file, id, x, y, nb_people)) {
                    if(SpacePermitsZone({x, y}, sqrt(nb_people))) {
                        CreateZone({id, ZoneType(idtype), {x, y}, nb_people});
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
                    }
                    links2read -= 1;
                }
            }
            idtype += 1;
        }
    }
    // std::cout<<zones.size()<<links.size()<<"openend\n";
    file.close();
    return 1;
}
#define watch(x) (#x) // return strin name
void Archipelago::SaveFile(std::string filename) {

    std::ofstream file{filename};
    if(!file.is_open()) return ;

    time_t clock = time(0);
    file <<"# "<< ctime(&clock);

    int idtype{-1}, count{0};
    while(idtype < ZoneType::NbZoneTypes) {
        switch(++idtype) {
            case ProductionZone:  file <<"\n# Production Zones\n"; break;
            case ResidentialArea: file <<"\n# Residential Areas\n"; break;
            case TransportHub:    file <<"\n# Transport Hubs\n"; break;
            default:;
        }
        file << nb_zones[idtype] <<'\n';
        for(auto& [key, zone] : zones) {
            if(zone.type == idtype) {
                file <<'\t'<< zone <<'\n';
                if(++count == nb_zones[idtype]) { //avoid looping when done
                    break;
                }
            }
        }
        count = 0;
    }
    file <<"\n# Links\n" << nb_links <<'\n';
    for(auto& link : links) {
        file <<'\t'<< link.first <<' '<< link.second <<'\n';
    }
    file.close();
}
// put msg in calling fn
bool Archipelago::SpacePermitsZone(Coord2D center, double radius, uint id) const {

    for(auto& [key, zone] : zones) {
        if(key != id && DistancePoint2Point(zone.getCenter(), center) < (zone.getRadius() + radius)) {
            std::cout <<"ERROR - Zones "<< key <<" and "<< center.x<<' '<<center.y <<" id "<<id<<" overlap.\n";
            return false;
        }
    }
    for(auto& [id1, id2] : links) {
        if(not SpacePermitsLink(id1, id2)) {
            return false;
        }
    }
    return true;
}

bool Archipelago::SpacePermitsZone(const Zone& z0) const {
    Coord2D c{z0.getCenter()};
    double  r{z0.getRadius()};
    return SpacePermitsZone(c, r, z0.id);
}

void Archipelago::CreateZone(const Zone& z0) {
    zones.emplace(z0.id, z0);
    nb_zones[z0.type] += 1;
    queue_draw();
}

void Archipelago::DestroyZone(Zone& z0) {

    for(auto [id1, id2]: links) {
        if(id1 == z0.id or id2 == z0.id) {
            DisconnectZones(zones.at(id1), zones.at(id2));
        }
    }
    nb_zones[z0.type] -= 1;
    zones.erase(z0.id);
    queue_draw();
}


// put msg in calling fn
bool Archipelago::SpacePermitsLink(uint id1, uint id2) const {

    for(auto& [key, zone] : zones) {
        if(key != id1 && key != id2 && DistancePoint2Segment(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter()) < zone.getRadius()) {
            std::cerr <<"ERROR - Link between zones "<< id1 <<"("<<zones.at(id1).getCenter().x<<','<<zones.at(id1).getCenter().y<<")"<<" and "<< id2<<"("<<zones.at(id2).getCenter().x<<','<<zones.at(id2).getCenter().y<<")"
                      <<" passes through zone "<< key<<"("<<zone.getCenter().x<<','<<zone.getCenter().y<<") dist: " <<DistancePoint2Line(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter())<<" vs "<<zone.getRadius()<<".\n";
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
    links.push_back({z1.id, z2.id}); // check for existence
    nb_links += 1;
    queue_draw();
}

void Archipelago::DisconnectZones(Zone& z1, Zone& z2) {
    z1.RemoveLink(z2.id);
    z2.RemoveLink(z1.id);
    links.erase(find(links.begin(), links.end(), std::make_pair(z1.id, z2.id)));
    nb_links -= 1;
    queue_draw();
}





// #define black Color(  0,  0,  0)
#define RED   Color(255,  0,  0)
#define GREEN Color(  0,255,  0)
#define BLUE  Color(  0,  0,255)
// #define GREEN {165, 42, 42}
// #define notGREEN {0, 102, 51}
// #define white {255,255,255}


void Archipelago::Zoom(double sign) {
    // deg = lastdeg;
    m_zoom = CLAMP(m_zoom*(1+sign*0.02), 0.1326, 7.2446);
    queue_draw();
}
void Archipelago::Rotate(double sign) {
    Rz = std::fmod(Rz + sign, 360);
    deg = 2*Rz*M_PI/180;
    queue_draw();
}


void Archipelago::DragStart(Coord2D pos) {

    for(auto& [key, zone] : zones) {
        if(DistancePoint2Point(pos, zone.getCenter()) < zone.getRadius()) {
            selected_zone = key;
            drag_select_offset = pos - zone.getCenter();
            origin = zone.getCenter();
            nb = zone.getRadius();
            break;
        }
    }
}

void Archipelago::DragUpdate(Coord2D pos) {
    if(moveFlag)
    if(selected_zone) {
        zones.at(selected_zone).setCenter(pos - drag_select_offset);

        queue_draw();
    }
    if(resizeFlag)
    if(selected_zone) {
        zones.at(selected_zone).setRadius(CLAMP(DistancePoint2Point(origin, pos), 10, 300));
        queue_draw();
    }
    if(connectFlag && selected_zone) {
        tmpline = true;
        tmpcoord = pos;
        queue_draw();
    }
}

void Archipelago::DragEnd(Coord2D pos) {
    if(selected_zone && (moveFlag or resizeFlag))
    if(not SpacePermitsZone(zones.at(selected_zone))) {
        zones.at(selected_zone).setCenter(origin);
        zones.at(selected_zone).setRadius(nb);
        // queue_draw();
    }
    if(selected_zone && connectFlag)
    for(auto& [key, zone] : zones) {
        if(key != selected_zone && DistancePoint2Point(pos, zone.getCenter()) < zone.getRadius()) {
            if(LinkAllowed(selected_zone, key) and SpacePermitsLink(selected_zone, key)) {
                ConnectZones(zones.at(selected_zone), zone);
                std::cout<<"connected: "<<selected_zone<<" and "<<key<<'\n';
            }
            break;
        }
    }
    ResetFlags();
    queue_draw();
    selected_zone = 0;
}

void Archipelago::Swipe(double dx, double dy) {
    Tx -= dx;
    Ty -= dy;
    queue_draw();
}
bool Archipelago::on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) {

    UpdateCoordinates();
    Draw(pencil, {{0, 0}, {width, 0}, {width, height}, {0, height}}, {black, 0.6}, red);
    pencil->save();
    // view = Cairo::identity_matrix();
    // pencil->translate(width/2, height/2);
    pencil->translate(center.x, center.y);
    // pencil->rotate_degrees(tmprotate);
    // pencil->scale(tmpzoom, -tmpzoom);
    pencil->rotate(deg);
    pencil->scale(scale, -scale);
    // pencil->set_matrix(view);
    Draw(pencil, {Coord2D(-20, 60),Coord2D(-20, -60),Coord2D(20, -60),Coord2D(20, 60)}, white);
    Draw(pencil, {Coord2D(60,20),Coord2D(60,-20),Coord2D(-60,-20),Coord2D(-60,20)}, white);

    if(zones.empty() && !tmpline) return 1;
    for(auto& link : links) {
        Draw(pencil, {zones.at(link.first).getCenter(), zones.at(link.second).getCenter()}, black);
    }
    if(tmpline) Draw(pencil, {zones.at(selected_zone).getCenter(), tmpcoord}, {black, 0.9});
    for(auto& [key, zone] : zones) {
        Coord2D c{zone.getCenter()}; double r{zone.getRadius()};
        switch(zone.type) {
          case ResidentialArea:
            Draw(pencil, {c, r});
            Draw(pencil, {c, r}, white);
            Draw(pencil, {c, r}, blue, {0,0,255,0.5});
            Draw(pencil, {{c.x, c.y-0.75*r}, {c.x-0.5*sqrt(3)*0.75*r, c.y+0.5*0.75*r}, {c.x+0.5*sqrt(3)*0.75*r, c.y+0.5*0.75*r}}, white);
            Draw(pencil, {{c.x, c.y+0.75*r}, {c.x-0.5*sqrt(3)*0.75*r, c.y-0.5*0.75*r}, {c.x+0.5*sqrt(3)*0.75*r, c.y-0.5*0.75*r}}, white);
            Draw(pencil, {{c.x, c.y-0.60*r}, {c.x-0.5*sqrt(3)*0.60*r, c.y+0.5*0.60*r}, {c.x+0.5*sqrt(3)*0.60*r, c.y+0.5*0.60*r}}, {142, 48, 136}, {142, 48, 136});
            Draw(pencil, {{c.x, c.y+0.60*r}, {c.x-0.5*sqrt(3)*0.60*r, c.y-0.5*0.60*r}, {c.x+0.5*sqrt(3)*0.60*r, c.y-0.5*0.60*r}}, {142, 48, 136}, {142, 48, 136});
            Draw(pencil, Point2D(c.x, c.y));
            break;
          case TransportHub:
            Draw(pencil, {c, r}, white);
            pencil->set_line_width(4);
            Draw(pencil, {c, r}, green, {0,255,0,0.2});
            pencil->set_line_width(1);
            Draw(pencil, {{c.x + r, c.y}, {c.x - r, c.y}}, green);
            Draw(pencil, {{c.x + M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x - M_SQRT1_2*r, c.y - M_SQRT1_2*r}}, green);
            Draw(pencil, {{c.x, c.y + r}, {c.x, c.y - r}}, GREEN);
            Draw(pencil, {{c.x - M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x + M_SQRT1_2*r, c.y - M_SQRT1_2*r}}, green);
            break;
          case ProductionZone:
            Draw(pencil, {c, r}, black, red);
            Draw(pencil, Quadrilateral2D({c.x - 0.7*r, c.y - 0.12*r}, {c.x + 0.7*r, c.y - 0.12*r},
                                         {c.x + 0.7*r, c.y + 0.12*r}, {c.x - 0.7*r, c.y + 0.12*r}), white); // change sign after ok cordintes
            // Draw(pencil, {{Coord2D(-40,-70)+c,Coord2D(80,-30)+c,Coord2D(0,0)+c,Coord2D(20,70)+c,Coord2D(-90,0)+c}});
            break;
          default:;
        }
    }
    view = pencil->get_matrix();
    pencil->restore();
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
    m_zoom = 1;
    scale = 1;
    deg = 0;
    Tx = 0;
    Ty = 0;
}


void Archipelago::on_button_press_event2(int button, Coord2D pos) {
    UpdateCoordinates();

    if(removeFlag && button == 1) {
        for(auto& [key, zone] : zones) {
            if(DistancePoint2Point(pos, zone.getCenter()) < zone.getRadius()) {
                DestroyZone(zone);
                return;
            }
        }
        for(auto& [id1, id2] : links) {
            if(DistancePoint2Line(pos, zones.at(id1).getCenter(), zones.at(id2).getCenter()) < 5) {
                DisconnectZones(zones.at(id1), zones.at(id2));
                return;
            }
        }
    }
    if(removeFlag && button == 3) {
        removeFlag = 0;
    }
}

void Archipelago::on_button_release_event2(int button, Coord2D pos) {
    UpdateCoordinates();


    if(button == 3) {

    }
}


void Archipelago::UpdateCoordinates(void) {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    center = {width/2 + Tx, height/2 - Ty};
    scale = CLAMP(m_zoom*MIN(width, height)/500, 0.3, 3);
}

Coord2D Archipelago::MouseXY_to_ArchipelagoXY(Coord2D mouse) {
    return { 1/scale*(cos(deg)*(mouse.x - center.x) + sin(deg)*(mouse.y - center.y)),
            -1/scale*(cos(deg)*(mouse.y - center.y) - sin(deg)*(mouse.x - center.x))};
}