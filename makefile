main:
	g++ main.cpp windowsh-utils.cpp -o \
		bfiv -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

gui:
	g++ main.cpp windowsh-utils.cpp -o \
		bfiv -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows\

t:
	g++ test.cpp -o test -std=c++17