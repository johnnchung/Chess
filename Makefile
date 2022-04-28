CXXFLAGS = -std=c++14 -Wall -g

chess: action.o action_visitor.o board.o chess_display.o coord.o colour.o computer_player_1.o computer_player_2.o computer_player_3.o computer_player_4.o game.o graphic_display.o human_player.o main.o move.o piece.o player.o resign.o text_display.o undo.o window.o
	g++ $^ -lX11 -o $@

player.o: player.cc player.h action.h

action.o: action.cc action.h

action_visitor.o: action_visitor.cc action_visitor.h

board.o: board.cc board.h colour.h coord.h move.h piece.h piece_type.h action.h

chess_display.o: chess_display.cc chess_display.h

colour.o: colour.cc colour.h

computer_player_1.o: computer_player_1.cc computer_player_1.h player.h board.h move.h colour.h piece.h piece_type.h coord.h action.h

computer_player_2.o: computer_player_2.cc computer_player_2.h player.h board.h move.h colour.h piece.h piece_type.h coord.h action.h

computer_player_3.o: computer_player_3.cc computer_player_3.h player.h board.h move.h colour.h piece.h piece_type.h coord.h action.h

computer_player_4.o: computer_player_4.cc computer_player_4.h player.h board.h move.h colour.h piece.h piece_type.h coord.h action.h

coord.o: coord.cc coord.h

game.o: game.cc game.h move.h coord.h piece_type.h board.h colour.h piece.h chess_display.h window.h player.h action_visitor.h undo.h resign.h action.h

graphic_display.o: graphic_display.cc graphic_display.h board.h chess_display.h window.h piece_type.h

human_player.o: player.h board.h move.h colour.h piece.h piece_type.h action.h coord.h

main.o: main.cc move.h coord.h piece_type.h board.h colour.h piece.h chess_display.h text_display.h graphic_display.h window.h human_player.h computer_player_1.h computer_player_2.h computer_player_3.h computer_player_4.h game.h action.h

move.o: move.cc move.h coord.h piece_type.h action.h action_visitor.h

piece.o: piece.cc piece.h colour.h piece_type.h

resign.o: resign.cc resign.h action.h action_visitor.h

text_display.o: text_display.cc text_display.h board.h chess_display.h

undo.o: undo.cc undo.h action.h action_visitor.h

window.o: window.cc window.h
