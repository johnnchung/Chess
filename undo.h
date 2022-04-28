#ifndef UNDO_H
#define UNDO_H

#include "action.h"

class ActionVisitor;

class Undo : public Action {
public:
  Undo();
  void accept(ActionVisitor &visitor) override;
};


#endif
