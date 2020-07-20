#ifndef ARCHIPELAGO
#define ARCHIPELAGO

#include <gtkmm/drawingarea.h>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <map>
class ArchipelagoUI;

using uint = unsigned int;
// struct vache;
template<class T>
using vector = std::vector<T>;
template<class T>
using matrix = std::vector< std::vector<T> >;

#include "zone.hpp"
#include "link.hpp"
#include "geometry.hpp"

class Archipelago : public Gtk::DrawingArea {

  public: // constructors etc.
    Archipelago(void);
   ~Archipelago(void) = default;


  public:
	std::map< ZoneId, Zone > zones;
	std::map< Link::Id, Link > links;

    std::string edit_text;
    std::string performance;




	void ComputePerformance(void);
	double ComputeENJ(void);
	double ComputeCI(void);
	double ComputeMTA(void);
	std::vector<Link::Id> Dijkstra(ZoneId z1, uint target_id = 0, Zone::Type target_type = Zone::NONE);


    double width, height;

//   private: // private methods





	// ==================================DOWN============================================
    bool OpenFile(std::string filename);
    bool SaveFile(std::string filename);

	void CreateZone(Zone::Type type, Coord2D position, uint nb_people, uint id);
	// bool ZoneIsValid(const Zone& zone) {}
	bool SpacePermitsZone(Coord2D center, double radius, uint ignore = 0);
	bool SpacePermitsZone(const Zone& zone);
	void DestroyZone(ZoneId id);

	void ConnectZones(ZoneId id1, ZoneId id2);
	bool LinkIsValid(const Link& link);
	bool LinkAllowed(ZoneId id1, ZoneId id2);
    bool SpacePermitsLink(ZoneId id1, ZoneId id2);
	void DisconnectZones(ZoneId id1, ZoneId id2);
	void DisconnectZones(Link::Id id);

	// origin, scale, rotation, translation
	float Ox, Oy, S, Rz, Tx, Ty;
	void Origin(void); // TODO:add positionsibility to fix at x,y
	void Scale(double sign = 0);
	void Rotate(double sign = 0);
	void Translate(double dx = 0, double dy = 0);
	void UpdateViewModifiers(void);
	void ResetViewModifiers(void);

	bool zone_edit, link_edit;
	void DrawZone(const Cairo::RefPtr<Cairo::Context>& cr, const Zone& zone);
	void DrawLink(const Cairo::RefPtr<Cairo::Context>& cr, const Link& link);
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;

	std::string InfoFromCoordinates(Coord2D position);
	Coord2D Pointer2ArchipelagoXY(Coord2D mouse);

	enum EditorOptions { NONE, ADD_ZONE, MODIFY_ZONE, REMOVE_ZONE, ADD_LINK, REMOVE_LINK };
	enum EditState { INIT, UPDATE, END };

	void AddZone(Zone::Type zonetype, Coord2D position, uint nb_people = 500, uint id = 0);
	void ModifyZone(Coord2D position, EditState state);
	void RemoveZone(Coord2D position);

	// bool AddLink(Coord2D origin, EditState state, Coord2D change = {0, 0});
	void AddLink(Coord2D position, EditState state);
	void RemoveLink(Coord2D position);

	ZoneId selectedzone;
	Zone   variablezone;
	ZoneId IdentifyZone(Coord2D xy);
	Link::Id IdentifyLink(Coord2D xy);
	double total_time{0};
	std::vector<uint> connection; bool shortest_path{0};
	void ShortestPath(Coord2D position, EditState state);

    void ClearCity(void);

};

#endif//ARCHIPELAGO


	// struct Link {
	// 	// std::shared_ptr<Zone> z1, z2;
	// 	// std::tuple<std::shared_ptr<Zone>, std::shared_ptr<Zone>> getZones() {
	// 	// 	return {z1, z2};
	// 	// }

	// 	float speed, distance, travel_time;
	// 	std::tuple<float&, float&, float&> getInfo() {
	// 		return {speed, distance, travel_time};
	// 	}

	// 	Link(/*std::shared_ptr<Zone> z1, std::shared_ptr<Zone> z2,*/ float v, float d)
	// 	// : z1{z1}, z2{z2}
	// 	, speed{v}, distance{d}, travel_time{d/v}
	// 	, marked{0} {}
	// 	bool marked;
	// };
