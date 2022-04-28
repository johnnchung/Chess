#ifndef MOVE_H
#define MOVE_H

#include "action.h"
#include "coord.h"
#include "piece_type.h"

struct Move : Action {
  Coord from, to;
  PieceType promoteTo;
  Move(Coord from, Coord to, PieceType promoteTo = PAWN);
  void accept(ActionVisitor &visitor) override;
  // this is so we can put moves into std::set
  bool operator<(const Move &) const;
};


#endif
