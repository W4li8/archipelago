#ifndef ARCHIPELAGO
#define ARCHIPELAGO


#include <gtkmm/drawingarea.h>
#include <string>
#include <vector>
#include <map>

#include "link.hpp"
#include "zone.hpp"

class Archipelago : public Gtk::DrawingArea {
  public: /* --- DEFINITIONS --------------------------------------- */
	enum class EditorOptions { NONE, ADD_ZONE, MODIFY_ZONE, REMOVE_ZONE, ADD_LINK, REMOVE_LINK };
	enum class EditState 	 { INIT, UPDATE, END };


  public: // constructors etc.
    Archipelago(void);


  public:
	std::map< ZoneId, Zone > zones;
	std::map< LinkId, Link > links;

    std::string edit_text;
    std::string performance;




	void  ComputePerformance(void); // in ArchipelagoUI
	float ComputeENJ(void);
	float ComputeCI(void);
	float ComputeMTA(void);
	std::vector<LinkId> Dijkstra(const ZoneId zi, const std::vector<ZoneId> zf);


    float width, height;

//   private: // private methods
	enum class ZoneEdit { NONE, MOVE, RESIZE } zone_edit;
	enum class LinkEdit { NONE, ADD } link_edit;





	// ==================================DOWN============================================
    bool OpenFile(std::string filename);
    bool SaveFile(std::string filename);

	void CreateZone(ZoneType type, Coord2D position, uint nb_people, uint id);
	// bool ZoneIsValid(const Zone& zone) {}
	bool SpacePermitsZone(Coord2D center, float radius, uint ignore = 0);
	bool SpacePermitsZone(const Zone& zone);
	void DestroyZone(ZoneId id);

	void ConnectZones(ZoneId id1, ZoneId id2);
	bool LinkIsValid(const Link& link);
	bool LinkAllowed(ZoneId id1, ZoneId id2);
    bool SpacePermitsLink(ZoneId id1, ZoneId id2);
	void DisconnectZones(ZoneId id1, ZoneId id2);
	void DisconnectZones(LinkId id);

	// origin, scale, rotation, translation
	float Ox, Oy, S, Rz, Tx, Ty;
	void Origin(void); // TODO:add positionsibility to fix at x,y
	void Scale(float sign = 0);
	void Rotate(float sign = 0);
	void Translate(float dx = 0, float dy = 0);
	void UpdateViewModifiers(void);
	void ResetViewModifiers(void);

	void DrawZone(const Cairo::RefPtr<Cairo::Context>& cr, const Zone& zone);
	void DrawLink(const Cairo::RefPtr<Cairo::Context>& cr, const Link& link);
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;

	std::string InfoFromCoordinates(Coord2D position);
	Coord2D Pointer2ArchipelagoXY(double mx, double my);




	void AddZone(ZoneType zonetype, Coord2D position, uint nb_people = 500, uint id = 0);
	void ModifyZone(Coord2D position, EditState state);
	void RemoveZone(Coord2D position);

	// bool AddLink(Coord2D origin, EditState state, Coord2D change = {0, 0});
	void AddLink(Coord2D position, EditState state);
	void RemoveLink(Coord2D position);


	ZoneId selectedzone;
	Coord2D cursor;

	ZoneId IdentifyZone(Coord2D xy);
	LinkId IdentifyLink(Coord2D xy);
	float total_time{0};
	std::vector<uint> connection; bool shortest_path{0};
	void ShortestPath(EditState state, Coord2D position = {0, 0}); // make default out of this world

    void ClearCity(void);

};

#endif//ARCHIPELAGO


	// struct Link {
	// 	// std::shared_ptr<Zone> z1, z2;
	// 	// std::tuple<std::shared_ptr<Zone>, std::shared_ptr<Zone>> getZones() {
	// 	// 	return {z1, z2};
	// 	// }

	// 	float speed, distance, time;
	// 	std::tuple<float&, float&, float&> getInfo() {
	// 		return {speed, distance, time};
	// 	}

	// 	Link(/*std::shared_ptr<Zone> z1, std::shared_ptr<Zone> z2,*/ float v, float d)
	// 	// : z1{z1}, z2{z2}
	// 	, speed{v}, distance{d}, time{d/v}
	// 	, marked{0} {}
	// 	bool marked;
	// };
