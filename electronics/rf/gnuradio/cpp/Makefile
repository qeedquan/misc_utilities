SRC=$(wildcard *.cpp) 

CC=g++
CXXFLAGS=-Wall -Wextra -pedantic #-fsanitize=address
LDFLAGS=-lgnuradio-filter -lgnuradio-fft -lgnuradio-runtime -lgnuradio-blocks -lgnuradio-analog -lgnuradio-digital -llog4cpp -ldl

all:
	$(foreach file, $(SRC), $(CC) $(CXXFLAGS) -o $(subst .cpp,,$(file)) $(file) $(LDFLAGS);)

clean:
	$(foreach file, $(SRC), rm -f $(subst .cpp,,$(file));)
	rm -f *.bin
