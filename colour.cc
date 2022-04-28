#include "colour.h"

Colour operator!(Colour colour) {
  if (colour == WHITE) return BLACK;
  return WHITE;
}
