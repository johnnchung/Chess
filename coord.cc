#include "coord.h"

Coord::Coord(int row, int col) : row{ row }, col{ col } {}

bool Coord::operator<(Coord other) const {
  return row < other.row || (row == other.row && col < other.col);
}

bool Coord::operator==(Coord other) const {
  return row == other.row && col == other.col;
}

bool Coord::operator!=(Coord other) const {
  return !(*this == other);
}

Coord Coord::operator+(Coord other) const {
  return Coord(row + other.row, col + other.col);
}

Coord &Coord::operator+=(Coord other) {
  row += other.row;
  col += other.col;
  return *this;
}

std::istream &operator>>(std::istream &in, Coord &coord) {
  int row;
  char col;
  if (!(in >> col >> row)) return in;
  if (row < 1 || row > 8 || col < 'a' || col > 'z') {
    in.setstate(std::istream::failbit);
    return in;
  }
  coord.row = row - 1;
  coord.col = col - 'a';
  return in;
}
