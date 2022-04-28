#include <iostream>

#include "board.h"

std::ostream &operator<<(std::ostream &out, Piece piece) {
  char c;
  switch (piece.type) {
  case PAWN:
    c = 'P';
    break;
  case ROOK:
    c = 'R';
    break;
  case KNIGHT:
    c = 'N';
    break;
  case BISHOP:
    c = 'B';
    break;
  case QUEEN:
    c = 'Q';
    break;
  case KING:
    c = 'K';
    break;
  }
  if (piece.colour == BLACK) {
    c += 'a' - 'A';
  }
  return out << c;
}

void printBoard(const Board &board) {
  std::cout << "\e[2J";
  for (int row = 7; row >=0; --row) {
    std::cout << row + 1 << " ";
    for (int col = 0; col < 8; ++col) {
      const Piece *piece = board.at(row, col);
      if (piece) {
        std::cout << *piece;
      } else {
        if ((row + col) % 2) {
          std::cout << ' ';
        } else {
          std::cout << '_';
        }
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "  abcdefgh" << std::endl;
}

uint64_t perft(Board &board, int depth) {
  if (depth == 0) return 1;
  uint64_t nodes = 0;
  for (const Move &move : std::vector<Move>(board.legalMoves())) {
    board.move(move);
    // printBoard(board);
    nodes += perft(board, depth - 1);
    board.undo();
    // printBoard(board);
  }
  return nodes;
}

int main() {
  for (int i = 0; i < 5; ++i) {
    Board board;
    // perft(board, i);
    std::cout << perft(board, i) << std::endl;
  }
}
