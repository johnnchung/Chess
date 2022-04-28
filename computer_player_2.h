#ifndef COMPUTER_PLAYER_2_H
#define COMPUTER_PLAYER_2_H

#include <random>

#include "player.h"

class Board;

class ComputerPlayer2 : public Player {
  std::mt19937_64 rng;
  static int boardPoints(const Board &board);
public:
  ComputerPlayer2();
  std::unique_ptr<Action> getAction(const Board &board) override;
};

#endif
