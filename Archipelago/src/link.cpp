#include "link.hpp"

#ifdef TMP
    struct Id {
      public: /* -- XXX -------------------------------------------- */
        Id(ZoneId id1, ZoneId id2)
        : id1{MIN(id1, id2)}, id2{MAX(id1, id2)} {}
        // easy access: [A] -> id1 & [B] -> id2
        ZoneId operator[](Extremity x) const {
            return x ? id2 : id1;
        }
      public: /* -- DATA ------------------------------------------- */
        const ZoneId id1;
        const ZoneId id2;

      public:
        // copy of operator< expression for std::pair, used by std::map for comparisons
        friend bool operator<(const Link::Id& lhs, const Link::Id& rhs) {
            return lhs.id1 < rhs.id1 || (!(rhs.id1 < lhs.id1) && lhs.id2 < rhs.id2);
        }
        operator bool() const { return id1 && id2; }

    };
  public:
    // Link(Id idx, float v, float d)
    // : idx{idx}, speed{v}, distance{d}, travel_time{d/v}, marked{0} {}


    // easy access: [A] -> z1 & [B] -> z2
    const Zone& operator[](Extremity x) const {
        return x ? z2 : z1;
    }
  public: /* -- DATA ----------------------------------------------- */

  private:
    const Zone& z1;
    const Zone& z2;
  public:
    float speed;    // defined by infrastructure
    float distance; // connection length
    float travel_time;     // travel travel_time

  public: /* -- FLAGS ---------------------------------------------- */
    bool marked; // is on selected path

  public:
    std::string getInfoString(void) {
        return "Link between "+ str(z2.id) +" and "+ str(z1.id) +"\nSpeed: "+ str(speed);
    }
    bool Connects(uint id) { return id == z1.id or id == z2.id; }
#endif

Link::Link(const Zone& z1, const Zone& z2)
: z1{z1}, z2{z2}
, speed{(z1.HasSpeedLimit() or z2.HasSpeedLimit()) ? S_SPEED : F_SPEED}
, distance{z1.Distance(z2)}
, travel_time{distance/speed}
, marked{0} {}