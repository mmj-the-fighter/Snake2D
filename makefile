.PHONY: clean

c_files := $(wildcard *.c)
cpp_files := $(wildcard *.cpp)
c_obj := $(c_files:.c=.o)
cpp_obj := $(cpp_files:.cpp=.opp)

snek: $(c_obj) $(cpp_obj)
	g++ -o $@ -I. `pkg-config --libs --cflags sdl2` $^

%.o: %.c
	gcc -c $^ `pkg-config --cflags sdl2` -o $@

%.opp: %.cpp
	g++ -c $^ -Wno-write-strings `pkg-config --cflags sdl2` -o $@

clean:
	rm -f .o
