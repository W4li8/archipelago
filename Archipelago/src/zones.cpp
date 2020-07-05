#include "zones.hpp"
#include <cmath>
#include <iostream>


Zone::Zone(uint id, Type type, Coord2D position, uint nb_people)
: id{id}, type{type}, position{position}, nb_people{nb_people},
  max_links{uint(type == Type::ResidentialArea ? 3 : INT8_MAX)} {
    // Counters[type] += 1; std::cout<<"nbzones++\n";
} // a ameliorer

Zone::~Zone(void) {
    // Counters[type] -= 1; std::cout<<"nbzones--\n";
}

std::string Zone::to_string(void) const {
    return std::to_string(id) +' '+ position.to_string() +' '+ std::to_string(nb_people) +'\n';
}


std::ostream& operator<<(std::ostream& os, const Zone& obj) {
    // std::cout << obj.to_string();
    return os << obj.id <<' '<< obj.position.x <<' '<< obj.position.y <<' '<< obj.nb_people;
}

// std::map<char, uint> Zone::Counters = {{'R', ZoneType::ResidentialArea}, {'T', ZoneType::TransportHub}, {'P', ZoneType::ProductionZone}};

// Zone::Zone::Counters =