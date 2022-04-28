#include <algorithm>
#include <chrono>
#include <limits>

#include "board.h"

#include "computer_player_4.h"

ComputerPlayer4::ComputerPlayer4()
  : rng(std::chrono::system_clock::now().time_since_epoch().count())
{}

int ComputerPlayer4::boardPoints(const Board &board, bool maximizePlayer) {
  int points = 0;
  switch (board.getState()) {
  case Board::CHECK:
    // phasing player gets checked
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
          // phasing player's piece
          points += value;
        } else {
          // phasing player's opponent's piece
          points -= value;
        }
      }
    }
    break;
  case Board::CHECKMATE:
    // phasing player gets checkmated, negative infinity points
    // use negative int max so we can flip the sign
    points = -std::numeric_limits<int>::max();
    break;
  case Board::STALEMATE:
    // assign 0 points, as initialized
    break;
  case Board::RESIGNED:
    // if the enemy resigns (they won't, but theoretically) phasing player
    // gets infinite points
    points = std::numeric_limits<int>::max();
    break;
  }
  if (maximizePlayer) {
    // phasing player is minimizing player
    return points;
  } else {
    return -points;
  }
}


int ComputerPlayer4::minimax(Board &board, int depth, int alpha, int beta, bool maximizePlayer) {
  if(depth <= 0) return boardPoints(board, maximizePlayer);
  std::vector<Move> moves = board.legalMoves();
  if (moves.empty()) return boardPoints(board, maximizePlayer);

  if(maximizePlayer) {
    int maxEval = std::numeric_limits<int>::min();
    for (const Move &move : moves) {
      board.move(move);
      int eval = minimax(board, depth - 1, alpha, beta, !maximizePlayer);
      board.undo();  
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, maxEval);
      if(beta <= alpha) break;
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    for (const Move &move : moves) {
      board.move(move);
      int eval = minimax(board, depth - 1, alpha, beta, !maximizePlayer);
      board.undo();  
      minEval = std::min(minEval, eval);
      beta = std::min(beta, minEval);
      if(beta <= alpha) break;
    }
    return minEval;
  }
}

std::unique_ptr<Action> ComputerPlayer4::getAction(const Board &board) {
  const int depth = 2;
  std::vector<Move> moves = board.legalMoves();
  int randomIndex = rng() % moves.size();
  Board tmpBoard = board;
  Move bestMove = moves.at(randomIndex);
  tmpBoard.move(bestMove);
  int bestPoints = minimax(tmpBoard, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
  tmpBoard.undo();

  for(const Move &move : moves) {
    tmpBoard.move(move);
    int points = minimax(tmpBoard, depth, bestPoints, std::numeric_limits<int>::max(), false);
    if (points > bestPoints) {
      bestMove = move;
      bestPoints = points;
    }
    tmpBoard.undo();
  }

  return std::make_unique<Move>(bestMove);
}
