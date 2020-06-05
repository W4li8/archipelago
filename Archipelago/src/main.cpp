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

class Archipelago {

  public: // constructors etc.
    Archipelago(void);
   ~Archipelago(void) = default;

  public: // public methods
    void OpenFile(void);
    void SaveFile(void);
    void Edit(void); //or design

  private: // private fields
    std::string name; // city name, also used as file name

    std::vector< std::vector<uint8_t> > adj_matrix; // adjacency matrix of a weighted directed graph

    // std::vector<Zone> zones[NbZoneTypes]; //

    uint8_t nb_zones[NbZoneTypes] = {0};     // number of zones/mini-islands ? can use zones.size as id
    std::vector< std::shared_ptr<Zone> > zones; //

    // std::vector<Zone> zones;
    // UserInterface gui;

//  private: // private methods


};



//  ==============================================================================  //

auto main(int argc, char *argv[]) -> int {
    Archipelago city;
}

//  ==============================================================================  //

#include <iostream>
Archipelago::Archipelago(void) {
    std::cout << "Select city: ";
    std::cin  >> name;

    OpenFile();
    SaveFile();
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
    if(file.is_open()) {
        int counter = 0, status = 0;
        while(!file.eof()) {
            if(ParseLineFromFile(file, counter)) {
                while(status < NbZoneTypes and counter > 0) {
                    uint id, nb_people; double x, y;
                    if(ParseLineFromFile(file, id, x, y, nb_people)) {  // and far from all zones
                        zones.emplace_back(new Zone(id, ZoneType(status), {x, y}, nb_people));
                        nb_zones[status] += 1;
                        counter -= 1;
                    }
                }
                while(status == NbZoneTypes and counter > 0) {
                    uint id1, id2;
                    if(ParseLineFromFile(file, id1, id2))
                    for(size_t i{0}, j{0}; i < NbZoneTypes; ++i) {
                        if(zones[i]->id == id1 or zones[i]->id == id2) {
                            if(j != i) { //TODO: and far from all zones
                                zones[i]->AddLink(zones[j]); // checks if already linked
                                zones[j]->AddLink(zones[i]); // redundant check
                                break;
                            } else {
                                j = i;
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
}
void Archipelago::SaveFile(void) {

    std::ofstream file{"debug/"+ name +"0.txt"};

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

    //print links

}