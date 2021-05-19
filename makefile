build:
	g++ main.cpp -lsfml-graphics -lsfml-window -lsfml-system -o game
debug:
	g++ -g main.cpp -lsfml-graphics -lsfml-window -lsfml-system -o game
	