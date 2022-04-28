#include <chrono>

#include "board.h"

#include "computer_player_1.h"

ComputerPlayer1::ComputerPlayer1()
  : rng(std::chrono::system_clock::now().time_since_epoch().count())
{}

std::unique_ptr<Action> ComputerPlayer1::getAction(const Board &board) {
  std::vector<Move> moves = board.legalMoves();
  int randomIndex = rng() % moves.size();
  return std::make_unique<Move>(moves.at(randomIndex));
}
