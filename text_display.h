#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H

#include "chess_display.h"

class Board;

class TextDisplay: public ChessDisplay {
public:
  TextDisplay();
  void display(const Board & board) override;
};

#endif
