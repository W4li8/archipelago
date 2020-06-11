#ifndef ARCHIPELAGO
#define ARCHIPELAGO

#include "zones.hpp"
#include <gtkmm/drawingarea.h>
class Archipelago : public Gtk::DrawingArea {

  public: // constructors etc.
    Archipelago(void);
   ~Archipelago(void) = default;

  public:
    void OpenFile(void);
    void SaveFile(void);
    void Edit(void); //or design

    std::vector< std::shared_ptr<Zone> > zones; //
//   private:
    std::string name; // city name, also used as file name

    std::vector< std::vector<uint8_t> > adj_matrix; // adjacency matrix of a weighted directed graph

    uint8_t nb_zones[NbZoneTypes] = {0};     // number of zones/mini-islands ? can use zones.size as id

    void Draw();
//  private: // private methods
    bool SpacePermitsZone(const Zone& z0);
    bool SpacePermitsLink(const Zone& z1, const Zone& z2);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override {return true;}
};

#endif//ARCHIPELAGO