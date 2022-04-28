#include "action_visitor.h"

#include "undo.h"

Undo::Undo() {}

void Undo::accept(ActionVisitor &visitor) {
  visitor.visit(*this);
}
