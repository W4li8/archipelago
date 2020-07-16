#ifndef ZONES
#define ZONES

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#include "geometry.hpp"
#include <iostream>
#include <unordered_map>

using uint = unsigned int;

struct Link { uint id1, id2; };

class Zone {

  public: // constructors etc.
    enum Type { NONE, ResidentialArea, TransportHub, ProductionZone, NbZoneTypes};
    static constexpr std::array<Type, NbZoneTypes-1> Types {
      ResidentialArea,
      TransportHub,
      ProductionZone
    };

    Zone(uint id, Type type, Coord2D position, uint nb_people);
   ~Zone(void);

    static inline std::unordered_map<Type, uint> Counters = {
      {ResidentialArea, 0},
      {TransportHub,    0},
      {ProductionZone,  0}
    };

    std::string to_string(void) const;
    friend std::ostream& operator<<(std::ostream& os, const Zone& obj);
    // friend std::istream& operator>>(std::istream& os, Zone& obj);

  public:
    const uint id;       // unique identifier
    const Type type; // zone classifier

	  uint  getPopulation(void) const { return nb_people; }
	  double  getRadius(void) const { return sqrt(nb_people); }
    Coord2D getCenter(void) const { return position; }
    void setRadius(double nb) { nb_people = nb*nb; }
    void setCenter(Coord2D pos) { position = pos; }
    bool canPass(void) { return type != ProductionZone; }
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
  bool operator==(const Type& x) const { return x == type; }
  bool operator!=(const Type& x) const { return x != type; }
  Coord2D operator+(const Coord2D& v) const  { return {position.x + v.x, position.y + v.y}; }
  Coord2D operator-(const Coord2D& v) const { return {position.x - v.x, position.y - v.y}; }
  double operator*(const double& x) const { return sqrt(sqrt(nb_people)*x); }
  double operator/(const double& x) const { return sqrt(sqrt(nb_people)/x); }

  bool operator<(const Zone& other) const { return this->type < other.type; }

};




#endif//ZONES