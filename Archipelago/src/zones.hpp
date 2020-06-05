using uint = unsigned int;
struct Coord { double x, y; };

template<class T>
using SharedLinkedList = std::vector< std::shared_ptr<T> >;

#include <iostream>
#include <vector>
#include <memory>

enum ZoneType { ResidentialArea, TransportHub, ProductionZone, NbZoneTypes };

class Zone {

  public: // constructors etc.
    Zone(uint id, ZoneType type, Coord position, uint nb_people);
    // Zone(char type, std::istringstream line);
   ~Zone(void) = default;

  public:
    const uint id;       // unique identifier
    const ZoneType type; // zone classifier

    void AddLink(std::shared_ptr<Zone> newlink);
	std::string Display(void) {
		// std::cout << id <<' '<< position.x <<' '
        //           << position.y <<' '
        //           << nb_people <<'\n';
		return std::to_string(id) +' '+std::to_string(position.x) +' '+ std::to_string(position.y) +' '+ std::to_string(nb_people) +'\n';
	}


  private:
    Coord position;  // map coordinates x, y
    uint  nb_people; // number of people, determines the size of the zone

    const uint max_links; // allowed number of links to other zones
    std::vector< std::shared_ptr<Zone> > links; // pointers to the connected zones

};

Zone::Zone(uint id, ZoneType type, Coord position, uint nb_people)
: id{id}, type{type}, position{position}, nb_people{nb_people},
  max_links{uint(type == ResidentialArea ? 3 : INT8_MAX)} {} // a ameliorer


void Zone::AddLink(std::shared_ptr<Zone> newlink) {
	if(links.size() < max_links && !std::count(links.begin(), links.end(), newlink)) {
		links.push_back(newlink);
	}
}