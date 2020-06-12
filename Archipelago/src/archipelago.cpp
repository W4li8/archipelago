#include <gtkmm/drawingarea.h>
#include "zones.hpp"
#include <memory.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "archipelago.hpp"

void DrawLine(const Cairo::RefPtr<Cairo::Context>& cr, Coord A, Coord B) {
	cr->save();
    cr->set_source_rgb(1, 0,0 );
	cr->move_to(A.x, A.y);
	cr->line_to(B.x, B.y);
	cr->stroke();
    cr->restore();
}

void DrawCircle(const Cairo::RefPtr<Cairo::Context>& cr, Coord center, double radius) {
    cr->save();
    cr->set_source_rgb(0, 0, 0);
	cr->arc(center.x, center.y, radius, 0.0, 2*M_PI);
    cr->stroke();
    cr->restore();
}
void DrawRectangle(const Cairo::RefPtr<Cairo::Context>& cr, Coord cornerTL, double width, double height) {
	cr->save();
    cr->set_source_rgb(0, 0, 0);
    cr->rectangle(cornerTL.x, cornerTL.y, width, height);
    cr->set_source_rgb(1, 1, 1);    // partially translucent
    cr->fill_preserve();
    cr->restore();
    cr->stroke();
}


Archipelago::Archipelago(void) {
    // OpenFile();

}

template<class... T>
bool ParseLineFromFile(std::ifstream& file, T&... args) {

    std::string line;
    getline(file, line);
    line.erase(std::find(line.begin(), line.end(), '#'), line.end()); // strip comment

    std::istringstream stream{line}; // parse line for args
    return (stream >> ... >> args) && !stream.fail();
}

#include <iostream>
void Archipelago::OpenFile(std::string filename) {
    Reset();

    std::ifstream file{filename};
    if(!file.is_open()) return ;

    int counter = 0, status = 0;
    while(!file.eof()) {
        if(ParseLineFromFile(file, counter)) {
            while(status < NbZoneTypes and counter > 0) {
                uint id, nb_people; double x, y;
                if(ParseLineFromFile(file, id, x, y, nb_people)) {  // and far from all zones
                    if(SpacePermitsZone({id, ZoneType(status), {x, y}, nb_people})) {
                        Archipelago::zones.emplace_back(new Zone(id, ZoneType(status), {x, y}, nb_people));
                        nb_zones[status] += 1;
                    }
                    counter -= 1;
                }
            }
            while(status == NbZoneTypes and counter > 0) {
                uint id1, id2;
                if(ParseLineFromFile(file, id1, id2)) {
                    for(int i{0}, j{-1}; i < zones.size(); ++i) {
                        if(zones[i].id == id1 or zones[i].id == id2) {
                            if(j >= 0) {
                                if(SpacePermitsLink(zones[i], zones[j])) {
                                    links[i][j] = 1;
                                    links[j][i] = 1;
                                    // zones[i]->AddLink(zones[j]); // i -> j
                                    // zones[j]->AddLink(zones[i]); // j -> i
                                }
                                break;
                            } else {
                                j = i;
                            }
                        }
                    }
                }
                counter -= 1; // will ignore entry if an id is incorrect
            }
            status += 1;
        }
    }
    file.close();
}
#define watch(x) (#x) // return strin name
void Archipelago::SaveFile(std::string filename) {

    std::ofstream file{filename};
    if(!file.is_open()) return ;

    time_t clock = time(0);
    file <<"# "<< ctime(&clock) <<"\n";

    file <<"\n"<< std::to_string(nb_zones[ResidentialArea]) +" # nbResidentialAreas\n";
    for(int i{0}; i < nb_zones[0]; ++i) {
        file << Archipelago::zones[i]->Print();
    }

    file <<"\n"<< std::to_string(nb_zones[TransportHub]) +" # nbTransportHubs\n";
    for(int i{nb_zones[0]}; i < nb_zones[1] + nb_zones[0]; ++i) {
        file << Archipelago::zones[i]->Print();
    }

    file <<"\n"<< std::to_string(nb_zones[ProductionZone]) +" # nbProductionZones\n";
    for(int i{nb_zones[1] + nb_zones[0]}; i < nb_zones[2] + nb_zones[1] + nb_zones[0]; ++i) { //make sum until index function
        file << Archipelago::zones[i]->Print();
    }

    file <<"\n"<< std::to_string(0) +" # nbLinks ???\n";

    //TODO: print links

}

bool Archipelago::SpacePermitsZone(const Zone& z0) {

    for(auto& zone : Archipelago::zones) {
        if(zone->id != z0.id && DistancePoint2Point(zone->getCenter(), z0.getCenter()) < (zone->getRadius() + z0.getRadius())) {
            std::cout <<"ERROR - Zones "<< zone->id <<" and "<< z0.id <<" overlap.\n";
            return false;
        }
    }
    return true;
}

bool Archipelago::SpacePermitsLink(const Zone& z1, const Zone& z2) {

    for(auto& zone : Archipelago::zones) {
        if(zone->id != z1.id && zone->id != z2.id && DistancePoint2Line(zone->getCenter(), z1.getCenter(), z2.getCenter()) < zone->getRadius()) {
            std::cout <<"ERROR - Link between zones " << z1.id <<" and "<< z2.id
                      <<" passes through zone "<< zone->id <<".\n";
            return false;
        }
    }
    return true;
}




bool Archipelago::on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) {
    if(zones.empty()) return 1;
    for(auto& zone : zones) {
        Coord tmp{100,100},c{zone->getCenter()+tmp}; double r{zone->getRadius()};
        switch(zone->type) {
          case ResidentialArea: std::cout<<"R\n";
            DrawCircle(pencil, c, r);
            break;
          case TransportHub: std::cout<<"T\n";
            DrawLine(pencil, {c.x + r, c.y}, {c.x - r, c.y});
            DrawLine(pencil, {c.x + M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x - M_SQRT1_2*r, c.y - M_SQRT1_2*r});
            DrawLine(pencil, {c.x, c.y + r}, {c.x, c.y - r});
            DrawLine(pencil, {c.x - M_SQRT1_2*r, c.y + M_SQRT1_2*r}, {c.x + M_SQRT1_2*r, c.y - M_SQRT1_2*r});
            DrawCircle(pencil, c, r);
            break;
          case ProductionZone:std::cout<<"P\n";
            DrawCircle(pencil, c, r);
            pencil->set_source_rgb(1, 0.0, 0.0);    // partially translucent
            pencil->fill_preserve();
            DrawRectangle(pencil, {c.x - 0.7*r, c.y - 0.12*r}, 1.4*r, 0.24*r); // change sign after ok cordintes
            pencil->set_source_rgb(1, 1, 1);    // partially translucent
            pencil->fill();
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
}