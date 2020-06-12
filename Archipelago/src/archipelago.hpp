#ifndef ARCHIPELAGO
#define ARCHIPELAGO

#include "zones.hpp"
#include <gtkmm/drawingarea.h>
#include <string>
#include <vector>


struct Site {
    // const Zone& z1;
    // const Zone& z2;
	uint id, it;
}

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
    void Edit(void); //or design

    vector<Zone> zones; //
    vector<Site> ; // adj_matrix; // adjacency matrix of a weighted directed graph
    matrix<bool> links; // adj_matrix; // adjacency matrix of a weighted directed graph
    // std::vector< std::shared_ptr<Zone> > zones; //
//   private:
    std::string name; // city name, also used as file name


    uint8_t nb_zones[NbZoneTypes] = {0};     // number of zones/mini-islands ? can use zones.size as id

  private: // private methods
    bool SpacePermitsZone(const Zone& z0);
    bool SpacePermitsLink(const Zone& z1, const Zone& z2);

    bool on_draw(const Cairo::RefPtr<Cairo::Context>& pencil) override;
    void Reset(void);
};

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