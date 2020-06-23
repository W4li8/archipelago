#ifndef ARCHIPELAGO
#define ARCHIPELAGO

#include "zones.hpp"
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturezoom.h>
#include <string>
#include <vector>
#include <map>
class ArchipelagoUI;

using uint = unsigned int;
// struct vache;
template<class T>
using vector = std::vector<T>;
template<class T>
using matrix = std::vector< std::vector<T> >;

class Archipelago : public Gtk::DrawingArea {

  public: // constructors etc.
    Archipelago(void);
   ~Archipelago(void) = default;

  public:
    void OpenFile(std::string filename);
    void SaveFile(std::string filename);
    // void Edit(void); //or design

    // std::map<uint, uint> pair;
    std::map<uint, Zone> zones;
	std::vector<std::pair<uint, uint>> links;
    // vector<Site> ; // adj_matrix; // adjacency matrix of a weighted directed graph
    // matrix<int8_t> links; // adj_matrix; // adjacency matrix of a weighted directed graph
    // std::vector< std::shared_ptr<Zone> > zones; //
//   private:
    // std::string name; // city name, also used as file name


    uint nb_zones[ZoneType::NbZoneTypes];     // number of zones/mini-islands ? can use zones.size as id
    uint nb_links;     // number of zones/mini-islands ? can use zones.size as id

  private: // private methods
    bool SpacePermitsZone(Coord2D center, double radius);
    // bool SpacePermitsZone(const Zone& z0);
	void CreateZone(const Zone& z0);
	void DestroyZone(Zone& z0);

	bool LinkAllowed(uint id1, uint id2);
	// bool LinkAllowed(const Zone& z1, const Zone& z2);
    bool SpacePermitsLink(uint id1, uint id2);
    // bool SpacePermitsLink(const Zone& z1, const Zone& z2);
	void ConnectZones(Zone& z1, Zone& z2);
	void DisconnectZones(Zone& z1, Zone& z2);

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;

	void zoomfn(double delta);// override;
  	Glib::RefPtr<Gtk::GestureZoom> m_GestureZoom;

	std::pair<uint, uint> newlink;
    void Reset(void);double zoom, scale, xoffset, yoffset;
	public:void ResetView(void);
	// friend vache;
};

// struct vache {
//  bool operator()(const uint &lhs, const uint &rhs) const {
//     return zones.at(lhs) < zones.at(rhs);
// }
// };
#endif//ARCHIPELAGO

/*

for( auto const& [key, val] : symbolTable )
{
    std::cout << key         // string (key)
              << ':'
              << val        // string's value
              << std::endl ;
}

*/