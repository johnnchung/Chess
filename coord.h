#ifndef COORD_H
#define COORD_H

#include <istream>

struct Coord {
  int row, col;
  Coord(int row, int col);
  // allows std::set of Coord
  // facilitates Move::operator<
  bool operator<(Coord other) const;
  bool operator==(Coord other) const;
  bool operator!=(Coord other) const;
  Coord operator+(Coord other) const;
  Coord &operator+=(Coord other);
};

std::istream &operator>>(std::istream &in, Coord &coord);

#endif
