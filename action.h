#ifndef ACTION_H_
#define ACTION_H_

class ActionVisitor;

class Action {
public:
  virtual ~Action() = 0;
  virtual void accept(ActionVisitor &visitor) = 0;
};

#endif
