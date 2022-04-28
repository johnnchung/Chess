#include <iostream>
#include <string>

#include "board.h"

#include "text_display.h"

TextDisplay::TextDisplay() {}

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

void TextDisplay::display(const Board &board) {
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
