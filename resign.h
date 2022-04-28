#ifndef RESIGN_H
#define RESIGN_H

#include "action.h"

class ActionVisitor;

class Resign : public Action {
public:
  Resign();
  void accept(ActionVisitor &visitor) override;
};


#endif
