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
    bool OpenFile(std::string filename);
    void SaveFile(std::string filename);
    // void Edit(void); //or design
	// void rotate(double angle_deg);
	double deg{0}, lastdeg{0};
	double tmpzoom{1}, tmprotate{0};
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
    bool SpacePermitsZone(Coord2D center, double radius, uint id = 0) const;
    bool SpacePermitsZone(const Zone& z0) const;
    // bool SpacePermitsZone(const Zone& z0);
	public: void CreateZone(const Zone& z0);
	void DestroyZone(Zone& z0);

	bool LinkAllowed(uint id1, uint id2);
	// bool LinkAllowed(const Zone& z1, const Zone& z2);
    bool SpacePermitsLink(uint id1, uint id2) const;
    // bool SpacePermitsLink(const Zone& z1, const Zone& z2);
	void ConnectZones(Zone& z1, Zone& z2);
	void DisconnectZones(Zone& z1, Zone& z2);

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;
    Cairo::Matrix view;
	public:
	void Zoom(double sign);
	void Rotate(double sign);
	void Swipe(double dx, double dy);



	bool moveFlag{0}, resizeFlag{0}, connectFlag{0}, removeFlag{0};
	void ResetFlags(void) {
		moveFlag=0; resizeFlag=0; connectFlag=0; removeFlag=0; tmpline=0;
	}
	void movemenu(void) { moveFlag = 1;}
	void resizemenu(void) { resizeFlag = 1;}
	void connectmenu(void) { connectFlag = 1;}
	void removemenu(void) { removeFlag = 1;}
	uint selected_zone;
	//--------------Zone DRAG-----------------//
	void DragStart(Coord2D pos);
	void DragUpdate(Coord2D pos);
	void DragEnd(Coord2D pos);
	Coord2D drag_select_offset;
	Coord2D origin; double nb;
	//-------------Zone RESIZE----------------//

	//-------------Zone CONNECT---------------//

	std::pair<uint, uint> newlink; // make tuple and include distance
    void Reset(void);
	double m_zoom, scale, Tx, Ty, Rz;

	public:void ResetView(void);
	void on_button_press_event2(int button, Coord2D pos) ;
	void on_button_release_event2(int button, Coord2D pos) ;
	// friend vache;
	void UpdateCoordinates(void);
    double width, height;
	Coord2D center;
	//for drag tmp line
	bool tmpline{0};
	Coord2D tmpcoord{0,0};

	public: Coord2D MouseXY_to_ArchipelagoXY(Coord2D mouse);
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