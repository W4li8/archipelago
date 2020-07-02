#ifndef ZONES
#define ZONES

#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include "geometry.hpp"
#include <iostream>
#include <map>

using uint = unsigned int;

struct Link { uint id1, id2; };
namespace zone
{
    enum ZoneType : char { ResidentialArea = 'R', TransportHub = 'T', ProductionZone = 'P', NbZoneTypes = 3};
    inline std::map<char, uint> nb_zones = {{'R', 0}, {'T', 0}, {'P', 0}};
    static uint nbProductionZones;
    static uint nbResidentialAreas;
    static uint nbTransportHubs;
}
class Zone {

  public: // constructors etc.
    Zone(uint id, zone::ZoneType type, Coord2D position, uint nb_people);
   ~Zone(void) = default;


    std::string to_string(void) const;
    friend std::ostream& operator<<(std::ostream& os, const Zone& obj);
    // friend std::istream& operator>>(std::istream& os, Zone& obj);

  public:
    const uint id;       // unique identifier
    const zone::ZoneType type; // zone classifier

	  double  getRadius(void) const { return sqrt(nb_people); }
    Coord2D getCenter(void) const { return position; }
    void setRadius(double nb) { nb_people = nb*nb; }
    void setCenter(Coord2D pos) { position = pos; }
	// uint   getNbLinks(void) const { return links.size(); }
  bool LinkAllowed(uint id) { return links.size() < max_links and find(links.begin(), links.end(), id) == links.end(); }
  void AddLink(uint id) {links.push_back(id);}
  void RemoveLink(uint id) {links.erase(find(links.begin(), links.end(), id));}
  private:
    Coord2D position; // map coordinates x, y
    uint  nb_people;  // number of people, determines the size of the zone

    const uint max_links; // allowed number of links to other zones
    std::vector<uint> links; // pointers to the connected zones
  public:
  bool operator==(const uint& x)     const { return x == id; }
  bool operator!=(const uint& x)     const { return x != id; }
  bool operator==(const zone::ZoneType& x) const { return x == type; }
  bool operator!=(const zone::ZoneType& x) const { return x != type; }
  Coord2D operator+(const Coord2D& v) const  { return {position.x + v.x, position.y + v.y}; }
  Coord2D operator-(const Coord2D& v) const { return {position.x - v.x, position.y - v.y}; }
  double operator*(const double& x) const { return sqrt(sqrt(nb_people)*x); }
  double operator/(const double& x) const { return sqrt(sqrt(nb_people)/x); }

  bool operator<(const Zone& other) const { return this->type < other.type; }

};




#endif//ZONES