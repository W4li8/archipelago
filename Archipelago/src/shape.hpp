#ifndef SHAPE_H
#define SHAPE_H

#include "coordinates.hpp"

double DistancePoint2Point(Coord2D P, Coord2D Q);
double DistancePoint2Segment(Coord2D P, Coord2D A, Coord2D B);
double DistancePoint2Ray(Coord2D P, Coord2D A, Coord2D B);
double DistancePoint2Line(Coord2D P, Coord2D A, Coord2D B);

class Shape2D {
  public:
    virtual double Distance(const Coord2D& P) = 0;
    virtual double Perimeter(void) = 0;
    virtual double Area(void) = 0;

    virtual Coord2D Center(void) = 0; //centroid for triangle
};


class Circle2D : public Shape2D {
public:
    Circle2D(Coord2D center, double radius): center{center}, radius{radius} {}
    Coord2D center;
    double  radius;

    double Distance(const Coord2D& P) override { return DistancePoint2Point(center, P) - radius; }
    double Perimeter(void) override { return 2*M_PI*radius; }
    double Area(void) override { return M_PI*radius*radius; }

    Coord2D Center(void) override { return center; }
};



#endif//SHAPE_H