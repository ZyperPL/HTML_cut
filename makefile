.PHONY: all obj_folder clean

EXECUTABLE=html_cut
CPP=g++
CPP_FLAGS=-Wall -Wextra

all: $(EXECUTABLE)

obj/%.o: src/%.cpp
	$(CPP) -c -o $@ $< $(CPP_FLAGS)

$(EXECUTABLE): obj/main.o obj/inputfile.o obj/parser.o
	$(CPP) -o $(EXECUTABLE) obj/main.o obj/inputfile.o obj/parser.o -I.

clean:
	rm -rf obj/*.o
	rm $(EXECUTABLE)

obj_folder:
	mkdir -p obj
