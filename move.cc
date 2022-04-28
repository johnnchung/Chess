#include "action_visitor.h"

#include "move.h"

Move::Move(Coord from, Coord to, PieceType promoteTo)
  : from{ from }, to{ to }, promoteTo{ promoteTo }
{}

void Move::accept(ActionVisitor &visitor) {
  visitor.visit(*this);
}

bool Move::operator<(const Move &other) const {
  if (from < other.from) return true;
  if (other.from < from) return false;
  if (to < other.to) return true;
  if (other.to < to) return false;
  return promoteTo < other.promoteTo;
}
