#include "zones.hpp"
#include <cmath>
#include <iostream>

Zone::Zone(uint id, ZoneType type, Coord position, uint nb_people)
: id{id}, type{type}, position{position}, nb_people{nb_people},
  max_links{uint(type == ResidentialArea ? 3 : INT8_MAX)} {} // a ameliorer


void Zone::AddLink(std::shared_ptr<Zone>& newlink) {
	if(links.size() < max_links && !std::count(links.begin(), links.end(), newlink)) {
		links.push_back(newlink);
	} else {
		std::cout <<"ERROR - Can't link zones "<< id <<" and "<< newlink->id
				  <<", connection exists or exceeds max_links.\n";
	}
}

// line equation between two points: y-y1 = (y2-y1)/(x2-x1) * (x-x1)
double DistancePoint2Point(Coord P, Coord Q) {
	double dx{P.x - Q.x}, dy{P.y - Q.y};
	return sqrt(dx*dx + dy*dy);
}

double DistancePoint2Line(Coord P, Coord A, Coord B) {
	double a{A.y - B.y}, b{B.x - A.x}, c{A.x*B.y - B.x*A.y}; // eq. ax + by + c = 0
	return abs(a*P.x + b*P.y + c)/sqrt(a*a + b*b);
}


