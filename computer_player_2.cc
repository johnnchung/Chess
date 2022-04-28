#include <chrono>
#include <limits>
#include <chrono>

#include "board.h"

#include "computer_player_2.h"

using namespace std;

ComputerPlayer2::ComputerPlayer2()
  : rng(std::chrono::system_clock::now().time_since_epoch().count())
{}

int ComputerPlayer2::boardPoints(const Board &board) {
  int points = 0;
  switch (board.getState()) {
  case Board::CHECK:
    points += 5;
  case Board::NORMAL:
    for (int row = 0; row < 8; ++row) {
      for (int col = 0; col < 8; ++col) {
        const Piece *piece = board.at(row, col);
        if (!piece) continue;
        if (piece->colour == board.getTurn()) {
          switch (piece->type) {
          case PAWN:
            points -= 1;
            break;
          case ROOK:
            points -= 5;
            break;
          case KNIGHT:
          case BISHOP:
            points -= 3;
            break;
          case QUEEN:
            points -= 9;
            break;
          case KING:
            // both players should still have their king, meaningless to add points
            break;
          }
        }
      }
    }
    break;
  case Board::CHECKMATE:
    points = std::numeric_limits<int>::max();
    break;
  case Board::STALEMATE:
    // assign 0 points, as initialized
    break;
  case Board::RESIGNED:
    // we would never resign
    break;
  }
  return points;
}

std::unique_ptr<Action> ComputerPlayer2::getAction(const Board &board) {
  std::vector<Move> moves = board.legalMoves();
  int randomIndex = rng() % moves.size();
  Board tmpBoard = board;
  Move bestMove = moves.at(randomIndex);
  tmpBoard.move(bestMove);
  int bestPoints = boardPoints(tmpBoard);
  tmpBoard.undo();

  for (const Move &move : moves) {
    tmpBoard.move(move);
    int points = boardPoints(tmpBoard);
    tmpBoard.undo();
    if (points > bestPoints) {
      bestMove = move;
      bestPoints = points;
    }
  }

  return std::make_unique<Move>(bestMove);
}
