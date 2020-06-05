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
    void Edit(void); //or design

  private: // private fields
    std::string name; // city name, also used as file name

    // int nb_zones;     // number of zones/mini-islands ? can use zones.size as id
    std::vector< std::shared_ptr<Zone> > zones; //

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

                while(status < 3 and counter) {
                    std::cout <<"reading zone\n";
                    uint id, nb_people; double x, y;
                    getline(file,line);
                    line.erase(std::find(line.begin(), line.end(), '#'), line.end());
                    std::cout << line << '\n';
                    std::istringstream iss{line};
                    if(iss >> id >> x >> y >> nb_people) {
                        zones.emplace_back(new Zone(id, types[status], {x, y}, nb_people));
                        counter -= 1;
                    }
                }
                while(status == 3 and counter) {
                     std::cout <<"reading link\n";
                    uint id1, id2;
                    getline(file,line);
                    line.erase(std::find(line.begin(), line.end(), '#'), line.end());
                    std::cout << line << '\n';
                    std::istringstream iss{line};
                    if(iss >> id1 >> id2) {
                        std::shared_ptr<Zone> zone1, zone2;
                        for(auto zone : zones) {
                            if(zone->id == id1) {
                                zone1 = zone;
                            }
                            if(zone->id == id2) {
                                zone2 = zone;
                            }
                        }
                        if(!std::count(zone1->links.begin(), zone1->links.end(), zone2)) {
                            zone1->links.push_back(zone2);
                            zone2->links.push_back(zone1);
                        }
                        counter -= 1;
                    }
                }
                status += 1;
            }
        }
        file.close();
    }
}