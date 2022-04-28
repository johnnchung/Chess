#ifndef COMPUTER_PLAYER_1_H
#define COMPUTER_PLAYER_1_H

#include <random>

#include "player.h"

class Board;

class ComputerPlayer1 : public Player {
  std::mt19937_64 rng;
public:
  ComputerPlayer1();
  std::unique_ptr<Action> getAction(const Board &board) override;
};

#endif
