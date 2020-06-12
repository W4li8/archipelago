#ifndef ZONES
#define ZONES

#include <memory>
#include <vector>
#include <string>
#include <cmath>

using uint = unsigned int;
template<class T>
using VectorOfPointers = std::vector< std::shared_ptr<T> >;

enum ZoneType { ResidentialArea, TransportHub, ProductionZone, NbZoneTypes };

struct Coord {
    double x, y;

    Coord operator+(const Coord& other) {
        return {x + other.x, y + other.y};
    }

    Coord operator*(const double& nb) {
         return {x*nb, y*nb};
    }
    // Coord operator=(const Coord& other) {
    // 	return other;
    // }
};


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

double DistancePoint2Point(Coord P, Coord Q);
double DistancePoint2Line(Coord P, Coord A, Coord B);

#endif//ZONES