#include <algorithm>
#include <chrono>
#include <limits>
#include <chrono>

#include "board.h"

#include "computer_player_3.h"

ComputerPlayer3::ComputerPlayer3()
  : rng(std::chrono::system_clock::now().time_since_epoch().count())
{}

int ComputerPlayer3::boardPoints(const Board &board) {
  int points = 0;
  switch (board.getState()) {
  case Board::CHECK:
    // we get checked
    points -= 5;
  case Board::NORMAL:
    for (int row = 0; row < 8; ++row) {
      for (int col = 0; col < 8; ++col) {
        const Piece *piece = board.at(row, col);
        if (!piece) continue;
        int value = 0;
        switch (piece->type) {
        case PAWN:
          value = 1;
          break;
        case ROOK:
          value = 5;
          break;
        case KNIGHT:
          value = 3;
          break;
        case BISHOP:
          value = 3;
          break;
        case QUEEN:
          value = 9;
          break;
        case KING:
          // king can never be captured, meaningless to assign value
          // value is 0 as initialized
          break;
        }
        if (piece->colour == board.getTurn()) {
          // our piece
          points += value;
        } else {
          // enemy's piece
          points -= value;
        }
      }
    }
    break;
  case Board::CHECKMATE:
    // we get checkmated, negative infinity points
    points = std::numeric_limits<int>::min();
    break;
  case Board::STALEMATE:
    // assign 0 points, as initialized
    break;
  case Board::RESIGNED:
    // if the enemy resigns (they won't, but theoretically) we get infinite points
    points = std::numeric_limits<int>::max();
    break;
  }
  return points;
}

int ComputerPlayer3::movePoints(Board &board, const Move &move) {
  board.move(move);
  int points = 0;
  switch (board.getState()) {
  case Board::CHECK:
    points += 5;
  case Board::NORMAL:
    {
      int worstPoints = std::numeric_limits<int>::max();
      // there should be moves since state is normal
      for (const Move &nextMove : board.legalMoves()) {
        board.move(nextMove);
        worstPoints = std::min(worstPoints, boardPoints(board));
        board.undo();
      }
      points += worstPoints;
    }
    break;
  case Board::CHECKMATE:
    points = std::numeric_limits<int>::max();
    break;
  case Board::STALEMATE:
    points = 0;
    break;
  case Board::RESIGNED:
    // we would never resign
    // this should never be reached
    break;
  }
  board.undo();
  return points;
}

std::unique_ptr<Action> ComputerPlayer3::getAction(const Board &board) {
  std::vector<Move> moves = board.legalMoves();
  int randomIndex = rng() % moves.size();
  Board tmpBoard = board;
  Move bestMove = moves.at(randomIndex);
  int bestPoints = movePoints(tmpBoard, bestMove);
  for(const Move &move : moves) {
    int points = movePoints(tmpBoard, move);
    if (points > bestPoints) {
      bestMove = move;
      bestPoints = points;
    }
  }
  return std::make_unique<Move>(bestMove);
}
