#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "geometry.hpp"
#include <gtkmm/drawingarea.h>


using Pencil = const Cairo::RefPtr<Cairo::Context>;

struct Color {
    Color(int red, int green, int blue, double alpha = 1.0)
    : red{double(red)/255}, green{double(green)/255}, blue{double(blue)/255}, alpha{alpha} {}
    Color(Color old, double alpha)
    : red{old.red}, green{old.green}, blue{old.blue}, alpha{alpha} {}


    constexpr Color(uint8_t r, uint8_t g, uint8_t b, double a)
        : red(r/255), green(g/255), blue(b/255), alpha(a) {}

    double red, green, blue, alpha;
};

const Color black {  0,  0,  0};
const Color white {255,255,255};
const Color red {255,  0,  0};
const Color green {  0,255,  0};
const Color blue {  0,  0,255};
const Color macdefault {246,245,244};

void Draw(Pencil& cr, const Point2D& P, Color line = black);
void Draw(Pencil& cr, const Segment2D& S, Color line = black);
void Draw(Pencil& cr, const Circle2D& C, Color line = black, Color fill = white);
void Draw(Pencil& cr, const Triangle2D& T, Color line = black, Color fill = white);
void Draw(Pencil& cr, const Quadrilateral2D& R, Color line = black, Color fill = white);
void Draw(Pencil& cr, const Polygon2D& Poly, Color line = black, Color fill = white);

#endif//GRAPHICS_H