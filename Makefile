CPP_FILES = equation.cpp face.cpp

O_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


all: $(O_FILES)

$(O_FILES): %.o: %.cpp
	g++ $^ -c -o $@ -std=c++0x

test:
	g++ arrtest.cpp -o arrtest -std=c++0x -Wall

clean:
	rm *.o -f

