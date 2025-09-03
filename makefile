main:
	g++ main.cpp cursor-pos.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader

gui:
	g++ main.cpp cursor-pos.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows\
		&& ./image-loader

x:
	g++ main.cpp cursor-pos.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\

s:
	g++ main.cpp cursor-pos.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader speed.png