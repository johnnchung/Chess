#include <string>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "board.h"

#include "graphic_display.h"

using namespace std;

GraphicDisplay::GraphicDisplay() : incremental{ false } {}

char pieceChar(Piece piece) {
  char c = 'P';
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
  if (piece.colour == BLACK) c = c - 'A' + 'a';
  return c;
}

void GraphicDisplay::redraw(const Board &board, int row, int col) {
  int foreground = (row + col) % 2;
  int background = !foreground;
  w.fillRectangle(50 + col * 50, 360 - row * 50, 50, 50, background);
  const Piece *piece = board.at(row, col);
  if (piece) {
    w.drawStringColour(75 + col * 50, 385 - row * 50, std::string{ pieceChar(*piece) }, foreground);
  }
}

void GraphicDisplay::display(const Board &board) {
  if (incremental) {
    for (Coord coord : board.getChangedCoords()) {
      redraw(board, coord.row, coord.col);
    }
  } else {
    incremental = true;
    for (int row = 0; row < 8; ++row) {
      char c = '1' + row;
      w.drawString(30, 395 - row * 50, std::string{ c });
    }
    for (int col = 0; col < 8; ++col) {
      char c = 'a' + col;
      w.drawString(70 + col * 50, 440, std::string{ c });
    }
    for (int row = 0; row < 8; ++row) {
      for (int col = 0; col < 8; ++col) {
        redraw(board, row, col);
      }
    }
  }
}
