#ifndef ARCHIPELAGO
#define ARCHIPELAGO

#include "zones.hpp"
#include <gtkmm/drawingarea.h>
#include <string>
#include <vector>
#include <tuple>
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
    std::string edit_text;
    std::string performance;
    // void Edit(void); //or design
	// void rotate(double angle_deg);
	// double deg{0}, lastdeg{0};
	// double tmpzoom{1}, tmprotate{0};
    // std::map<uint, uint> pair;
    std::map<uint, Zone> zones;
	std::map<Zone::Type, std::vector<uint>> zones_by_type;
	// std::vector<std::pair<uint, uint>> links;
	struct Link {
		uint id1, id2;
		float speed, distance, time;
		bool marked;
		Link(uint id1, uint id2, float v, float d)
		: id1{id1}, id2{id2}
		, speed{v}, distance{d}, time{distance/speed}
		, marked{0} {}
		// std::tuple<uint&, uint&> getZones() { return std::make_tuple(id1, id2); }
		std::tuple<uint&, uint&> getZones() { return {id1, id2}; }
		// std::tuple<uint&, uint&> getZones() { return {id1, id2}; }
	};
	std::vector<Link> links;
	void ComputePerformance(void);
	double ComputeENJ(void);
	double ComputeCI(void);
	double ComputeMTA(void);
	void Dijkstra(const Zone& z1, uint target_id, Zone::Type target_type = Zone::NONE);
// template<typename T>
// std::vector<uint> DijkstraZone(const Zone& z1, std::vector<T>& EndCondition);

    // vector<Site> ; // adj_matrix; // adjacency matrix of a weighted directed graph
    // matrix<int8_t> links; // adj_matrix; // adjacency matrix of a weighted directed graph
    // std::vector< std::shared_ptr<Zone> > zones; //
//   private:
    // std::string name; // city name, also used as file name
    double width, height;


    // uint Zone::Counters[Zone::Type::NbZoneTypes];     // number of zones/mini-islands ? can use zones.size as id
    // uint nb_links;     // number of zones/mini-islands ? can use zones.size as id

  private: // private methods
    bool ZoneAllowed(uint id) const;
    bool SpacePermitsZone(Coord2D center, double radius, uint id = 0) const;
    bool SpacePermitsZone(const Zone& z0) const;
    // bool SpacePermitsZone(const Zone& z0);
	public:
	// void CreateZone(const Zone& z0);

	bool LinkAllowed(uint id1, uint id2);
	// bool LinkAllowed(const Zone& z1, const Zone& z2);
    bool SpacePermitsLink(uint id1, uint id2) const;
    // bool SpacePermitsLink(const Zone& z1, const Zone& z2);
	// void ConnectZones(Zone& z1, Zone& z2);




	// ==================================DOWN============================================
    bool OpenFile(std::string filename);
    void SaveFile(std::string filename);

	void CreateZone(Zone::Type zonetype, Coord2D position, uint nb_people, uint id); //add condition for different id
	void DestroyZone(Zone& z0);

	bool ConnectZones(uint id1, uint id2);
	void DisconnectZones(Zone& z1, Zone& z2);

	// origin, scale, rotation, translation
	double Ox, Oy, S, Rz, Tx, Ty;
	void Origin(void); // TODO:add positionsibility to fix at x,y
	void Scale(double sign = 0);
	void Rotate(double sign = 0);
	void Translate(double dx = 0, double dy = 0);
	void UpdateViewModifiers(void);
	void ResetViewModifiers(void);

	bool zone_edit, link_edit;
	void Draw(const Cairo::RefPtr<Cairo::Context>& cr, Zone& zone);
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;

	std::string InfoCoordinates(Coord2D position);
	Coord2D MouseXY_to_ArchipelagoXY(Coord2D mouse);

	enum EditorOptions { NONE, ADD_ZONE, MODIFY_ZONE, REMOVE_ZONE, ADD_LINK, REMOVE_LINK };
	enum EditState { INIT, UPDATE, END };

	bool AddZone(Zone::Type zonetype, Coord2D position, uint nb_people = 500, uint id = 0);
	bool ModifyZone(Coord2D position, EditState state);
	bool RemoveZone(Coord2D position);

	// bool AddLink(Coord2D origin, EditState state, Coord2D change = {0, 0});
	bool AddLink(Coord2D position, EditState state);
	bool RemoveLink(Coord2D position);

	uint selected_zone;
	Zone varzone;
	uint IdentifyZoneFromXY(Coord2D position);
	double total_time{0};
	std::vector<uint> connection; bool shortest_path{0};
	bool ShortestPath(Coord2D position, EditState state);
	// struct ZoneEditor {
	// 	uint selected_zone;
	// 	Zone backup_zone;
	// 	// enum Options { NONE, ADD_ZONE, MODIFY_ZONE, REMOVE_ZONE };

	// 	void PrepareEditZone(Coord2D position);
	// 	void EditZone(Coord2D position);
	// 	std::string getEditZoneInfo(void);
	// 	void ValidateEditZone(Coord2D position);
	// 	private:
	// 	Vector2D offset; // visual selection gap
	// };
	// struct LinkEditor {
	// 	uint selected_zone;
	// 	Zone fake_zone;
	// 	// enum Options { NONE, ADD_LINK, REMOVE_LINK } link_edit;

	// 	void PrepareAddLink(Coord2D position);
	// 	void AddLink(Coord2D position);
	// 	std::string getAddLinkInfo(void);
	// 	void ValidateAddLink(Coord2D position);
	// };

	// =================================UP=============================================

	// bool moveFlag{0}, resizeFlag{0}, ttFlag{0}, removeFlag{0};
	// void ResetFlags(void) {
	// 	moveFlag=0; resizeFlag=0; connectFlag=0; removeFlag=0; tmpline=0;
	// }
	// void movemenu(void) { moveFlag = 1;}
	// void resizemenu(void) { resizeFlag = 1;}
	// void connectmenu(void) { connectFlag = 1;}
	// void removemenu(void) { removeFlag = 1;}
	//--------------Zone DRAG-----------------//
	// void DragStart(Coord2D position);
	// void DragUpdate(Coord2D position);
	// void DragEnd(Coord2D position);
	// Coord2D offset;
	// Coord2D origin; double nb;
	//-------------Zone RESIZE----------------//

	//-------------Zone CONNECT---------------//

	// std::pair<uint, uint> newlink; // make tuple and include distance
    void Reset(void);
	// Zone& FindZone


	public:
	// void on_button_press_event2(int button, Coord2D position) ;
	// void on_button_release_event2(int button, Coord2D position) ;
	// friend vache;
	// void UpdateCoordinates(void);
	// Coord2D center;
	//for drag tmp line
	// bool tmpline{0};
	// Coord2D tmpcoord{0,0};

	// public:
	//     virtual bool on_motion_notify_event(GdkEventMotion*event) override {std::cout<<"MOTION\n"; return 0;}

	// virtual bool on_button_press_event(GdkEventButton *ev) override {std::cout<<"PRESS\n"; return 1;}
	// virtual bool on_button_release_event(GdkEventButton *ev) override{std::cout<<"RELEASE\n"; return 1;}

	// virtual bool on_scroll_event(GdkEventScroll *ev) override{std::cout<<"SCROLL\n"; return 0;}

    // virtual bool on_key_release_event(GdkEventKey* event) override{std::cout<<"KEY\n";}


// uint selected_zone;
// Coord2D backup_center;
// uint backup_nb_people;




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