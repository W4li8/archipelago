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

void Archipelago::OpenFile(void) {

    std::ifstream file{"debug/"+ name +".txt"};
    std::string line;
    if(file.is_open()) {
        int counter = 0, status = 0;
        while(!file.eof()) {
            std::cout <<"reading number\n";
            getline(file,line);
            line.erase(std::find(line.begin(), line.end(), '#'), line.end());
            std::cout << line << '\n';
            std::istringstream iss{line};
            if(iss >> counter) {
                uint id, nb_people; double x, y;
                while(status < NbZoneTypes and counter > 0) {
                    std::cout <<"reading zone\n";
                    getline(file,line);
                    line.erase(std::find(line.begin(), line.end(), '#'), line.end());
                    std::cout << line << '\n';
                    std::istringstream iss{line};
                    if(iss >> id >> x >> y >> nb_people) {
                        // zones.push_back(Zone(id, ZoneType(status), {x, y}, nb_people));
                        zones.emplace_back(new Zone(id, ZoneType(status), {x, y}, nb_people));
                        nb_zones[status] += 1;
                        counter -= 1;
                    }
                }
                while(status == NbZoneTypes and counter > 0) {
                    std::cout <<"reading link\n";
                    uint id1, id2;
                    getline(file,line);
                    line.erase(std::find(line.begin(), line.end(), '#'), line.end());
                    std::cout << line << '\n';
                    std::istringstream iss{line};
                    if(iss >> id1 >> id2)
                    for(size_t i{0}, j{0}; i < NbZoneTypes; ++i) {
                        if(zones[i]->id == id1 or zones[i]->id == id2) {
                            if(j != i) {
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
    file <<"# last edit: "<<"xx.xx.xxxx at xx:xx:xx"<<"\n\n";
    file << std::to_string(nb_zones[0]) +" # nbResidentialAreas\n";
    for(int i{0}; i < nb_zones[0]; ++i) {
        file << zones[i]->Display();
    }file << '\n';
    file << std::to_string(nb_zones[1]) +" nbTransportHubs\n";
    for(int i{nb_zones[0]}; i < nb_zones[1] + nb_zones[0]; ++i) {
        file << zones[i]->Display();
    }file << '\n';
    file << std::to_string(nb_zones[2]) +" nbProductionZones\n";
    for(int i{nb_zones[1] + nb_zones[0]}; i < nb_zones[2] + nb_zones[1] + nb_zones[0]; ++i) { //make sum until index function
        file << zones[i]->Display();
    }file << '\n';

}