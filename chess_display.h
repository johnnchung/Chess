#ifndef DISPLAY_H
#define DISPLAY_H

#include "colour.h"
#include "piece.h"

class Board;

class ChessDisplay {
public:
  virtual ~ChessDisplay() = 0;
  virtual void display(const Board &board) = 0;
};

#endif
