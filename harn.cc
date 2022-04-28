#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "board.h"

std::ostream &operator<<(std::ostream &out, const Coord &coord) {
  int row = coord.row + 1;
  char col = 'a' + coord.col;
  return out << col << row;
}

std::ostream &operator<<(std::ostream &out, PieceType type) {
  switch (type) {
  case PAWN:
    out << "P";
    break;
  case ROOK:
    out << "R";
    break;
  case KNIGHT:
    out << "N";
    break;
  case BISHOP:
    out << "B";
    break;
  case QUEEN:
    out << "Q";
    break;
  case KING:
    out << "K";
    break;
  }
  return out;
}

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

std::istream &operator>>(std::istream &in, Coord &coord) {
  int row;
  char col;
  in >> col >> row;
  coord.row = row - 1;
  coord.col = col - 'a';
  return in;
}

std::istream &operator>>(std::istream &in, PieceType &type) {
  char c;
  in >> c;
  switch (c) {
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
  default:
    // TODO: ERROR
    break;
  }
  return in;
}

void printMove(const Move &move) {
  std::cout << "move " << move.from << " " << move.to;
  if (move.promoteTo != PAWN) std::cout << " " << move.promoteTo;
  std::cout << std::endl;
}

int main() {
  Board board;
  std::string line;
  while (getline(std::cin, line)) {
    std::istringstream iss(std::move(line));
    std::string cmd;
    iss >> cmd;
    switch (cmd[0]) {
      case 'p': {
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
      } break;
      case 'm': {
        Coord from(0, 0), to(0, 0);
        PieceType promoteTo = PAWN;
        iss >> from >> to >> promoteTo;
        board.move(Move(from, to, promoteTo));
      } break;
      case 'l': {
        for (const Move &move : board.legalMoves()) {
          printMove(move);
        }
      } break;
      case 'u': {
        board.undo();
      } break;
    }
  }
}
