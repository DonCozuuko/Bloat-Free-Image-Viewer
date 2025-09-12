main:
	g++ main.cpp windowsh-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

gui:
	g++ main.cpp windowsh-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows\

s:
	g++ main.cpp windowsh-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader ./assets/test-imgs/speed.png

# m:
# 	g++ -std=c++17 main.cpp windowsh-utils.cpp -o \
# 		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
# 		&& ./image-loader ./assets/test-imgs/markiplier-chins.jpg

m:
	g++ -std=c++17 main.cpp windowsh-utils.cpp -o \
		image-loader -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm\
		&& ./image-loader ./markiplier-chins.jpg