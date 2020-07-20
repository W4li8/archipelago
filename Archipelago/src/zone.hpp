#ifndef ZONE_HPP
#define ZONE_HPP

using ZoneId = uint;
// class Zone;
#include "utilities.hpp"
#include "geometry.hpp"
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
class Zone {
  public:
    // order defines position in map which defines reading order from file
    enum Type { NONE, RESIDENTIAL, TRANSPORT, PRODUCTION, CNT};
    static inline std::map< Type, std::vector<ZoneId> > Types = {
      {RESIDENTIAL, {}},
      {TRANSPORT,   {}},
      {PRODUCTION,  {}}
    };

    // struct Id {

    //     const uint id;
    //     const Zone::Type type;

    //     friend bool operator<(const ZoneXId& lhs, const ZoneXId& rhs) {
    //         return lhs.id != rhs.id && lhs.type < rhs.type;
    //     }
    // };


    friend std::ostream& operator<<(std::ostream& os, const Zone& obj) {
        return os << obj.id <<' '<< obj.location.x <<' '<< obj.location.y <<' '<< obj.capacity;
    }
    friend std::ostream& operator<<(std::ostream& os, const Zone::Type& type) { return os;}
    std::string ZoneType2String(Type type) {
        switch(type) {
          case PRODUCTION: return "Production Zone ";
          case TRANSPORT: return "Transport Hub ";
          case RESIDENTIAL: return "Residential Area ";
          default:
            return "";
        }
    }

    Zone(ZoneId id, Type type, Coord2D xy, uint nb)
    : id{id}, type{type}, location{xy}, capacity{nb}/*, neighbours{{}}*/ {
        // for(auto x : neighbours)std::cout<<x<<',';std::cout<<'\n';

        if(type)Zone::Types.at(type).push_back(id);
    }
    // Zone(const Zone& obj)
    // : id{obj.id}, type{obj.type}, location{obj.location}, capacity{obj.capacity}, neighbours{obj.neighbours} {
    //     std::vector<ZoneId>& v = Zone::Types.at(type);
    //     if(std::find(v.begin(), v.end(), id) == v.end()) {
    //         Zone::Types.at(type).push_back(id); std::cout<<Zone::Types.at(type).size()<<" COPY"<<id<<"\n";
    //     }
    // }
   ~Zone(void) {
        if(type)Zone::Types.at(type).erase(std::find(Zone::Types.at(type).begin(), Zone::Types.at(type).end(), id));
    }
        // std::vector<ZoneId>& v = Zone::Types.at(type);
    Zone(void)
    : id{0}, type{NONE}, location{Coord2D()}, capacity{0}, neighbours{{}} {}

  public: /* -- DATA ----------------------------------------------- */
    const ZoneId id; // unique identifier
    const Type type; // zone classifier
  private:
    Coord2D location; // map coordinates x, y
    uint capacity;    // max population, determines the size of the zone
  public:
    std::vector< ZoneId > neighbours; // adjacent zones (compiler error for vector of const)

  public: /* -- METHODS -- GET/SET --------------------------------- */
    Coord2D getLocation(void) const { return location; }
    Coord2D getCenter(void)   const { return location; } // justified ? should be abstract
    float getCapacity(void) const { return capacity; }
    float getRadius(void)   const { return sqrt(capacity); }

    std::vector< ZoneId > getNeighbours(void) {
        // for(auto x : neighbours)std::cout<<x<<',';std::cout<<'\n';
        return neighbours; }


    void setCenter(Coord2D c) { location = c; }
    void setRadius(float r)   { capacity = r*r; }

    void AddNeighbour(const ZoneId id)    { neighbours.emplace_back(id); } // and conditions...
    void RemoveNeighbour(const ZoneId id) { neighbours.erase(std::find(neighbours.begin(), neighbours.end(), id)); }

  public: /* -- METHODS -- INFO ------------------------------------ */
    bool CanTraverse(void)      const { return type != PRODUCTION; }
    bool IsOftenCongested(void) const { return (type == RESIDENTIAL) ? (neighbours.size() > 3) : false; }
    bool HasSpeedLimit(void)    const { return type != TRANSPORT; }

    std::string getInfoString(void) {
        return ZoneType2String(type) + str(id) + "\nCapacity: "+ str(capacity);
    }
  public: /* -- METHODS -------------------------------------------- */

    float Distance(const Coord2D& xy) { return fDistancePoint2Point(location, xy) - getRadius(); }
    float DistanceFromCenter(const Coord2D& xy) { return fDistancePoint2Point(location, xy); }
    float Distance(const Zone& zone) const { return fDistancePoint2Point(this->location, zone.location)
                                              - this->getRadius() - zone.getRadius(); }

};

#endif//ZONE_HPP