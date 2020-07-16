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

Archipelago::Archipelago(void): varzone{Zone(0, Zone::Type::NONE, {0, 0}, 0)} {
    Ox = 0; Oy = 0; S = 1; Tx = 0; Ty = 0; Rz = 0;
    zone_edit = 0; link_edit = 0;
    selected_zone = 0;

    OpenFile("test.txt");
}

template<class... T>
bool ParseLineFromFile(std::ifstream& file, T&... args) {
    std::string line; getline(file, line);

    line.erase(std::find(line.begin(), line.end(), '#'), line.end()); // strip comment

    std::istringstream stream{line}; // parse line for args
    return (stream >> ... >> args) && !stream.fail();
}

// std::string StripPathToFileName(std::string filename) {
//     filename.erase(filename.begin(), std::find(filename.begin(), filename.end(), '/')); // strip directory
//     filename.erase(std::find(filename.begin(), filename.end(), '.'), filename.end()); // strip extension
// }

#include <iostream>
bool Archipelago::OpenFile(std::string filename) {
    Reset();
    std::ifstream file{filename};
    if(!file.is_open()) return 0;

    int i{0}; Zone::Type zonetype{Zone::Types[i]};
    while(!file.eof() && i < Zone::Types.size()) {
        uint zones2read{0};
        if(ParseLineFromFile(file, zones2read)) {
            uint id, nb_people;
            double x, y;
            while(!file.eof() && zones2read) {
                if(ParseLineFromFile(file, id, x, y, nb_people)) {
                    CreateZone(zonetype, {x, y}, nb_people, id);
                    zones2read -= 1;
                }
            }
            zonetype = Zone::Types[++i];
        }
    }
    while(!file.eof()) {
        uint links2read{0};
        if(ParseLineFromFile(file, links2read)) {
            uint id1, id2;
            while(!file.eof() && links2read) {
                if(ParseLineFromFile(file, id1, id2)) {
                    ConnectZones(id1, id2);
                    links2read -= 1;
                }
            }
            break;
        }
    }
    file.close();
    ComputePerformance();
    std::cout<<"init success\n";
    return 1;
}

void Archipelago::SaveFile(std::string filename) {

    std::ofstream file{filename};
    if(!file.is_open()) return ;

    time_t clock = time(0);
    file <<"# "<< ctime(&clock);

    for(auto zonetype : Zone::Types) {
        switch(zonetype) {
          case Zone::ProductionZone:
            file <<"\n# Production Zones\n"; break;
          case Zone::ResidentialArea:
            file <<"\n# Residential Areas\n"; break;
          case Zone::TransportHub:
            file <<"\n# Transport Hubs\n"; break;
          default:;
        }
        file << zones_by_type.at(zonetype).size() <<'\n';
        for(auto id : zones_by_type.at(zonetype)) {
            file <<'\t'<< zones.at(id) <<'\n';
        }
    }
    file <<"\n# Links\n";
    file << links.size() <<'\n';
    for(auto& link : links) {
        file <<'\t'<< link.id1 <<' '<< link.id2 <<'\n';
    }
    file.close();
}

bool Archipelago::ZoneAllowed(uint id) const {
    return zones.find(id) == zones.end();
}

bool Archipelago::SpacePermitsZone(Coord2D center, double radius, uint id) const {

    for(auto& [key, zone] : zones) {
        if(key != id && DistancePoint2Point(zone.getCenter(), center) < (zone.getRadius() + radius)) {
            //std::cout <<"ERROR - Zones "<< key <<" and "<< center.x<<' '<<center.y <<" id "<< id <<" overlap.\n";
            return false;
        }
    }
    // for(auto& [id1, id2] : links) {
    for(auto& link : links) {
        if(not SpacePermitsLink(link.id1, link.id2)) {
            return false;
        }
    }
    return true;
}

bool Archipelago::SpacePermitsZone(const Zone& z0) const {
    // Coord2D c{z0.getCenter()};
    // double  r{z0.getRadius()};
    return SpacePermitsZone(z0.getCenter(), z0.getRadius(), z0.id);
}

void Archipelago::CreateZone(Zone::Type zonetype, Coord2D pos, uint nb_people, uint id) {
    static uint max_id{0};
    //TODO: params validation check
    if(zonetype != Zone::NONE && SpacePermitsZone(pos, sqrt(nb_people))) {
        //std::cout<<"Create 1 ("<<id<<")\n";
        id ? max_id = MAX(id, max_id) : id = ++max_id;
        zones.emplace(id, Zone(id, zonetype, pos, nb_people));
        //std::cout<<"Create 2 - "<<zones_by_type.size()<<'\n';
        // nb_zones += 1;
        zones_by_type.at(zonetype).push_back(id);
        //std::cout<<"Create 3\n";
        // ComputePerformance();
        queue_draw();
    }
}



void Archipelago::DestroyZone(Zone& z0) {

    // for(auto& [id1, id2]: links) {
    for(int i{0}; i < links.size(); ++i) {
        if(links[i].id1 == z0.id or links[i].id2 == z0.id) {
            //std::cout<<"Destroy 0\n";
            DisconnectZones(zones.at(links[i].id1), zones.at(links[i].id2));
            --i;
        }
    }
    //std::cout<<"Destroy 1 ("<<z0.id<<")\n";
    zones_by_type.at(z0.type).erase(std::find(zones_by_type.at(z0.type).begin(), zones_by_type.at(z0.type).end(), z0.id));
    //std::cout<<"Destroy 2\n";
    // nb_zones -= 1;
    zones.erase(z0.id);
    //std::cout<<"Destroy 3\n";
    // ComputePerformance();
    queue_draw();
}


// put msg in calling fn
bool Archipelago::SpacePermitsLink(uint id1, uint id2) const {

    for(auto& [key, zone] : zones) {
        if(key != id1 && key != id2 && DistancePoint2Segment(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter()) < zone.getRadius()) {
            std::cerr <<"ERROR - Link between zones "<< id1 <<"("<<zones.at(id1).getCenter().x<<','<<zones.at(id1).getCenter().y<<")"<<" and "<< id2<<"("<<zones.at(id2).getCenter().x<<','<<zones.at(id2).getCenter().y<<")"
                      <<" passes through zone "<< key<<"("<<zone.getCenter().x<<','<<zone.getCenter().y<<") dist: " <<DistancePoint2Segment(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter())<<" vs "<<zone.getRadius()<<".\n";
            return false;
        }
    }
    return true;
}

bool Archipelago::LinkAllowed(uint id1, uint id2) {
    return zones.count(id1) && zones.count(id2) && zones.at(id1).LinkAllowed(id2) and zones.at(id2).LinkAllowed(id1);
}


// void Archipelago::ConnectZones(Zone& z1, Zone& z2) {
bool Archipelago::ConnectZones(uint id1, uint id2) {
    //TODO: params validation check
    /* remove?
    z1.AddLink(z2.id);
    z2.AddLink(z1.id);
    */

    if(LinkAllowed(id1, id2) and SpacePermitsLink(id1, id2)) {
        float speed = (zones.at(id1).type == Zone::TransportHub && zones.at(id2).type == Zone::TransportHub) ? 2 : 1;
        links.push_back({id1, id2, speed, fDistancePoint2Point(zones.at(id1).getCenter(), zones.at(id2).getCenter())}); // check for existence


        // auto f1 = [id2](uint id) { return id == id2; };
        // std::cout<<"-- -- -- "; Dijkstra(zones.at(id1), id2);for(auto zone : connection) std::cout<<zone<<' ';std::cout<<'\n';
        // nb_links += 1;
        // ComputePerformance();
        queue_draw();
        return 1;
    }
    return 0;
}

void Archipelago::DisconnectZones(Zone& z1, Zone& z2) {
    /* remove?
    z1.RemoveLink(z2.id);
    z2.RemoveLink(z1.id);
    */
    for(int i{0}; i < links.size(); ++i) {
        if((links[i].id1 == z1.id and links[i].id2 == z2.id) or (links[i].id1 == z2.id and links[i].id2 == z1.id)) {
            links.erase(links.begin() + i);
            break;
        }
    }
    // links.erase(std::find(links.begin(), links.end(), {z1.id, z2.id}));
    // nb_links -= 1;
    // ComputePerformance();
    queue_draw();
}




void Archipelago::Scale(double sign) {
    static double zoom{1};
    if(S == 1) zoom = 1; // for smooth view reset

    zoom = CLAMP(zoom*(1 + sign*0.02), 0.1326, 7.2446);
    // slightly adapt view to windows size
    S = CLAMP(zoom*MIN(width, height)/500, 0.3, 3);
    queue_draw();
}

void Archipelago::Rotate(double sign) {
    static double deg{0};
    if(Rz == 0) deg = 0; // for smooth view reset

    deg = std::fmod(deg + sign, 360);
    // convert to radians for direct use
    Rz = 2*deg*M_PI/180;
    queue_draw();
}

void Archipelago::Translate(double dx, double dy) {
    Tx -= dx;
    Ty -= dy;
    queue_draw();
}

void Archipelago::Origin(void) {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    Ox = width/2;
    Oy = height/2;
    queue_draw();
}

void Archipelago::UpdateViewModifiers(void) {
    // Ox, Oy and S depend on window size
    Origin();
    Scale();
    // Tx, Ty and Rz are unchanged
}

void Archipelago::ResetViewModifiers(void) {
    Origin();
    S  = 1;
    Rz = 0;
    Tx = 0;
    Ty = 0;
    queue_draw();
}

//##########################   EDITOR    #################################

uint Archipelago::IdentifyZoneFromXY(Coord2D position) {
    for(auto& [key, zone] : zones) {
        if(DistancePoint2Point(position, zone.getCenter()) < zone.getRadius()) { //make this zone.distance(pt)
            return key;
        }
    }
    return 0;
}


bool Archipelago::AddZone(Zone::Type zonetype, Coord2D position, uint nb_people, uint id) {
    CreateZone(zonetype, position, nb_people, id);
    // ComputePerformance();
    return 1;
}
// bool Archipelago::ModifyZone(Coord2D origin, EditState state, Coord2D change) {
bool Archipelago::ModifyZone(Coord2D position, EditState state) {
    static bool resize{0}, move{0};
    static Vector2D offset; // visual selection gap

    if(state != INIT && not zone_edit) return 0;
    switch(state) {
      case INIT:
        for(auto& [key, zone] : zones) {
            if(BOUNDED(DistancePoint2Point(position, zone.getCenter()), zone.getRadius()-5, zone.getRadius()+5) {
                selected_zone = key;
                resize = 1;
                break;
            } else if(DistancePoint2Point(position, zone.getCenter()) < zone.getRadius()-5) {
                selected_zone = key;
                move = 1;
                break;
            }
        }
        if(selected_zone) {
            zone_edit = 1;
            varzone.setCenter(zones.at(selected_zone).getCenter());
            varzone.setRadius(zones.at(selected_zone).getRadius());
            offset = position - zones.at(selected_zone).getCenter();
        }
        return 0;
      break;
      case UPDATE:
        if(resize) {
            zones.at(selected_zone).setRadius(CLAMP(DistancePoint2Point(varzone.getCenter(), position), 10, 300));
            edit_text = "Resizing zone "+ std::to_string(selected_zone) +" to fit "+ std::to_string(zones.at(selected_zone).getPopulation()) +" people.";
        }
        if(move) {
            zones.at(selected_zone).setCenter(position - offset);
            edit_text = "Moving zone "+ std::to_string(selected_zone) +" \nto X: "+ std::to_string(zones.at(selected_zone).getCenter().x) +" Y: "+ std::to_string(zones.at(selected_zone).getCenter().y);
        }
        break;
      case END:
        if(not SpacePermitsZone(zones.at(selected_zone))) {
            if(move) {
                zones.at(selected_zone).setCenter(varzone.getCenter());
            }
            if(resize) {
                zones.at(selected_zone).setRadius(varzone.getRadius());
            }
            queue_draw();
        }
        zone_edit = 0;
        move = 0;
        resize = 0;
        selected_zone = 0;
      break;
    }
    ComputePerformance();
    queue_draw();
    return 0;
}
bool Archipelago::RemoveZone(Coord2D position) {
    selected_zone = IdentifyZoneFromXY(position);
    if(selected_zone) {
        DestroyZone(zones.at(selected_zone));
        selected_zone = 0;
        ComputePerformance();
        return 1;
    }
    return 0;
}


// bool Archipelago::AddLink(Coord2D origin, EditState state, Coord2D change) {
bool Archipelago::AddLink(Coord2D position, EditState state) {
    if(state != INIT && not link_edit) return 0;

    switch(state) {
      case INIT:
        selected_zone = IdentifyZoneFromXY(position);
        if(selected_zone) {
            link_edit = 1;
            varzone.setCenter(zones.at(selected_zone).getCenter());
        }
        return 0;
      break;
      case UPDATE:
        varzone.setCenter(position);
        edit_text = "Connecting zone "+ std::to_string(selected_zone) +" to "+ std::to_string(IdentifyZoneFromXY(position));
      break;
      case END:
        ConnectZones(selected_zone, IdentifyZoneFromXY(position));
        link_edit = 0;
        selected_zone = 0;
        ComputePerformance();
      break;
    }
    queue_draw();
    return 0;
}
bool Archipelago::RemoveLink(Coord2D position) {
    if(!IdentifyZoneFromXY(position)) {
        // for(auto& [id1, id2] : links) {
        for(auto& link : links) {
            if(DistancePoint2Segment(position, zones.at(link.id1).getCenter(), zones.at(link.id2).getCenter()) < 5) {
                DisconnectZones(zones.at(link.id1), zones.at(link.id2));
                ComputePerformance();
                return 1;
            }
        }
    }
    return 0;
}

bool Archipelago::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    cr->save();
    UpdateViewModifiers();
    ::Draw(cr, Quadrilateral2D({0,0}, {width,0}, {width,height}, {0,height}));

    cr->translate(Ox + Tx, Oy + Ty);
    cr->rotate(Rz);
    cr->scale(S, -S);

    // for(auto& [id1, id2] : links) {
    cr->save();
    for(auto& link : links) {
        // ::Draw(cr, Segment2D(zones.at(id1).getCenter(), zones.at(id2).getCenter()), (zones.at(id1).type == Zone::TransportHub && zones.at(id2) == Zone::TransportHub) ? Color(0,147,0) : black);
        cr->set_line_width((link.speed > 1) ? 2 : 1);
        ::Draw(cr, Segment2D(zones.at(link.id1).getCenter(), zones.at(link.id2).getCenter()), ((link.marked) ? ((link.speed > 1) ? red :Color(255,123,0)) : ((link.speed > 1) ? Color(0,147,0) : black)));
    }
    cr->restore();
    if(link_edit) ::Draw(cr, Segment2D(zones.at(selected_zone).getCenter(), varzone.getCenter()), {255, 140, 0});

    for(auto& [key, zone] : zones) {
        if(zone_edit && key == selected_zone) {
            continue; // to display on top, draw last
        } else {
            Draw(cr, zone);
        }
    }
    if(zone_edit) Draw(cr, zones.at(selected_zone));
    cr->restore();
    return 1; // return 1 == do not propagate, event has been dealt with
}

void Archipelago::Draw(const Cairo::RefPtr<Cairo::Context>& cr, Zone& zone) {
    Coord2D c{zone.getCenter()}; double r{zone.getRadius()};
    switch(zone.type) {
      case Zone::ResidentialArea:
        ::Draw(cr, {c, r}, white);
        ::Draw(cr, {c, r}, blue, {0,0,255,0.5});
        ::Draw(cr, {{c.x, c.y-0.75*r}, {c.x-0.5*sqrt(3)*0.75*r, c.y+0.5*0.75*r}, {c.x+0.5*sqrt(3)*0.75*r, c.y+0.5*0.75*r}}, white);
        ::Draw(cr, {{c.x, c.y+0.75*r}, {c.x-0.5*sqrt(3)*0.75*r, c.y-0.5*0.75*r}, {c.x+0.5*sqrt(3)*0.75*r, c.y-0.5*0.75*r}}, white);
        ::Draw(cr, {{c.x, c.y-0.60*r}, {c.x-0.5*sqrt(3)*0.60*r, c.y+0.5*0.60*r}, {c.x+0.5*sqrt(3)*0.60*r, c.y+0.5*0.60*r}}, {142, 48, 136}, {142, 48, 136});
        ::Draw(cr, {{c.x, c.y+0.60*r}, {c.x-0.5*sqrt(3)*0.60*r, c.y-0.5*0.60*r}, {c.x+0.5*sqrt(3)*0.60*r, c.y-0.5*0.60*r}}, {142, 48, 136}, {142, 48, 136});
        break;
      case Zone::TransportHub:
        ::Draw(cr, {c, r}, white);
        cr->set_line_width(4);
        ::Draw(cr, {c, r}, green, {0,255,0,0.2});
        cr->set_line_width(1);
        ::Draw(cr, {{c.x + r, c.y}, {c.x - r, c.y}}, green);
        ::Draw(cr, {{c.x + M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x - M_SQRT1_2*r, c.y - M_SQRT1_2*r}}, green);
        ::Draw(cr, {{c.x, c.y + r}, {c.x, c.y - r}}, green);
        ::Draw(cr, {{c.x - M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x + M_SQRT1_2*r, c.y - M_SQRT1_2*r}}, green);
        break;
      case Zone::ProductionZone:
        ::Draw(cr, {c, r}, black, red);
        ::Draw(cr, Quadrilateral2D({c.x - 0.7*r, c.y - 0.12*r}, {c.x + 0.7*r, c.y - 0.12*r},
                                    {c.x + 0.7*r, c.y + 0.12*r}, {c.x - 0.7*r, c.y + 0.12*r}), white);
        break;
      default:;
    }
    // if(std::find(connection.begin(), connection.end(), zone.id) != connection.end()) ::Draw(cr, {c, r}, black, red);
}

void Archipelago::Reset(void) {
    zones.clear();
    links.clear();
    for(auto& [key, count] : Zone::Counters) {
        count = 0;
    }
    zones_by_type.clear();
    for(auto zonetype : Zone::Types) {
        zones_by_type.emplace(zonetype, std::vector<uint>());
        //std::cout<<"count init ("<<zonetype<<") = "<<zones_by_type.size()<<'\n';
    }
    // nb_links = 0;
    ResetViewModifiers();
}





std::string Archipelago::InfoCoordinates(Coord2D position) {
    uint tmp = IdentifyZoneFromXY(position);
    if(tmp) {
        switch(zones.at(tmp).type) {
          case Zone::ProductionZone: return "Production Zone "+ std::to_string(tmp) + "\nPopulation: "+ std::to_string(zones.at(tmp).getPopulation());
          case Zone::TransportHub: return "Transport Hub "+ std::to_string(tmp) + "\nPopulation: "+ std::to_string(zones.at(tmp).getPopulation());
          case Zone::ResidentialArea: return "Residential Area "+ std::to_string(tmp) + "\nPopulation: "+ std::to_string(zones.at(tmp).getPopulation());
        //   case Zone::ProductionZone: return "<b>Production Zone</b> "+ std::to_string(tmp) + "\n<b>Population:</b> "+ std::to_string(zones.at(tmp).getPopulation());
        //   case Zone::TransportHub: return "<b>Transport Hub</b> "+ std::to_string(tmp) + "\n<b>Population:</b> "+ std::to_string(zones.at(tmp).getPopulation());
        //   case Zone::ResidentialArea: return "<b>Residential Area</b> "+ std::to_string(tmp) + "\n<b>Population:</b> "+ std::to_string(zones.at(tmp).getPopulation());
          default :;
        }
    }
    // for(auto& [id1, id2] : links) {
    for(auto& link : links) {
        if(DistancePoint2Segment(position, zones.at(link.id1).getCenter(), zones.at(link.id2).getCenter()) < 5) {
            return "Link between "+ std::to_string(link.id1) +" and "+ std::to_string(link.id2) +"\nSpeed: 0";
            // return "<b>Link</b> between "+ std::to_string(id1) +" and "+ std::to_string(id2) +"\n<b>Speed:</b> 0";
        }
    }
    return "City Name :)";
}

Coord2D Archipelago::MouseXY_to_ArchipelagoXY(Coord2D mouse) {
    return { 1/S*(cos(Rz)*(mouse.x - Ox - Tx) + sin(Rz)*(mouse.y - Oy - Ty)),
            -1/S*(cos(Rz)*(mouse.y - Oy - Ty) - sin(Rz)*(mouse.x - Ox - Tx))};
}


void Archipelago::ComputePerformance(void) {
    static int i{0};
    std::cout<<i++<<" UPDATE PERFORMANCE\n";

    double ENJ = ComputeENJ();
    double CI = ComputeCI();
    double MTA = ComputeMTA();

    std::cout <<"ENJ "<<ENJ<<'\n';
    std::cout <<"CI "<<CI<<'\n';
    std::cout <<"MTA "<<MTA<<'\n';
}

double Archipelago::ComputeENJ(void) {
    int result{0}, normalize{0};
    for(auto [zonetype, ids] : zones_by_type) {
        uint tmp{0};
        for(auto id : ids) {
            tmp += zones.at(id).getPopulation();
        }
        switch(zonetype) {
          case Zone::ProductionZone:
            result += tmp;
            normalize += tmp;
            break;
          case Zone::ResidentialArea:
            result += tmp;
            normalize += tmp;
            break;
          case Zone::TransportHub:
            result -= tmp;
            normalize += tmp;
            break;
            default:;
        }
    }
    return double(result)/double(normalize);
}

double Archipelago::ComputeCI(void) {
    double result{0};
    for(auto link : links) {
        result += link.distance*MIN(zones.at(link.id1).getPopulation(), zones.at(link.id2).getPopulation())*link.speed;
    }
    return result;
}

double Archipelago::ComputeMTA(void) {
    double result{0};
    for(auto id : zones_by_type.at(Zone::ResidentialArea)) {
        std::cout<<"LOOP-2\n";
        for(auto zonetype : Zone::Types) {
            std::cout<<"LOOP-1\n";
            if(zonetype != Zone::ResidentialArea) {
                std::cout<<"loop id! "<<id<<'\n';
                Dijkstra(zones.at(id), 0, zonetype);
                // std::cout <<'\n';
                result += total_time;
            }
            std::cout<<"LOOP+1\n";
        }
        std::cout<<"LOOP+2\n";
    }
    return result/zones_by_type.at(Zone::ResidentialArea).size();
}


void Archipelago::Dijkstra(const Zone& z1, uint target_id, Zone::Type target_type) {
    // total_time = 0;
    connection.clear();
 //////////////0 DEFINE NEEDED STUFF FOR DIJKSTRA
    struct DijkstraNode {
        bool  visited; // has it the been evaluated, RENAME DONE?
        bool  stop;    // should outgoing connections be considered
        float time;    // shortest access time
        uint  prev;    // id of the node from which it was accessed the fastest
        DijkstraNode(bool yn): visited{0}, stop{yn}, time{FLT_MAX}, prev{0} {}
    };
    std::map<uint, DijkstraNode> DijkstraNodes;

    auto FastestAccessedUnvisitedNode = [&DijkstraNodes](void) {
        float best_time{FLT_MAX};
        uint  ans{0};
        for(auto& [id, node] : DijkstraNodes) {
            if(not node.visited and node.time < best_time) {
                best_time = node.time;
                ans = id;
            }
        }
        return ans;
    };
//////////////////1 INIT STRUCTURES ---- TODO!
    for(auto& link : links) {
        auto [id1, id2] = link.getZones();
        DijkstraNodes.emplace(id1, DijkstraNode(zones.at(id1).type == Zone::ProductionZone));
        DijkstraNodes.emplace(id2, DijkstraNode(zones.at(id2).type == Zone::ProductionZone));
    }
    if(DijkstraNodes.count(z1.id)) {
        DijkstraNodes.at(z1.id).stop = 0;
        DijkstraNodes.at(z1.id).time = 0;
    } else return ; // avoid problems with isolated zones


///////////////2 Find Path
    for(uint id{0}; (id = FastestAccessedUnvisitedNode()); ) {
        // Dijkstra worked, shortest path found; reconstruct path
        if(id == target_id or zones.at(id).type == target_type) {
            for( ; id; (id = DijkstraNodes.at(id).prev)) {
                connection.push_back(id);
            }
            return ;
        }
        // Keep looking
        DijkstraNodes.at(id).visited = true;
        if(not DijkstraNodes.at(id).stop) { // condition for passing through zone, call it CanCross/Traverse()?
            for(auto& link : links) {
                uint neighbour = (id == link.id1) ? link.id2 : (id == link.id2) ? link.id1 : 0;
                // DijkstraNode& neighbour = DijkstraNodes.at(tmp);
                if(neighbour && not DijkstraNodes.at(neighbour).visited) {
                    double time = DijkstraNodes.at(id).time + link.time;
                    if(time < DijkstraNodes.at(neighbour).time) {
                        DijkstraNodes.at(neighbour).time = time;
                        DijkstraNodes.at(neighbour).prev = id;
                    }
                }
            }
        }

    }
}


// PBM : CRASH map access if two separate graphs present

bool Archipelago::ShortestPath(Coord2D position, EditState state) {
    if(state != INIT && not shortest_path) return 0;

    switch(state) {
      case INIT:
        for(auto& link : links) {
            link.marked = false;
        }
        connection.clear();
        selected_zone = IdentifyZoneFromXY(position);
        if(selected_zone) {
            shortest_path = 1;
            connection.clear();
        }
      break;
      case UPDATE:
     break;
      case END:
        uint tmp = IdentifyZoneFromXY(position);
        if(tmp && tmp != selected_zone) Dijkstra(zones.at(selected_zone), tmp);
        for(int i{1}; i < connection.size(); ++i) {
            for(auto& link : links) {
                if((link.id1 == connection[i-1] and link.id2 == connection[i]) or (link.id2 == connection[i-1] and link.id1 == connection[i])) {
                    link.marked = true;
                }
            }
        }
        selected_zone = 0;
        shortest_path = 0;
        queue_draw();
      break;
    }
    return 0;
}