#ifndef PLAYER_H
#define PLAYER_H

#include "action.h"
#include <memory>

class Board;

class Player {
public:
  virtual ~Player() = 0;
  virtual std::unique_ptr<Action> getAction(const Board &board) = 0;
};

#endif
