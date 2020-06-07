/**
 * @file    main.cpp
 * @brief	???
 *
 * @author	Filip Slezak
 * @version 04.06.2020
 **/

#include <string>
#include <vector> // maybe use a std::list (non sequential memory storage)
#include <fstream>
#include <sstream>
#include <memory>

#include <ctime>
#include "zones.hpp"
#include "gui.hpp"

class Archipelago {

  public: // constructors etc.
    Archipelago(int argc, char *argv[]);
    Archipelago(void);
   ~Archipelago(void) = default;

  public:
    void OpenFile(void);
    void SaveFile(void);
    void Edit(void); //or design

  private:
    std::string name; // city name, also used as file name

    std::vector< std::vector<uint8_t> > adj_matrix; // adjacency matrix of a weighted directed graph

    // std::vector<Zone> zones[NbZoneTypes]; //

    uint8_t nb_zones[NbZoneTypes] = {0};     // number of zones/mini-islands ? can use zones.size as id
    std::vector< std::shared_ptr<Zone> > zones; //

    // std::vector<Zone> zones;
    // UserInterface gui;
    UserInterface app;
    void Draw(void);
//  private: // private methods
    bool SpacePermits(const Zone& z0);
    bool SpacePermits(const Zone& z1, const Zone& z2);

};



//  ==============================================================================  //

auto main(int argc, char *argv[]) -> int {
    Archipelago city;// {argc, argv};
    Coord P{1,0}, A{0,1}, B{2,49};
    DistancePoint2Line(P,A,B);
}

//  ==============================================================================  //

#include <iostream>
Archipelago::Archipelago(void)
: app{"Archipelago"} {}

Archipelago::Archipelago(int argc, char *argv[]): app{"Archipel"} {
    std::cout << "Select city: ";
    std::cin  >> name;

    OpenFile();
    SaveFile();
    ;
}

template<class... T>
bool ParseLineFromFile(std::ifstream& file, T&... args) {

    std::string line;
    getline(file, line);
    line.erase(std::find(line.begin(), line.end(), '#'), line.end()); // strip comment

    std::istringstream stream{line}; // parse line for args
    return (stream >> ... >> args) && !stream.fail();
}

void Archipelago::OpenFile(void) {

    std::ifstream file{"debug/"+ name +".txt"};
    if(!file.is_open()) return ;

    int counter = 0, status = 0;
    while(!file.eof()) {
        if(ParseLineFromFile(file, counter)) {
            while(status < NbZoneTypes and counter > 0) {
                uint id, nb_people; double x, y;
                if(ParseLineFromFile(file, id, x, y, nb_people)) {  // and far from all zones
                    if(SpacePermits({id, ZoneType(status), {x, y}, nb_people})) {
                        zones.emplace_back(new Zone(id, ZoneType(status), {x, y}, nb_people));
                        nb_zones[status] += 1;
                    }
                    counter -= 1;
                }
            }
            while(status == NbZoneTypes and counter > 0) {
                uint id1, id2;
                if(ParseLineFromFile(file, id1, id2)) {
                    for(int i{0}, j{-1}; i < zones.size(); ++i) {
                        if(zones[i]->id == id1 or zones[i]->id == id2) {
                            if(j >= 0) {
                                if(SpacePermits(*zones[i], *zones[j])) {
                                    zones[i]->AddLink(zones[j]); // i -> j
                                    zones[j]->AddLink(zones[i]); // j -> i
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
    app.RenameWindow("Archipelago - " + name);
}
void Archipelago::SaveFile(void) {

    std::ofstream file{"debug/"+ name +"0.txt"};
    if(!file.is_open()) return ;

    time_t clock = time(0);
    file <<"# "<< ctime(&clock) <<"\n";

    file <<"\n"<< std::to_string(nb_zones[ResidentialArea]) +" # nbResidentialAreas\n";
    for(int i{0}; i < nb_zones[0]; ++i) {
        file << zones[i]->Print();
    }

    file <<"\n"<< std::to_string(nb_zones[TransportHub]) +" # nbTransportHubs\n";
    for(int i{nb_zones[0]}; i < nb_zones[1] + nb_zones[0]; ++i) {
        file << zones[i]->Print();
    }

    file <<"\n"<< std::to_string(nb_zones[ProductionZone]) +" # nbProductionZones\n";
    for(int i{nb_zones[1] + nb_zones[0]}; i < nb_zones[2] + nb_zones[1] + nb_zones[0]; ++i) { //make sum until index function
        file << zones[i]->Print();
    }

    file <<"\n"<< std::to_string(0) +" # nbLinks ???\n";

    //TODO: print links

}

bool Archipelago::SpacePermits(const Zone& z0) {

    for(auto& zone : zones) {
        if(zone->id != z0.id && DistancePoint2Point(zone->getCenter(), z0.getCenter()) < (zone->getRadius() + z0.getRadius())) {
            std::cout <<"ERROR - Zones "<< zone->id <<" and "<< z0.id <<" overlap.\n";
            return false;
        }
    }
    return true;
}

bool Archipelago::SpacePermits(const Zone& z1, const Zone& z2) {

    for(auto& zone : zones) {
        if(zone->id != z1.id && zone->id != z2.id && DistancePoint2Line(zone->getCenter(), z1.getCenter(), z2.getCenter()) < zone->getRadius()) {
            std::cout <<"ERROR - Link between zones " << z1.id <<" and "<< z2.id
                      <<" passes through zone "<< zone->id <<".\n"<<DistancePoint2Line(zone->getCenter(), z1.getCenter(), z2.getCenter())<<'\n'<<zone->getRadius()<<'\n';
            return false;
        }
    }
    return true;
}

constexpr double sqrt2 = 1.4142135624;
void Archipelago::Draw(void) {
    app.DrawLine({-2,2},{-1,3});
    for(auto& zone : zones) {
        switch(zone->type) {
          case ResidentialArea:
            // app.DrawCircle(zone->getCenter(), zone->getRadius());
            break;
          case TransportHub:
            // app.DrawCircle(zone->getCenter(), zone->getRadius());
            // app.DrawLine(zone->getCenter() + {0.5*sqrt2, 0.5*sqrt2}*zone->getRadius(), zone->getCenter() + {-0.5*sqrt2, -0.5*sqrt2}*zone->getRadius());
            // app.DrawLine(zone->getCenter() + {1, 0}*zone->getRadius(), zone->getCenter() + {-1, 0}*zone->getRadius());
            // app.DrawLine(zone->getCenter() + {0, 1}*zone->getRadius(), zone->getCenter() + {0, -1}*zone->getRadius());
            // app.DrawLine(zone->getCenter() + {-0.5*sqrt2, 0.5*sqrt2}*zone->getRadius(), zone->getCenter() + {0.5*sqrt2, -0.5*sqrt2}*zone->getRadius());
            break;
          case ProductionZone:
            // app.DrawCircle(zone->getCenter(), zone->getRadius());
            // app.DrawRectangle(zone->getCenter() + ({0.75, 0.125}*(-0.5)*zone->getRadius()), zone->getCenter() + Coord((0.5)*zone->getRadius()*{0.75, 0.125}));
            break;
          default:;
            // no other
        }
    }
}
