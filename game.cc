#include <iostream>

#include "action_visitor.h"
#include "resign.h"
#include "undo.h"

#include "game.h"

class ActionPerformer : public ActionVisitor {
  Board &board;
public:
  ActionPerformer(Board &board) : board{ board } {}
  void visit(Move &move) override { board.move(move); }
  void visit(Resign &) override { board.resign(); }
  void visit(Undo &) override {
    board.atomicUndo();
  }
};

Game::Game(Board board, std::unique_ptr<Player> white, std::unique_ptr<Player> black, std::vector<std::unique_ptr<ChessDisplay>> displays)
  : board{ std::move(board) }
  , white{ std::move(white) }
  , black{ std::move(black) }
  , displays{ std::move(displays) }
{}

Game::Outcome Game::run() {
  while (true) {
    for (std::unique_ptr<ChessDisplay> &display : displays) {
      display->display(board);
    }
    Colour turn = board.getTurn();
    switch (board.getState()) {
    case Board::NORMAL:
      break;
    case Board::CHECK:
      if (turn == WHITE) {
        std::cout << "White is in check." << std::endl;
      } else { // turn == BLACK
        std::cout << "Black is in check." << std::endl;
      }
      break;
    case Board::CHECKMATE:
      if (turn == WHITE) {
        std::cout << "Checkmate! Black wins!" << std::endl;
        return CHECKMATE_BLACK_WINS;
      } else { // turn == BLACK
        std::cout << "Checkmate! White wins!" << std::endl;
        return CHECKMATE_WHITE_WINS;
      }
      break;
    case Board::STALEMATE:
      std::cout << "Stalemate!" << std::endl;
      return STALEMATE;
      break;
    case Board::RESIGNED:
      if (turn == WHITE) {
        std::cout << "White wins!" << std::endl;
        return BLACK_RESIGNED;
      } else { // turn == BLACK
        std::cout << "Black wins!" << std::endl;
        return WHITE_RESIGNED;
      }
      break;
    }
    std::unique_ptr<Action> action;
    if (turn == WHITE) {
      action = white->getAction(board);
    } else { // turn == BLACK
      action = black->getAction(board);
    }
    {
      ActionPerformer performer(board);
      action->accept(performer);
    }
  }
}
