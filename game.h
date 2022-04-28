#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include "board.h"
#include "chess_display.h"
#include "player.h"

class Game {
public:
  enum Outcome {
    CHECKMATE_WHITE_WINS,
    CHECKMATE_BLACK_WINS,
    STALEMATE,
    BLACK_RESIGNED,
    WHITE_RESIGNED,
  };
private:
  Board board;
  std::unique_ptr<Player> white;
  std::unique_ptr<Player> black;
  std::vector<std::unique_ptr<ChessDisplay>> displays;
public:
  Game(Board board, std::unique_ptr<Player> white, std::unique_ptr<Player> black, std::vector<std::unique_ptr<ChessDisplay>> displays);
  Outcome run();
};

#endif
