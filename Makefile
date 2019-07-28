
PROG_NAME = sdl2.elf

SRC_FILES = \
src/main.cpp \
src/gui/window.cpp \
src/gui/event.cpp \
src/scene.cpp \
src/graphics/shader_program.cpp \
src/util/stl_parser.cpp \
src/graphics/camera.cpp

CXX_FLAGS = \
-m32 \
-O2 \
-Wall \
-Werror \
-std=c++17 \
-DGLEW_NO_GLU

INC = \
-Isrc \
-Ilib/glm-0.9.9.5 \
-Ilib/tinyobjloader-1.0.7 \
-I/usr/include/SDL2
LIB = \

LINK = \
-lSDL2 \
-lGLEW \
-lGL

all:
	mkdir -p build
	mkdir -p build/models
	cp -rf res/* build/models
	mkdir -p build/shaders
	cp -rf src/shaders/* build/shaders
	g++ -o build/${PROG_NAME} ${CXX_FLAGS} ${INC} ${LIB} ${LINK} ${SRC_FILES}

clean:
	rm -rf build/*
