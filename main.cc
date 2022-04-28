#include <iostream>
#include <sstream>

#include "board.h"
#include "computer_player_1.h"
#include "computer_player_2.h"
#include "computer_player_3.h"
#include "computer_player_4.h"
#include "graphic_display.h"
#include "game.h"
#include "human_player.h"
#include "text_display.h"

std::istream &operator>>(std::istream &in, std::unique_ptr<Player> &player) {
  std::string playerString;
  if (!(in >> playerString)) return in;
  if (playerString[0] == 'h') {
    player = std::make_unique<HumanPlayer>();
  } else if (playerString[0] == 'c') {
    switch (playerString.back()) {
    case '1':
      player = std::make_unique<ComputerPlayer1>();
      break;
    case '2':
      player = std::make_unique<ComputerPlayer2>();
      break;
    case '3':
      player = std::make_unique<ComputerPlayer3>();
      break;
    case '4':
      player = std::make_unique<ComputerPlayer4>();
      break;
    default:
      in.setstate(std::istream::failbit);
      break;
    }
  } else {
    in.setstate(std::istream::failbit);
  }
  return in;
}

std::istream &operator>>(std::istream &in, Piece &piece) {
  char c;
  if (!(in >> c))return in;
  Colour colour = WHITE;
  if (c >= 'a' && c <= 'z') {
    colour = BLACK;
    c = c - 'a' + 'A';
  }
  PieceType type;
  switch (c) {
  case 'P':
    type = PAWN;
    break;
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
  case 'K':
    type = KING;
    break;
  default:
    in.setstate(std::istream::failbit);
    return in;
    break;
  }
  piece.colour = colour;
  piece.type = type;
  return in;
}

void setup(Board &board) {
  std::array<std::array<std::unique_ptr<Piece>, 8>, 8> pieces;
  Colour turn = WHITE;
  std::array<int, 2> numKings{ 0, 0 };
  std::string line;
  while(getline(std::cin, line)) {
    std::istringstream iss(std::move(line));
    std::string cmd;
    iss >> cmd;
    bool validCommand = false;
    switch (cmd[0]) {
      case '+': {
        Piece piece(BLACK, PAWN);
        Coord coord(0, 0);
        if (iss >> piece >> coord) {
          const Piece *origPiece = pieces[coord.row][coord.col].get();
          if (origPiece && origPiece->type == KING) {
            --numKings[origPiece->colour];
          }
          if (piece.type == KING) {
            ++numKings[piece.colour];
          }
          pieces[coord.row][coord.col] = std::make_unique<Piece>(piece);
          validCommand = true;
        }
      } break;
      case '-': {
        Coord coord(0, 0);
        if (iss >> coord) {
          const Piece *piece = pieces[coord.row][coord.col].get();
          if (piece && piece->type == KING) {
            --numKings[piece->colour];
          }
          pieces[coord.row][coord.col].reset();
          validCommand = true;
        }
      } break;
      case '=': {
        std::string colour;
        if (iss >> colour) {
          if (colour == "white") {
            validCommand = true;
            turn = WHITE;
          } else if (colour == "black") {
            validCommand = true;
            turn = BLACK;
          }
        }
      } break;
      case 'd': { // done
        validCommand = true;
        if (numKings[WHITE] != 1 || numKings[BLACK] != 1) {
          std::cout << "Must have exactly one king of each colour." << std::endl;
          continue;
        }
        bool pawnsValid = true;
        for (int col = 0; col < 8; ++col) {
          const Piece *piece = pieces[7][col].get();
          if (piece && piece->type == PAWN) {
            pawnsValid = false;
            break;
          }
          piece = pieces[0][col].get();
          if (piece && piece->type == PAWN) {
            pawnsValid = false;
            break;
          }
        }
        if (!pawnsValid) {
          std::cout << "Cannot have pawns in the first or last row." << std::endl;
          continue;
        }
        Board tmpBoard(pieces, turn);
        if (tmpBoard.getState() == Board::CHECK || tmpBoard.getState() == Board::CHECKMATE) {
          std::cout << "Kings must not be in check." << std::endl;
          continue;
        }
        board = tmpBoard;
        std::cout << "Setup complete." << std::endl;
        return;
      };
    }
    if (!validCommand) {
      std::cout << "Invalid setup command." << std::endl;
    }
  }
}

int main() {
  Board board;
  std::string line;
  std::array<int, 2> halfPoints{ 0, 0 };
  while (getline(std::cin, line)) {
    std::istringstream iss(std::move(line));
    std::string cmd;
    iss >> cmd;
    bool validCommand = false;
    switch (cmd[0]) {
      case 'g': {
        std::unique_ptr<Player> white, black;
        if (iss >> white >> black) {
          validCommand = true;
          std::vector<std::unique_ptr<ChessDisplay>> displays;
          displays.push_back(std::make_unique<TextDisplay>());
          displays.push_back(std::make_unique<GraphicDisplay>());
          Game game(std::move(board), std::move(white), std::move(black), std::move(displays));
          switch (game.run()) {
          case Game::CHECKMATE_WHITE_WINS:
          case Game::BLACK_RESIGNED:
            halfPoints[WHITE] += 2;
            break;
          case Game::CHECKMATE_BLACK_WINS:
          case Game::WHITE_RESIGNED:
            halfPoints[BLACK] += 2;
            break;
          case Game::STALEMATE:
            ++halfPoints[WHITE];
            ++halfPoints[BLACK];
            break;
          }
          board = Board();
        }
      } break;
      case 's': {
        validCommand = true;
        setup(board);
      } break;
    }
    if (!validCommand) {
      std::cout << "Invalid command." << std::endl;
    }
  }
  std::array<int, 2> fullPoints{ halfPoints[BLACK] / 2, halfPoints[WHITE] / 2 };
  std::cout << "Final Score:" << std::endl;
  std::cout << "White: " << fullPoints[WHITE];
  if (halfPoints[WHITE] % 2) std::cout << " 1/2";
  std::cout << std::endl;
  std::cout << "Black: " << fullPoints[BLACK];
  if (halfPoints[BLACK] % 2) std::cout << " 1/2";
  std::cout << std::endl;
}
