buildrun:
	make build
	make run
build:
	g++ src/*.cpp -Isrc -std=c++17 -o bin/sfmlgame -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
run:
	./bin/sfmlgame
test:
	g++ tests/*.cpp `ls src/*.cpp | grep -v main.cpp` -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o bin/test
	./bin/test
