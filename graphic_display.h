#ifndef GRAPHIC_DISPLAY_H
#define GRAPHIC_DISPLAY_H

#include <X11/Xlib.h>

#include "chess_display.h"
#include "window.h"

class Board;


class GraphicDisplay: public ChessDisplay {
  Xwindow w;
  bool incremental;
  void redraw(const Board &board, int row, int col);
public:
  GraphicDisplay();
  void display(const Board & board) override;
};

#endif
