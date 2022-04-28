#include "action_visitor.h"

#include "resign.h"

Resign::Resign() {}

void Resign::accept(ActionVisitor &visitor) {
  visitor.visit(*this);
}
