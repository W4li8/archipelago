#include <gtkmm/drawingarea.h>
#include "zones.hpp"
#include <memory.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "archipelago.hpp"




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
void Archipelago::OpenFile(void) {
    std::cout << "Select city: ";
    std::cin  >> name;
    std::ifstream file{"debug/"+ name +".txt"};
    if(!file.is_open()) return ;

    // Archipelago::gui.setTitle("Archipelago - " + name);
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
                    for(int i{0}, j{-1}; i < Archipelago::zones.size(); ++i) {
                        if(Archipelago::zones[i]->id == id1 or Archipelago::zones[i]->id == id2) {
                            if(j >= 0) {
                                if(SpacePermitsLink(*Archipelago::zones[i], *Archipelago::zones[j])) {
                                    Archipelago::zones[i]->AddLink(Archipelago::zones[j]); // i -> j
                                    Archipelago::zones[j]->AddLink(Archipelago::zones[i]); // j -> i
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
void Archipelago::SaveFile(void) {

    std::ofstream file{"debug/"+ name +"0.txt"};
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



constexpr double sqrt2 = 1.4142135624;
void Archipelago::Draw(void) {
        // gui.DrawLine({-2,2},{-1,3});
    for(auto& zone : Archipelago::zones) {
        switch(zone->type) {
          case ResidentialArea:
            // gui.DrawCircle(zone->getCenter(), zone->getRadius());
            break;
          case TransportHub:
            // gui.DrawCircle(zone->getCenter(), zone->getRadius());
            // gui.DrawLine(zone->getCenter() + {0.5*sqrt2, 0.5*sqrt2}*zone->getRadius(), zone->getCenter() + {-0.5*sqrt2, -0.5*sqrt2}*zone->getRadius());
            // gui.DrawLine(zone->getCenter() + {1, 0}*zone->getRadius(), zone->getCenter() + {-1, 0}*zone->getRadius());
            // gui.DrawLine(zone->getCenter() + {0, 1}*zone->getRadius(), zone->getCenter() + {0, -1}*zone->getRadius());
            // gui.DrawLine(zone->getCenter() + {-0.5*sqrt2, 0.5*sqrt2}*zone->getRadius(), zone->getCenter() + {0.5*sqrt2, -0.5*sqrt2}*zone->getRadius());
            break;
          case ProductionZone:
            // gui.DrawCircle(zone->getCenter(), zone->getRadius());
            // gui.DrawRectangle(zone->getCenter() + ({0.75, 0.125}*(-0.5)*zone->getRadius()), zone->getCenter() + Coord((0.5)*zone->getRadius()*{0.75, 0.125}));
            break;
          default:;
            // no other
        }
    }
    // gtk_widget_queue_draw();
}
