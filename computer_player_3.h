#ifndef COMPUTER_PLAYER_3_H
#define COMPUTER_PLAYER_3_H

#include <random>

#include "player.h"

class Board;

class ComputerPlayer3 : public Player {
  std::mt19937_64 rng;
  // should only be called at 2 moves into the future
  static int boardPoints(const Board &board);
  static int movePoints(Board &board, const Move &move);
public:
  ComputerPlayer3();
  std::unique_ptr<Action> getAction(const Board &board) override;
};

#endif
