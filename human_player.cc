#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "board.h"
#include "resign.h"
#include "undo.h"

#include "human_player.h"

HumanPlayer::HumanPlayer() {}

std::istream &operator>>(std::istream &in, PieceType &type) {
  char c;
  if (!(in >> c)) return in;
  switch (c) {
  case 'P':
    type = PAWN;
    break;
  case 'R':
    type = ROOK;
    break;
  case 'N':
    type = KNIGHT;
    break;
  case 'B':
    type = BISHOP;
    break;
  case 'Q':
    type = QUEEN;
    break;
  case 'K':
    type = KING;
    break;
  default:
    in.setstate(std::istream::failbit);
    break;
  }
  return in;
}

std::unique_ptr<Action> HumanPlayer::getAction(const Board &board) {
  std::unique_ptr<Action> action;
  std::string line;
  do {
    getline(std::cin, line);
    std::istringstream iss(std::move(line));
    std::string cmd;
    iss >> cmd;
    switch (cmd[0]) {
      case 'm': {
        Coord from(0, 0), to(0, 0);
        PieceType promoteTo = PAWN;
        if (iss >> from >> to) {
          iss >> promoteTo;
          Move move(from, to, promoteTo);
          if (board.isLegalMove(move)) {
            action = std::make_unique<Move>(from, to, promoteTo);
          } else {
            std::cout << "Illegal move." << std::endl;
          }
        }
      } break;
      case 'u': {
        if (board.hasPriorMove()) {
          action = std::make_unique<Undo>();
        } else {
          std::cout << "No prior move to undo." << std::endl;
        }
      } break;
      case 'r': {
        action = std::make_unique<Resign>();
      } break;
      default: {
        std::cout << "Unrecognized command." << std::endl;
      } break;
    }
  } while (!action);
  return action;
}
