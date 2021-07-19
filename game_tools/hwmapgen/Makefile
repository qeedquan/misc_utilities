BIN=hwmapgen

all:
	cc -o $(BIN) *.c -lm -fstack-protector-all #-Wall -Wextra -fsanitize=address -ggdb -g3

clean:
	rm -f $(BIN)
