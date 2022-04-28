#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <memory>
#include <set>
#include <vector>

#include "colour.h"
#include "move.h"
#include "piece.h"

class Board {
  struct Square {
    std::unique_ptr<Piece> piece;
    std::set<Coord> observers;
    std::vector<Coord> subjects;
    std::vector<Coord> moves;
    std::array<bool, 2> attacksKing;
    Square();
    Square(const Square &);
    Square(Square &&);
    Square &operator=(const Square &);
    Square &operator=(Square &&);
  };
  struct CastlingRights {
    bool queenSide, kingSide;
    CastlingRights();
    bool operator==(CastlingRights other) const;
    bool operator!=(CastlingRights other) const;
  };
  struct Capture {
    Piece piece;
    Coord location;
    Capture(Piece piece, Coord location);
  };
  struct Crumb {
    Move move;
    std::unique_ptr<Capture> capture;
    std::unique_ptr<Coord> enPassantTarget;
    std::array<CastlingRights, 2> castlingRights;
    Crumb(const Move &move, std::unique_ptr<Capture> capture,
      std::unique_ptr<Coord> enPassantTarget,
      std::array<CastlingRights, 2> castlingRights);
    Crumb(const Crumb &);
    Crumb(Crumb &&);
    Crumb &operator=(const Crumb &);
    Crumb &operator=(Crumb &&);
  };
public:
  enum State {
    NORMAL,
    CHECK,
    CHECKMATE,
    STALEMATE,
    RESIGNED,
  };
private:
  std::array<std::array<Square, 8>, 8> squares;
  State state;
  Colour turn;
  std::array<int, 2> kingAttackers;
  std::unique_ptr<Coord> enPassantTarget;
  std::array<CastlingRights, 2> castlingRights;
  std::set<Move> moves;
  std::vector<Crumb> history;
  std::vector<Coord> changedCoords;
  static bool outOfBounds(Coord coord);
  // assumes coord is within bounds
  Square &squareAt(Coord coord);
  void attach(Square &subject, Coord observer);
  void detach(Square &subject, Coord observer);
  // assumes both observer and subject are within range
  void observe(Coord observer, Coord subject);
  // assumes there is a piece at dest and that moving from from to to is
  // pseudo-legal
  void addCapture(Square &from, Coord to);
  // NOTE: all tryAdd... functions assume from is within bounds
  // returns whether to is an empty square
  bool tryAddPawnAdvance(Coord from, Coord to);
  void tryAddPawnCapture(Coord from, Coord to);
  // returns whether to is an empty square
  bool tryAddMove(Coord from, Coord to);
  void tryAddLongRange(Coord from, Coord direction);
  void tryAddHoriVert(Coord from);
  void tryAddDiag(Coord from);
  // assumes coord is within bounds
  void update(Coord coord);
  // assumes coord is within bounds
  void notify(Coord coord);
  void tryUpdateEnPassantPawn(Coord coord);
  void notifyEnPassantTarget(Coord target);
  // does not update Board::moves
  // assumes move is pseudo-legal
  // returns changed coords
  std::vector<Coord> quickMove(const Move &move);
  // does not update Board::moves
  // returns changed coords
  std::vector<Coord> quickUndo();
  bool hasPiece(Square &square, Piece piece) const;
  void tryRetractCastlingRights(Colour colour);
  void updateMoves();
  void updateState();
public:
  Board();
  // assumes that if the a rook-king pair is in the initial position, then
  // the king has castling rights with that rook
  Board(const std::array<std::array<std::unique_ptr<Piece>, 8>, 8> &pieces, Colour turn);
  Board(const Board &);
  Board(Board &&);
  Board &operator=(const Board &);
  Board &operator=(Board &&);
  std::vector<Move> legalMoves() const;
  bool isLegalMove(const Move &move) const;
  State getState() const;
  bool gameOver() const;
  Colour getTurn() const;
  // returns the coords that were changed in the last move.
  // if there is no previous move, then this is empty.
  const std::vector<Coord> &getChangedCoords() const;
  // checks if there is a previous move by the same player to undo (i.e. there
  // are at least 2 moves in history)
  // NOTE: this is not the same as whether or not the function undo() can be
  // called. undo undoes a single move, but when a player uses the undo command
  // 2 moves need to be undone: the player's previous move and the opponent's
  // move following that.
  bool hasPriorMove() const;
  const Piece *at(int row, int col) const;
  void move(const Move &move);
  // NOTE: can also undo a resign
  void undo();
  // undoes twice in a single transaction
  void atomicUndo();
  void resign();
};

#endif
