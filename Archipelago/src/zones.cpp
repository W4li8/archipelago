#include "zones.hpp"
#include <cmath>
#include <iostream>


Zone::Zone(uint id, ZoneType type, Coord2D position, uint nb_people)
: id{id}, type{type}, position{position}, nb_people{nb_people},
  max_links{uint(type == ZoneType::ResidentialArea ? 3 : INT8_MAX)} {} // a ameliorer

std::ostream& operator<<(std::ostream& os, const Zone& obj) {
    return os << obj.id <<' '<< obj.position.x <<' '<< obj.position.y <<' '<< obj.nb_people;
}





