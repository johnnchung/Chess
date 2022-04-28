#ifndef PIECE_H
#define PIECE_H

#include "colour.h"
#include "piece_type.h"

struct Piece {
  Colour colour;
  PieceType type;
  Piece(Colour colour, PieceType type);
};

#endif
