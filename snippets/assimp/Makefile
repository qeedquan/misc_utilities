all:
	g++ -o test-load test-load.cpp `pkg-config --cflags --libs assimp` -fsanitize=address -ggdb -g3 -Wall -Wextra -pedantic

clean:
	rm -f test-load
