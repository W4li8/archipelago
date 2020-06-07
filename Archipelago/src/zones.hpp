using uint = unsigned int;
#ifndef COORD
#define COORD
struct Coord {
	double x, y;

	Coord operator+(const Coord& other) {
		return {x + other.x, y + other.y};
    }

	Coord operator*(double nb) {
        return {x*nb, y*nb};
    }
	// Coord operator=(const Coord& other) {
	// 	return other;
	// }
};
#endif

template<class T>
using VectorOfPointers = std::vector< std::shared_ptr<T> >;


#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

enum ZoneType { ResidentialArea, TransportHub, ProductionZone, NbZoneTypes };

class Zone {

  public: // constructors etc.
    Zone(uint id, ZoneType type, Coord position, uint nb_people);
    // Zone(char type, std::istringstream line);
   ~Zone(void) = default;

  public:
    const uint id;        // unique identifier
    const ZoneType type;  // zone classifier

    void AddLink(std::shared_ptr<Zone>& newlink);
	std::string Print(void) {
		return std::to_string(id) +'\t'
			 + std::to_string(position.x) +'\t'+ std::to_string(position.y) +'\t'
			 + std::to_string(nb_people) +'\n';
	}
	double getRadius(void) const { return sqrt(nb_people); }
	Coord  getCenter(void) const { return position; }
	uint   getNbLinks(void) const { return links.size(); }

  private:
    Coord position;  // map coordinates x, y
    uint  nb_people; // number of people, determines the size of the zone

    const uint max_links; // allowed number of links to other zones
    VectorOfPointers<Zone> links; // pointers to the connected zones

};

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

