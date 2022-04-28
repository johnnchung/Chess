#ifndef ACTION_VISITOR_H
#define ACTION_VISITOR_H

struct Move;
class Resign;
class Undo;

class ActionVisitor {
public:
  virtual ~ActionVisitor() = 0;
  virtual void visit(Move &) = 0;
  virtual void visit(Resign &) = 0;
  virtual void visit(Undo &) = 0;
};

#endif
