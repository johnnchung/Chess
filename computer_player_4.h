#ifndef COMPUTER_PLAYER_4_H
#define COMPUTER_PLAYER_4_H

#include <random>

#include "player.h"

class Board;

class ComputerPlayer4 : public Player {
  std::mt19937_64 rng;
  // maximizePlayer is true if we are evaluating after a move by the maximizing player
  static int boardPoints(const Board &board, bool maximizePlayer);
  static int minimax(Board &board, int depth, int alpha, int beta, bool maximizePlayer);
public:
  ComputerPlayer4();
  std::unique_ptr<Action> getAction(const Board &board) override;
};

#endif
