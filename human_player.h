#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "player.h"

class Board;

class HumanPlayer : public Player {
public:
  HumanPlayer();
  std::unique_ptr<Action> getAction(const Board &board) override;
};

#endif
