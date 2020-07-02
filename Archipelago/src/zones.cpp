#include "zones.hpp"
#include <cmath>
#include <iostream>


Zone::Zone(uint id, zone::ZoneType type, Coord2D position, uint nb_people)
: id{id}, type{type}, position{position}, nb_people{nb_people},
  max_links{uint(type == zone::ZoneType::ResidentialArea ? 3 : INT8_MAX)} {} // a ameliorer

std::string Zone::to_string(void) const {
    return std::to_string(id) +' '+ position.to_string() +' '+ std::to_string(nb_people) +'\n';
}


std::ostream& operator<<(std::ostream& os, const Zone& obj) {
    return os << obj.to_string();
    return os << obj.id <<' '<< obj.position.x <<' '<< obj.position.y <<' '<< obj.nb_people;
}

// std::map<char, uint> zone::nb_zones = {{'R', zone::ZoneType::ResidentialArea}, {'T', zone::ZoneType::TransportHub}, {'P', zone::ZoneType::ProductionZone}};

// Zone::nb_zones =