#include <cmath>
#include <stdexcept>
#include <utility>

#include "board.h"

Board::Square::Square() : attacksKing{ false, false } {}

Board::Square::Square(const Square &other)
  : piece{ other.piece ? std::make_unique<Piece>(*other.piece) : nullptr }
  , observers{ other.observers }
  , subjects{ other.subjects }
  , moves{ other.moves }
  , attacksKing{ other.attacksKing }
{}

Board::Square::Square(Square &&other) = default;

Board::Square &Board::Square::operator=(const Square &other) {
  if (this == &other) return *this;
  Square copy = other;
  std::swap(*this, copy);
  return *this;
}

Board::Square &Board::Square::operator=(Square &&other) = default;

Board::CastlingRights::CastlingRights() : queenSide{ true }, kingSide{ true } {}

bool Board::CastlingRights::operator==(CastlingRights other) const {
  return kingSide == other.kingSide && queenSide == other.queenSide;
}

bool Board::CastlingRights::operator!=(CastlingRights other) const {
  return !(*this == other);
}

Board::Capture::Capture(Piece piece, Coord location)
  : piece{ piece }
  , location{ location }
{}

Board::Crumb::Crumb(const Move &move, std::unique_ptr<Capture> capture,
    std::unique_ptr<Coord> enPassantTarget,
    std::array<CastlingRights, 2> castlingRights)
  : move{ move }
  , capture{ std::move(capture) }
  , enPassantTarget{ std::move(enPassantTarget) }
  , castlingRights{ castlingRights }
{}

Board::Crumb::Crumb(const Crumb &other)
  : move{ other.move }
  , capture{ other.capture ? std::make_unique<Capture>(*other.capture) : nullptr }
  , enPassantTarget{ other.enPassantTarget ? std::make_unique<Coord>(*other.enPassantTarget) : nullptr }
  , castlingRights{ other.castlingRights }
{}

Board::Crumb::Crumb(Crumb &&other) = default;

Board::Crumb &Board::Crumb::operator=(const Crumb &other) {
  if (this == &other) return *this;
  Crumb copy = other;
  std::swap(*this, copy);
  return *this;
}

Board::Crumb &Board::Crumb::operator=(Crumb &&other) = default;

bool Board::outOfBounds(Coord coord) {
  return coord.row < 0 || coord.row >= 8 || coord.col < 0 || coord.col >= 8;
}

Board::Square &Board::squareAt(Coord coord) {
  return squares[coord.row][coord.col];
}

void Board::attach(Square &subject, Coord observer) {
  subject.observers.insert(observer);
}

void Board::detach(Square &subject, Coord observer) {
  subject.observers.erase(observer);
}

void Board::observe(Coord observer, Coord subject) {
  attach(squareAt(subject), observer);
  squareAt(observer).subjects.push_back(subject);
}

void Board::addCapture(Square &from, Coord to) {
  Piece target = *squareAt(to).piece;
  if (target.type == KING) from.attacksKing[target.colour] = true;
  from.moves.push_back(to);
}

bool Board::tryAddPawnAdvance(Coord from, Coord to) {
  if (outOfBounds(to)) return false;
  observe(from, to);
  if (squareAt(to).piece) return false;
  squareAt(from).moves.push_back(to);
  return true;
}

void Board::tryAddPawnCapture(Coord from, Coord to) {
  if (outOfBounds(to)) return;
  observe(from, to);
  Square &origin = squareAt(from), &dest = squareAt(to);
  if (dest.piece && dest.piece->colour != origin.piece->colour) {
    addCapture(origin, to);
  } else {
    // check for en passant
    Coord target(from.row, to.col);
    if (enPassantTarget && *enPassantTarget == target
        && squareAt(target).piece->colour != origin.piece->colour) {
      origin.moves.push_back(to);
    } else {
      // watch for an en passant
      // IDEA: could refine further to only watch under more specific
      // circumstances
      observe(from, target);
    }
  }
}

bool Board::tryAddMove(Coord from, Coord to) {
  if (outOfBounds(to)) return false;
  observe(from, to);
  Square &origin = squareAt(from), &dest = squareAt(to);
  if (dest.piece) {
    if (dest.piece->colour != origin.piece->colour) addCapture(origin, to);
    return false;
  } else {
    origin.moves.push_back(to);
    return true;
  }
}

void Board::tryAddLongRange(Coord from, Coord direction) {
  for (Coord to = from + direction; tryAddMove(from, to); to += direction);
}

void Board::tryAddHoriVert(Coord from) {
  tryAddLongRange(from, Coord(0, 1));
  tryAddLongRange(from, Coord(0, -1));
  tryAddLongRange(from, Coord(1, 0));
  tryAddLongRange(from, Coord(-1, 0));
}

void Board::tryAddDiag(Coord from) {
  tryAddLongRange(from, Coord(1, 1));
  tryAddLongRange(from, Coord(1, -1));
  tryAddLongRange(from, Coord(-1, 1));
  tryAddLongRange(from, Coord(-1, -1));
}

void Board::update(Coord coord) {
  int row = coord.row, col = coord.col;
  Square &square = squares[row][col];

  // detach from all subjects and clear the subjects vector, will reattach
  // if still interested
  for (Coord subject : square.subjects) {
    detach(squareAt(subject), coord);
  }
  square.subjects.clear();

  // clear moves, will reconstruct based on current state of the board
  square.moves.clear();

  std::array<bool, 2> attackedKing = square.attacksKing;
  square.attacksKing[BLACK] = false;
  square.attacksKing[WHITE] = false;

  const Piece *piece = square.piece.get();
  if (piece) {
    switch (piece->type) {
      case PAWN: {
        int direction = piece->colour == WHITE ? 1 : -1;
        int initRow = piece->colour == WHITE ? 1 : 6;
        if (tryAddPawnAdvance(coord, Coord(row + direction, col))
            && row == initRow)
          tryAddPawnAdvance(coord, Coord(row + 2 * direction, col));
        tryAddPawnCapture(coord, Coord(row + direction, col - 1));
        tryAddPawnCapture(coord, Coord(row + direction, col + 1));
      } break;
      case ROOK: {
        tryAddHoriVert(coord);
      } break;
      case KNIGHT: {
        tryAddMove(coord, Coord(row + 1, col + 2));
        tryAddMove(coord, Coord(row + 2, col + 1));
        tryAddMove(coord, Coord(row + 1, col - 2));
        tryAddMove(coord, Coord(row + 2, col - 1));
        tryAddMove(coord, Coord(row - 1, col + 2));
        tryAddMove(coord, Coord(row - 2, col + 1));
        tryAddMove(coord, Coord(row - 1, col - 2));
        tryAddMove(coord, Coord(row - 2, col - 1));
      } break;
      case BISHOP: {
        tryAddDiag(coord);
        tryAddDiag(coord);
      } break;
      case QUEEN: {
        tryAddHoriVert(coord);
        tryAddDiag(coord);
      } break;
      case KING: {
        for (int rowDirection = -1; rowDirection <= 1; ++rowDirection) {
          for (int colDirection = -1; colDirection <= 1; ++colDirection) {
            if (rowDirection != 0 || colDirection != 0) {
              tryAddMove(coord, Coord(row + rowDirection, col + colDirection));
            }
          }
        }
        if (castlingRights[piece->colour].queenSide) {
          observe(coord, Coord(row, 0));
          observe(coord, Coord(row, 1));
          observe(coord, Coord(row, 2));
          // we should already be observing (row, 3)
          if (!at(row, 1) && !at(row, 2) && !at(row, 3)) {
            square.moves.emplace_back(row, 2);
          }
        }
        if (castlingRights[piece->colour].kingSide) {
          // we should already be observing (row, 5)
          observe(coord, Coord(row, 6));
          observe(coord, Coord(row, 7));
          if (!at(row, 5) && !at(row, 6)) {
            square.moves.emplace_back(row, 6);
          }
        }
      } break;
    }
  }

  for (int colour = BLACK; colour <= WHITE; ++colour) {
    if (!attackedKing[colour] && square.attacksKing[colour]) {
      ++kingAttackers[colour];
    } else if (attackedKing[colour] && !square.attacksKing[colour]) {
      --kingAttackers[colour];
    }
  }
}

void Board::notify(Coord coord) {
  Square &square = squareAt(coord);

  // moves observers out to temp value, observers now empty, those who are
  // still interested will reattach
  for (Coord observer : std::set<Coord>(std::move(square.observers))) {
    update(observer);
  }

  // a square is always observing itself
  update(coord);
}

void Board::tryUpdateEnPassantPawn(Coord coord) {
  if (outOfBounds(coord)) return;
  Square &square = squareAt(coord);
  if (!square.piece) return;
  Piece piece = *square.piece;
  if (piece.colour == turn && piece.type == PAWN) update(coord);
}

void Board::notifyEnPassantTarget(Coord target) {
  tryUpdateEnPassantPawn(Coord(target.row, target.col - 1));
  tryUpdateEnPassantPawn(Coord(target.row, target.col + 1));
}

std::vector<Coord> Board::quickMove(const Move &move) {
  Coord from = move.from, to = move.to;
  Square &origin = squareAt(from), &dest = squareAt(to);
  Piece piece = *origin.piece;

  std::unique_ptr<Capture> capture;
  if (dest.piece) capture = std::make_unique<Capture>(*dest.piece, to);

  dest.piece = std::move(origin.piece);

  // promotion
  if (move.promoteTo != PAWN) dest.piece->type = move.promoteTo;

  // save old en passant target before overwriting it
  std::unique_ptr<Coord> oldEnPassantTarget = std::move(enPassantTarget);

  std::unique_ptr<Move> rookCastling;

  if (piece.type == PAWN) {
    if (std::abs(to.row - from.row) == 2) {
      // en passant target
      enPassantTarget = std::make_unique<Coord>(to);
    } else if (to.col != from.col && !capture) {
      // en passant
      // NOTE: enPassantTarget is moved from, cannot refer to it here
      Coord target(from.row, to.col);
      capture = std::make_unique<Capture>(*squareAt(target).piece, 
        target);
      squareAt(target).piece.reset();
    }
  } else if (piece.type == KING && std::abs(to.col - from.col) == 2) {
    // castling, need to move rook
    if (to.col == 2) {
      rookCastling = std::make_unique<Move>(Coord(to.row, 0), Coord(to.row, 3));
    } else { // to.col == 6
      rookCastling = std::make_unique<Move>(Coord(to.row, 7), Coord(to.row, 5));
    }
    squareAt(rookCastling->to).piece = std::move(squareAt(rookCastling->from).piece);
  }

  // save old castling rights before we modify it
  std::array<CastlingRights, 2> oldCastlingRights = castlingRights;

  // update castling rights
  if ((turn == WHITE && from.row == 0) || (turn == BLACK && from.row == 7)) {
    if (from.col == 4 || from.col == 0) castlingRights[turn].queenSide = false;
    if (from.col == 4 || from.col == 7) castlingRights[turn].kingSide = false;
  }

  // update pawns next to old en passant target
  // NOTE: this depends on turn so we have to do this before updating turn
  // NOTE: need to do this before moving oldEnPassantTarget into crumb
  if (oldEnPassantTarget) notifyEnPassantTarget(*oldEnPassantTarget);

  // notify changed squares
  // NOTE: must do this before moving capture into crumb
  std::vector<Coord> changed;
  changed.push_back(from);
  changed.push_back(to);
  if (capture && capture->location != to) changed.push_back(capture->location);
  if (rookCastling) {
    changed.push_back(rookCastling->from);
    changed.push_back(rookCastling->to);
  }
  for (Coord coord : changed) {
    notify(coord);
  }

  // push crumb onto history
  history.emplace_back(move, std::move(capture), std::move(oldEnPassantTarget),
    oldCastlingRights);
  
  // update turn
  turn = !turn;

  return changed;
}

std::vector<Coord> Board::quickUndo() {
  Crumb &crumb = history.back();

  const Move &move = crumb.move;
  Coord from = move.from, to = move.to;
  Square &origin = squareAt(from), &dest = squareAt(to);

  // undo promotion
  if (move.promoteTo != PAWN) dest.piece->type = PAWN;

  // move piece back
  origin.piece = std::move(dest.piece);

  // restore captured piece
  if (crumb.capture) {
    Square &location = squareAt(crumb.capture->location);
    location.piece = std::make_unique<Piece>(crumb.capture->piece);
  }

  // undo castling (move of rook)
  std::unique_ptr<Move> rookCastling;
  if (origin.piece->type == KING && std::abs(to.col - from.col) == 2) {
    if (to.col == 2) {
      rookCastling = std::make_unique<Move>(Coord(to.row, 0), Coord(to.row, 3));
    } else { // to.col == 6
      rookCastling = std::make_unique<Move>(Coord(to.row, 7), Coord(to.row, 5));
    }
    squareAt(rookCastling->from).piece = std::move(squareAt(rookCastling->to).piece);
  }

  // move en passant target out before we overwrite it
  std::unique_ptr<Coord> newEnPassantTarget = std::move(enPassantTarget);

  // save new castling rights for comparison
  std::array<CastlingRights, 2> newCastlingRights = castlingRights;

  // restore en passant target and castling rights
  enPassantTarget = std::move(crumb.enPassantTarget);
  castlingRights = crumb.castlingRights;

  // update new en passant pawns so they lose en passant move
  // NOTE: must do this before flipping turn since tryUpdateEnPassantPawn
  // depends on turn
  if (newEnPassantTarget) notifyEnPassantTarget(*newEnPassantTarget);

  turn = !turn;

  // update current en passant pawns so they regain en passant move
  if (enPassantTarget) notifyEnPassantTarget(*enPassantTarget);

  // notify changed squares
  std::vector<Coord> changed;
  changed.push_back(from);
  changed.push_back(to);
  if (crumb.capture) changed.push_back(crumb.capture->location);
  if (rookCastling) {
    changed.push_back(rookCastling->from);
    changed.push_back(rookCastling->to);
  }
  for (Coord coord : changed) {
    notify(coord);
  }

  // if castling rights different update king
  if (castlingRights[BLACK] != newCastlingRights[BLACK]) update(Coord(0, 4));
  if (castlingRights[WHITE] != newCastlingRights[WHITE]) update(Coord(7, 4));

  // NOTE: must do this at the end since crumb is a reference to history.back()
  history.pop_back();

  return changed;
}

bool Board::hasPiece(Square &square, Piece piece) const {
  if (!square.piece) return false;
  return square.piece->colour == piece.colour && square.piece->type == piece.type;
}

void Board::tryRetractCastlingRights(Colour colour) {
  int row = colour == WHITE ? 0 : 7;
  if (!hasPiece(squares[row][4], Piece(colour, KING))
      || !hasPiece(squares[row][0], Piece(colour, ROOK))) {
    castlingRights[colour].queenSide = false;
  }
  if (!hasPiece(squares[row][4], Piece(colour, KING))
      || !hasPiece(squares[row][7], Piece(colour, ROOK))) {
    castlingRights[colour].kingSide = false;
  }
}

void Board::updateMoves() {
  moves.clear();
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      Square &square = squares[row][col];
      if (!square.piece || square.piece->colour != turn) continue;
      // need to save this to check the type of piece later, when piece will
      // have moved from square
      Piece piece = *square.piece;
      // must copy square.moves into temp value since executing the move
      // might modify the moves vector of the square, invalidating iterator
      for (Coord to : std::vector<Coord>(square.moves)) {
        // NOTE: we ignore promotion here since the choice of promotion
        // is not relevant to the legality of a move
        quickMove(Move(Coord(row, col), to));
        // turn is flipped since quickMove changes whose turn it is
        if (!kingAttackers[!turn]) {
          Coord from(row, col);
          if (piece.type == PAWN && (to.row == 0 || to.row == 7)) {
            moves.emplace(from, to, ROOK);
            moves.emplace(from, to, KNIGHT);
            moves.emplace(from, to, BISHOP);
            moves.emplace(from, to, QUEEN);
          } else {
            moves.emplace(from, to);
          }
        }
        quickUndo();
      }
    }
  }
}

void Board::updateState() {
  if (kingAttackers[turn]) {
    if (moves.size()) {
      state = CHECK;
    } else {
      state = CHECKMATE;
    }
  } else {
    if (moves.size()) {
      // stalemate if only two pieces (the two kings) left
      // otherwise normal
      int numPieces = 0;
      for(int row = 0; row < 8; ++row) {
        for(int col = 0; col < 8; ++col) {
          if (squares[row][col].piece) ++numPieces;
        }
      }
      if(numPieces == 2) {
        state = STALEMATE;
      } else {
        state = NORMAL;
      }
    } else {
      state = STALEMATE;
    };
  }
}

Board::Board() : state{ NORMAL } , turn{ WHITE } , kingAttackers{ 0, 0 } {
  for (int col = 0; col < 8; ++col) {
    squares[1][col].piece = std::make_unique<Piece>(WHITE, PAWN);
    squares[6][col].piece = std::make_unique<Piece>(BLACK, PAWN);
  }
  squares[0][0].piece = std::make_unique<Piece>(WHITE, ROOK);
  squares[0][1].piece = std::make_unique<Piece>(WHITE, KNIGHT);
  squares[0][2].piece = std::make_unique<Piece>(WHITE, BISHOP);
  squares[0][3].piece = std::make_unique<Piece>(WHITE, QUEEN);
  squares[0][4].piece = std::make_unique<Piece>(WHITE, KING);
  squares[0][5].piece = std::make_unique<Piece>(WHITE, BISHOP);
  squares[0][6].piece = std::make_unique<Piece>(WHITE, KNIGHT);
  squares[0][7].piece = std::make_unique<Piece>(WHITE, ROOK);
  squares[7][0].piece = std::make_unique<Piece>(BLACK, ROOK);
  squares[7][1].piece = std::make_unique<Piece>(BLACK, KNIGHT);
  squares[7][2].piece = std::make_unique<Piece>(BLACK, BISHOP);
  squares[7][3].piece = std::make_unique<Piece>(BLACK, QUEEN);
  squares[7][4].piece = std::make_unique<Piece>(BLACK, KING);
  squares[7][5].piece = std::make_unique<Piece>(BLACK, BISHOP);
  squares[7][6].piece = std::make_unique<Piece>(BLACK, KNIGHT);
  squares[7][7].piece = std::make_unique<Piece>(BLACK, ROOK);

  // update each square with a piece
  for (int col = 0; col < 8; ++col) {
    update(Coord(0, col));
    update(Coord(1, col));
    update(Coord(6, col));
    update(Coord(7, col));
  }

  updateMoves();
}

Board::Board(const Board &other)
  : squares{ other.squares }
  , state{ other.state }
  , turn{ other.turn }
  , kingAttackers{ other.kingAttackers }
  , enPassantTarget{ other.enPassantTarget ? std::make_unique<Coord>(*other.enPassantTarget) : nullptr }
  , castlingRights{ other.castlingRights }
  , moves{ other.moves }
  , history{ other.history }
{}

Board::Board(Board &&other) = default;

Board &Board::operator=(const Board &other) {
  if (this == &other) return *this;
  Board copy = other;
  std::swap(*this, copy);
  return *this;
}

Board &Board::operator=(Board &&other) = default;

Board::Board(const std::array<std::array<std::unique_ptr<Piece>, 8>, 8> &pieces, Colour turn)
  : turn{ turn }
  , kingAttackers{ 0, 0 }
{
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      if (pieces[row][col]) {
        squares[row][col].piece = std::make_unique<Piece>(*pieces[row][col]);
      }
    }
  }

  // have to do this before updating pieces bc otherwise king would add
  // castling move
  tryRetractCastlingRights(WHITE);
  tryRetractCastlingRights(BLACK);

  // update each square with a piece
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      if (squares[row][col].piece) update(Coord(row, col));
    }
  }

  updateMoves();
  updateState();

  // updateState only checks kingAttackers[turn], but all bets are off
  // on custom board config
  // we will set state to CHECK if !turn is checked and state is not already
  // CHECK or CHECKMATE to let setup know
  if (state != CHECK && state != CHECKMATE && kingAttackers[!turn]) {
    state = CHECK;
  }
}

std::vector<Move> Board::legalMoves() const {
  return std::vector<Move>(moves.begin(), moves.end());
}

bool Board::isLegalMove(const Move &move) const {
  return moves.count(move);
}

const Piece *Board::at(int row, int col) const {
  if (outOfBounds(Coord(row, col))) throw std::out_of_range("Coordinates out of range.");
  return squares[row][col].piece.get();
}

bool Board::hasPriorMove() const {
  return history.size() >= 2;
}

void Board::move(const Move &move) {
  if (!isLegalMove(move)) throw std::logic_error("Illegal move.");
  // NOTE: if game is over then moves should be empty so the above covers it
  changedCoords = quickMove(move);
  updateMoves();
  updateState();
}

Board::State Board::getState() const {
  return state;
}

bool Board::gameOver() const {
  return state == CHECKMATE || state == STALEMATE || state == RESIGNED;
}

Colour Board::getTurn() const {
  return turn;
}

const std::vector<Coord> &Board::getChangedCoords() const {
  return changedCoords;
}

void Board::undo() {
  if (history.empty()) throw std::logic_error("No move to undo.");
  if (state == RESIGNED) {
    // if previous action was resign, then we simply flip turn and updateMoves
    // and updateState will take care of the rest
    turn = !turn;
  } else {
    changedCoords = quickUndo();
  }
  updateMoves();
  updateState();
}

void Board::atomicUndo() {
  if (!hasPriorMove()) throw std::logic_error("No prior move to undo.");
  if (state == RESIGNED) {
    turn = !turn;
  } else {
    changedCoords = quickUndo();
  }
  std::vector<Coord> changed = quickUndo();
  changedCoords.insert(changedCoords.end(), changed.begin(), changed.end());
  updateMoves();
  updateState();
}

void Board::resign() {
  if (gameOver()) throw std::logic_error("Game already over.");
  turn = !turn;
  state = RESIGNED;
  moves.clear();
  // this is not strictly necessary but makes sense
  enPassantTarget.reset();
}
