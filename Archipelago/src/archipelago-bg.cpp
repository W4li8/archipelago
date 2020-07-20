#include <iostream>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturezoom.h>
// #include "zone.hpp"
#include <memory.h>
#include <sstream>
#include <cmath>
#include <fstream>
#include <vector>
#include "archipelago-bg_copy.hpp"
#include "graphics.hpp"
#include "geometry.hpp"
// bool operator==(const Link::Id& lhs, const Link::Id& rhs) {
//     return (lhs.first == rhs.first and lhs.second == rhs.second)
// 		or (lhs.first == rhs.second and lhs.second == rhs.first);
// }
using uint = unsigned int;

Archipelago::Archipelago(void)
: Ox{0}, Oy{0}, S{1}, Rz{0}, Tx{0}, Ty{0}
, zone_edit{0}, link_edit{0}
, selectedzone{0}, variablezone{Zone()} {

    OpenFile("test2.txt");
    std::cout << "here\n";
    SaveFile("test2s.txt");
    std::cout << "here\n";
    ClearCity();
    OpenFile("test2s.txt");
    std::cout << "here\n";
    SaveFile("test2s.txt");
    ClearCity();
    OpenFile("test2s.txt");
}

bool Archipelago::OpenFile(std::string filename) {
    std::ifstream file{filename};
    if(!file.is_open()) return 0;

    auto ParseLineFromFile = [&file](auto&... args) {
        std::string line; getline(file, line);
        line.erase(std::find(line.begin(), line.end(), '#'), line.end()); // strip comment

        std::istringstream iss{line};
        return (iss >> ... >> args) && !iss.fail(); // parse line for args
    };

    for(auto& [type, zonelist] : Zone::Types) {
        while(!file.eof()) {
            uint zones2read{0};
            if(ParseLineFromFile(zones2read)) {
                uint id, nb_people;
                double x, y;
                while(!file.eof() && zones2read) {
                    if(ParseLineFromFile(id, x, y, nb_people)) {
                        CreateZone(type, {x, y}, nb_people, id);
                        zones2read -= 1;
                    }
                }
                break;
            }
        }
    }
    while(!file.eof()) {
        uint links2read{0};
        if(ParseLineFromFile(links2read)) {
            uint id1, id2;
            while(!file.eof() && links2read) {
                if(ParseLineFromFile(id1, id2)) {
                    ConnectZones(id1, id2);
                    links2read -= 1;
                }
            }
            break;
        }
    }
    file.close();
    return 1;
}

bool Archipelago::SaveFile(std::string filename) {
    std::ofstream file{filename};
    if(!file.is_open()) return 0;

    time_t clock = travel_time(0);
    file <<"# "<< ctime(&clock);

    for(auto& [type, zonelist] : Zone::Types) {
        file <<"\n# "<< type <<"s\n"<< zonelist.size() <<'\n';
        for(auto id : zonelist) {
            file <<'\t'<< zones.at(id) <<'\n';
        }
    }
    file <<"\n# Links\n" << links.size() <<'\n';
    for(auto& [idx, link] : links) {
        file <<'\t'<< idx[A] <<' '<< idx[B] <<'\n';
    }
    file.close();
    return 1;
}

bool Archipelago::SpacePermitsZone(Coord2D center, double radius, uint ignore) {

    for(auto& [id, zone] : zones) {
        if(id != ignore && DistancePoint2Point(zone.getCenter(), center) < (zone.getRadius() + radius)) {
            //std::cout <<"ERROR - Zones "<< key <<" and "<< center.x<<' '<<center.y <<" id "<< id <<" overlap.\n";
            return false;
        }
    }
    for(auto& [idx, link] : links) {
        if(not SpacePermitsLink(idx[A], idx[B])) {
            return false;
        }
    }
    return true;
}

bool Archipelago::SpacePermitsZone(const Zone& z0) {
    // Coord2D c{z0.getCenter()};
    // double  r{z0.getRadius()};
    return SpacePermitsZone(z0.getCenter(), z0.getRadius(), z0.id);
}

void Archipelago::CreateZone(Zone::Type zonetype, Coord2D pos, uint nb_people, uint id) {
    static uint max_id{0};
    //TODO: params validation check
    if(zonetype != Zone::NONE && SpacePermitsZone(pos, sqrt(nb_people))) {
        id ? max_id = MAX(id, max_id) : id = ++max_id;
        // auto new_zone = std::make_shared<Zone>(id, zonetype, pos, nb_people);

        zones.try_emplace(id, id, zonetype, pos, nb_people);

        queue_draw();
    }
}



void Archipelago::DestroyZone(ZoneId id) {

    for(auto& [idx, link] : links) {
        if(link.Connects(id)) {
            DisconnectZones(idx[A], idx[B]);
        }
    }
    zones.erase(id);
    queue_draw();
}


bool Archipelago::SpacePermitsLink(ZoneId id1, ZoneId id2) {

    for(auto& [id, zone] : zones) {
        if(id != id1 && id != id2 && DistancePoint2Segment(zone.getCenter(), zones.at(id1).getCenter(), zones.at(id2).getCenter()) < zone.getRadius()) {
            // std::cerr <<"ERROR - Link between zones "<< id1 <<"("<<citymap.at(id1)->getCenter().x<<','<<citymap.at(id1)->getCenter().y<<")"<<" and "<< id2<<"("<<citymap.at(id2)->getCenter().x<<','<<citymap.at(id2)->getCenter().y<<")"
            //           <<" passes through zone "<< zone->id<<"("<<zone->getCenter().x<<','<<zone->getCenter().y<<") dist: " <<DistancePoint2Segment(zone->getCenter(), citymap.at(id1)->getCenter(), citymap.at(id2)->getCenter())<<" vs "<<zone->getRadius()<<".\n";
            return false;
        }
    }
    return true;
}
bool Archipelago::LinkAllowed(ZoneId id1, ZoneId id2) {
    return zones.count(id1) && zones.count(id2)
        && (not zones.at(id1).IsOftenCongested()) and (not zones.at(id2).IsOftenCongested());
}


void Archipelago::ConnectZones(ZoneId id1, ZoneId id2) {
    // if(id1 > id2) std::swap(id1, id2);

    if(LinkAllowed(id1, id2) and SpacePermitsLink(id1, id2)) {
        // float speed{(zones.at(id1).HasSpeedLimit() or zones.at(id2).HasSpeedLimit()) ? S_SPEED : F_SPEED};
        // float distance{zones.at(id1).Distance(zones.at(id2))};

        // zones.at(id1).neighbours.push_back(id2);
        // zones.at(id2).neighbours.push_back(id1);
        zones.at(id1).AddNeighbour(id2);
        zones.at(id2).AddNeighbour(id1);

        // links.emplace(Link::Id(id1, id2), Link({id1, id2}, speed, distance));
        // links.try_emplace(Link::Id(id1, id2), Link::Id(id1, id2), speed, distance);
        links.try_emplace(Link::Id(id1, id2), zones.at(id1), zones.at(id2));
        // links.at({id1, id2});
        queue_draw();
    }
}

void Archipelago::DisconnectZones(ZoneId id1, ZoneId id2) {

    // zones.at(id1).neighbours.erase(std::find(zones.at(id1).neighbours.begin(), zones.at(id1).neighbours.end(), id2));
    // zones.at(id2).neighbours.erase(std::find(zones.at(id2).neighbours.begin(), zones.at(id2).neighbours.end(), id1));
    zones.at(id1).RemoveNeighbour(id2);
    zones.at(id2).RemoveNeighbour(id1);

    links.erase({id1, id2});
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

ZoneId Archipelago::IdentifyZone(Coord2D xy) {
    for(auto& [id, zone] : zones) {
        if(DistancePoint2Point(xy, zone.getCenter()) < zone.getRadius()) {
            return id;
        }
    }
    return 0;
}

Link::Id Archipelago::IdentifyLink(Coord2D xy) {
    for(auto& [idx, link] : links) {
        if(DistancePoint2Segment(xy, link[A].getCenter(), link[B].getCenter()) < EPS) {
            return idx;
        }
    }
    return {0, 0};
}


void Archipelago::AddZone(Zone::Type type, Coord2D xy, uint capacity, uint id) {
    CreateZone(type, xy, capacity, id);
}
void Archipelago::ModifyZone(Coord2D xy, EditState state) {
    static bool resize{0}, move{0};
    static Vector2D offset; // visual selection gap

    if(state != INIT && not zone_edit) return ;

    switch(state) {
      case INIT:
        for(auto& [id, zone] : zones) {
            float distance = zone.Distance(xy);
            if(abs(distance) < EPS) {
                resize = true;
                selectedzone = id;
                variablezone.setRadius(zone.getRadius());
                zone_edit = true;
                break;
            } else if(distance < 0) {
                move = true;
                selectedzone = id;
                variablezone.setCenter(zone.getCenter());
                offset = xy - zone.getCenter();
                zone_edit = true;
                break;
            }
        }
        return ;
      case UPDATE:
        if(resize) {
            zones.at(selectedzone).setRadius(CLAMP(zones.at(selectedzone).DistanceFromCenter(xy), 10, 300));
            edit_text = "Resizing zone "+ str(selectedzone) +" to fit "+ str(zones.at(selectedzone).getCapacity()) +" people ";
        }
        if(move) {
            zones.at(selectedzone).setCenter(xy - offset);
            edit_text = "Moving zone "+ str(selectedzone) +" \nto "+ zones.at(selectedzone).getCenter().getInfoString();
        }
        break;
      case END:
        if(not SpacePermitsZone(zones.at(selectedzone))) {
            if(resize) {
                zones.at(selectedzone).setRadius(variablezone.getRadius());
            }
            if(move) {
                zones.at(selectedzone).setCenter(variablezone.getCenter());
            }
        }
        resize = 0;
        move = 0;
        selectedzone = 0;
        zone_edit = 0;
        break;
    }
    queue_draw();
}
void Archipelago::RemoveZone(Coord2D xy) {
    auto id = IdentifyZone(xy);
    if(id) {
        DestroyZone(id);
    }
}


void Archipelago::AddLink(Coord2D xy, EditState state) {
    if(state != INIT && not link_edit) return ;

    ZoneId id = IdentifyZone(xy);
    switch(state) {
      case INIT:
        if(id) {
            selectedzone = id;
            variablezone.setCenter(zones.at(selectedzone).getCenter());
            link_edit = true;
        }
        return ;
      case UPDATE:
        variablezone.setCenter(xy);
        edit_text = "Connecting zone "+ str(selectedzone) +" to "+ (id ? str(id) : "??");
        break;
      case END:
        if(id) {
            ConnectZones(selectedzone, id);
        }
        selectedzone = 0;
        link_edit = false;
        break;
    }
    queue_draw();
}
void Archipelago::RemoveLink(Coord2D xy) {
    if(!IdentifyZone(xy)) {
        Link::Id idx = IdentifyLink(xy);
        if(idx) {
            DisconnectZones(idx[A], idx[B]);
        }
    }
}

bool Archipelago::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    UpdateViewModifiers();

    // cr->save();
    // Fill background (default: black border, white background)
    DrawQuadrilateral(cr, {{0, 0}, {width, 0}, {width, height}, {0, height}});
    // Setup drawing for Archipelago coordinates
    cr->translate(Ox + Tx, Oy + Ty);
    cr->rotate(Rz);
    cr->scale(S, -S);

    for(auto& [idx, link] : links) {
        DrawLink(cr, link);
    }
    if(link_edit) {
        DrawSegment(cr, {zones.at(selectedzone).getCenter(), variablezone.getCenter()});
    }

    for(auto& [id, zone] : zones) {
        DrawZone(cr, zone);
    }
    if(zone_edit) {
        Coord2D c{zones.at(selectedzone).getCenter()}; double r{zones.at(selectedzone).getRadius()};
        if(r != variablezone.getRadius() && c == variablezone.getCenter()) {
            DrawCircle(cr, {c, r}, black, {white, 0.0});
        } else if(r == variablezone.getRadius() && c != variablezone.getCenter()) {
            const float r015 = 0.15*r;
            DrawSegment(cr, {{c + Coord2D(-r015, -r015)}, {c + Coord2D(r015, r015)}});
            DrawSegment(cr, {{c + Coord2D(-r015, r015)}, {c + Coord2D(r015, -r015)}});
        }
    }
    // cr->restore();
    return 1; // return 1 stops event propagation
}

void Archipelago::DrawLink(const Cairo::RefPtr<Cairo::Context>& cr, const Link& link) {

    // cr->save();
    cr->set_line_width(link.speed);
    DrawSegment(cr, {link[A].getCenter(), link[B].getCenter()},
                    (link.speed == F_SPEED) ? (link.marked ? red : green)
                                            : (link.marked ? orange : black));
    cr->set_line_width(1);
    // cr->restore();
}

void Archipelago::DrawZone(const Cairo::RefPtr<Cairo::Context>& cr, const Zone& zone) {
    Coord2D c{zone.getCenter()}; double r{zone.getRadius()};

    // cr->save();
    switch(zone.type) {
      case Zone::PRODUCTION: {
        float r012 = 0.12*r; float r070 = 0.70*r;
        DrawCircle(cr, {c, r}, black, red);

        DrawQuadrilateral(cr, {{c.x - r070, c.y - r012}, {c.x + r070, c.y - r012},
                               {c.x + r070, c.y + r012}, {c.x - r070, c.y + r012}}, white);
      } break;
      case Zone::RESIDENTIAL: {
        float r038 = 0.5*0.75*r; float r065 = 0.5*sqrt(3)*0.75*r; float r075 = 0.75*r;
        float r030 = 0.5*0.60*r; float r052 = 0.5*sqrt(3)*0.60*r; float r060 = 0.60*r;
        DrawCircle(cr, {c, r}, white);
        DrawCircle(cr, {c, r}, blue, {0,0,255,0.5});

        DrawTriangle(cr, {{c.x, c.y - r075}, {c.x - r065, c.y + r038}, {c.x + r065, c.y + r038}}, white);
        DrawTriangle(cr, {{c.x, c.y + r075}, {c.x - r065, c.y - r038}, {c.x + r065, c.y - r038}}, white);
        DrawTriangle(cr, {{c.x, c.y - r060}, {c.x - r052, c.y + r030}, {c.x + r052, c.y + r030}}, pink, pink);
        DrawTriangle(cr, {{c.x, c.y + r060}, {c.x - r052, c.y - r030}, {c.x + r052, c.y - r030}}, pink, pink);
      } break;
      case Zone::TRANSPORT: {
        float r071 = M_SQRT1_2*r;
        DrawCircle(cr, {c, r}, white);
        cr->set_line_width(4);
        DrawCircle(cr, {c, r}, lgreen, {lgreen,0.2});
        cr->set_line_width(1);

        DrawSegment(cr, {{c.x + r, c.y}, {c.x - r, c.y}}, lgreen);
        DrawSegment(cr, {{c.x + r071, c.y + r071}, {c.x - r071, c.y - r071}}, lgreen);
        DrawSegment(cr, {{c.x, c.y + r}, {c.x, c.y - r}}, lgreen);
        DrawSegment(cr, {{c.x - r071, c.y + r071}, {c.x + r071, c.y - r071}}, lgreen);
      } break;
      default:;
    }
    // cr->restore();
}

void Archipelago::ClearCity(void) {
    links.clear();
    zones.clear();
    ResetViewModifiers();
}





std::string Archipelago::InfoFromCoordinates(Coord2D xy) {

    ZoneId id = IdentifyZone(xy);
    if(id) return zones.at(id).getInfoString();

    Link::Id idx = IdentifyLink(xy);
    if(idx) return links.at(idx).getInfoString();

    return "";
}

Coord2D Archipelago::Pointer2ArchipelagoXY(Coord2D mouse) {
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
    // for(auto [zonetype, ids] : zones_by_type) {
    //     uint tmp{0};
    //     for(auto id : ids) {
    //         tmp += citymap.at(id)->getCapacity();
    //     }
    //     switch(zonetype) {
    //       case Zone::ProductionZone:
    //         result += tmp;
    //         normalize += tmp;
    //         break;
    //       case Zone::ResidentialArea:
    //         result += tmp;
    //         normalize += tmp;
    //         break;
    //       case Zone::TransportHub:
    //         result -= tmp;
    //         normalize += tmp;
    //         break;
    //         default:;
    //     }
    // }
    return double(result)/double(normalize);
}

double Archipelago::ComputeCI(void) {
    double result{0};
    // for(auto link : linkss) {
    //     result += link.distance*MIN(link[A]->getCapacity(), link[B]->getCapacity())*link.speed;
    // }
    return result;
}

double Archipelago::ComputeMTA(void) {
    // double result{0};
    return 0;
    // for(auto id : zones_by_type.at(Zone::ResidentialArea)) {
    //     std::cout<<"LOOP-2\n";
    //     for(auto zonetype : Zone::Types) {
    //         std::cout<<"LOOP-1\n";
    //         if(zonetype != Zone::ResidentialArea) {
    //             std::cout<<"loop id! "<<id<<'\n';
    //             Dijkstra(*citymap.at(id), 0, zonetype);
    //             // std::cout <<'\n';
    //             result += total_time;
    //         }
    //         std::cout<<"LOOP+1\n";
    //     }
    //     std::cout<<"LOOP+2\n";
    // }
    // return result/zones_by_type.at(Zone::ResidentialArea).size();
}


std::vector<Link::Id> Archipelago::Dijkstra(ZoneId zi, uint target_id, Zone::Type target_type) {

    struct DijkstraNode {
        bool  visited; // has it the zone been visited by the algorithm
        bool  stop;    // should outgoing connections be considered
        float travel_time;    // shortest access travel_time
        uint  prev;    // id of the node that provides best access travel_time
        DijkstraNode(bool yn): visited{0}, stop{yn}, travel_time{FLT_MAX}, prev{0} {}
    };
    std::map< uint, DijkstraNode > DijkstraGraph;
    // Trick with auto doesn't work for recursive lambdas
    std::function< void(ZoneId) > ZoneAccessGraph = [this, &ZoneAccessGraph, &DijkstraGraph](ZoneId id) {
        DijkstraGraph.try_emplace(id, !zones.at(id).CanTraverse());
        for(auto zone : zones.at(id).getNeighbours()) {
            if(!DijkstraGraph.count(zone)) {
                ZoneAccessGraph(zone);
            }
        }
    };
    // Utility function to find out which node to visit next
    auto FastestAccessUnvisitedNode = [&DijkstraGraph](void) {
        uint ans{0}; float best_time{FLT_MAX};
        for(auto& [id, node] : DijkstraGraph) {
            if(not node.visited and node.travel_time < best_time) {
                best_time = node.travel_time;
                ans = id;
            }
        }
        return ans;
    };
    // Init graph of interest and starting node
    ZoneAccessGraph(zi);
    DijkstraGraph.at(zi).stop = 0;
    DijkstraGraph.at(zi).travel_time = 0;
    // Look for shortest path
    for(uint id{0}; (id = FastestAccessUnvisitedNode()); ) {
        DijkstraNode& self = DijkstraGraph.at(id);
        self.visited = true;
        // Solution exists
        if(id == target_id){//or citymap.at(id)->type == target_type) { END CONDITION
            std::vector< Link::Id > shortest_path;
            for( ; id != zi; (id = DijkstraGraph.at(id).prev)) {
                uint zone = DijkstraGraph.at(id).prev;
                // Link::Id tmp = (id < zone) ? Link::Id(id, zone) : Link::Id(zone, id);
                shortest_path.push_back(Link::Id(id, zone));
            }
            return shortest_path;
        }
        if(self.stop) continue;
        // Evaluate access to neighbours
        for(auto zone : zones.at(id).getNeighbours()) {
            DijkstraNode& other = DijkstraGraph.at(zone);
            if(not other.visited) {
                // Link::Id tmp{zone, id};
                float travel_time = self.travel_time + links.at(Link::Id(id, zone)).travel_time;
                if(travel_time < other.travel_time) {
                    other.travel_time = travel_time;
                    other.prev = id;
                }
            }
        }
    }
    return {};
}
            // uint neighbour = (id == link[A]->id) ? link[B]->id : (id == link[B]->id) ? link[A]->id : 0;
            // DijkstraNode& neighbour = DijkstraGraph.at(tmp);

void Archipelago::ShortestPath(Coord2D position, EditState state) {
    static ZoneId z1{0}, z2{0};

    switch(state) {
      case INIT:
        for(auto& [idx, link] : links) {
            link.marked = false;
        }
        z1 = IdentifyZone(position);
        break;
      case UPDATE:
        break;
      case END:
        if(z1 && (z2 = IdentifyZone(position)) && z1 != z2)  {
            for(auto& idx : Dijkstra(z1, z2)) {
                links.at(idx).marked = true;
            }
        }
        z1 = z2 = 0;
        queue_draw();
        break;
    }
}