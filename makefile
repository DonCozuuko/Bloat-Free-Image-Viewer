main:
	g++ main.cpp cursor-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader

gui:
	g++ main.cpp cursor-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows\

# x:
# 	g++ main.cpp cursor-pos.cpp -o \
# 		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\

s:
	g++ main.cpp cursor-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader ./assets/test-imgs/speed.png

m:
	g++ -std=c++17 main.cpp cursor-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader ./assets/test-imgs/markiplier-chins.jpg