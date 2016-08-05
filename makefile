.PHONY: all obj_folder clean

EXECUTABLE=html_cut
CPP=g++
CPP_FLAGS=-Wall -Wextra

all: $(EXECUTABLE)

obj/main.o: obj_folder src/main.cpp
	$(CPP) -c -o $@ src/main.cpp $(CPP_FLAGS)

obj/inputfile.o: obj_folder src/inputfile.cpp src/inputfile.h
	$(CPP) -c -o $@ src/inputfile.cpp $(CPP_FLAGS)

$(EXECUTABLE): obj/main.o obj/inputfile.o
	$(CPP) -o $(EXECUTABLE) obj/main.o obj/inputfile.o -I.

clean:
	rm -rf obj/*.o
	rm $(EXECUTABLE)

obj_folder:
	mkdir -p obj
