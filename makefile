.PHONY: deps clean libtest cleanall compdb run cxxtargets cxxtargets_em

MAKETHREADS := 11
CXX := g++
CXX_EM := em++

LIBDIR := libs


GLEWDIR := $(LIBDIR)/glew-2.1.0
IGLEW := -isystem $(GLEWDIR)/include
LGLEW := -L $(GLEWDIR)/lib -Wl,-rpath,$(GLEWDIR)/lib -lGLEW

GLFWDIR := $(LIBDIR)/glfw-3.4
IGLFW := -isystem $(GLFWDIR)/include
LGLFW := -L $(GLFWDIR)/build/src -Wl,-rpath,$(GLFWDIR)/build/src -lglfw

GLMDIR := $(LIBDIR)/glm
IGLM := -isystem $(GLMDIR)
LGLM :=

LUADIR := $(LIBDIR)/lua-5.4.6
ILUA := -isystem $(LUADIR)/src
LLUA := -L $(LUADIR)/src -llua

LUADIR_EM := $(LIBDIR)/lua-5.4.6-em
ILUA_EM := -isystem $(LUADIR_EM)/src
LLUA_EM := -L $(LUADIR_EM)/src -llua

STBDIR := $(LIBDIR)/stb
ISTB := -isystem $(STBDIR)
LSTB := -L $(STBDIR)/gcc -lstb_image
ISTB_EM := $(ISTB)
LSTB_EM := -L $(STBDIR)/emcc -lstb_image

IFLAGSBASE := $(IGLM)
LFLAGSBASE := $(LGLM)

IFLAGS := $(IFLAGSBASE) $(IGLEW) $(IGLFW) $(ISTB) $(ILUA)
LFLAGS := $(LFLAGSBASE) $(LGLEW) $(LGLFW) $(LSTB) $(LLUA)

IFLAGS_EM := $(IFLAGSBASE) $(ISTB_EM) $(ILUA_EM)
LFLAGS_EM := $(LFLAGSBASE) $(LSTB_EM) $(LLUA_EM)

CPPFLAGSBASE := --std=c++17 -O3 -Wall -Wextra -lGL
CPPFLAGS := $(CPPFLAGSBASE) $(IFLAGS) $(LFLAGS)

CPPFLAGSBASE_EM := $(CPPFLAGSBASE) --preload-file files --preload-file shaders -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sUSE_WEBGL2=1 -sUSE_GLFW=3
CPPFLAGS_EM := $(CPPFLAGSBASE_EM) $(IFLAGS_EM) $(LFLAGS_EM) 

SRC := src

MAINOBJS := $(SRC)/glshader.o $(SRC)/utils.o $(SRC)/gltexture.o $(SRC)/glframebuffer.o $(SRC)/lpglctx.o
MAINOBJS_EM := $(SRC)/glshader.oe $(SRC)/utils.oe $(SRC)/gltexture.oe $(SRC)/glframebuffer.oe $(SRC)/lpglctx.oe

run: cxxtargets #emxxtargets
	./main

cxxtargets: main
emxxtargets: main.html


all: main cubetest2

%.o: %.cpp %.h
	$(CXX) $< -o $@ $(CPPFLAGS) -c
	echo

%.o: %.cpp
	$(CXX) $< -o $@ $(CPPFLAGS) -c
	echo

%.oe: %.cpp %.h
	$(CXX_EM) $< -o $@ $(CPPFLAGS_EM) -c
	echo

%.oe: %.cpp
	$(CXX_EM) $< -o $@ $(CPPFLAGS_EM) -c
	echo

main.html: $(SRC)/main.cpp $(MAINOBJS_EM)
	$(CXX_EM) $^ -o $@ $(CPPFLAGS_EM)

main: $(SRC)/main.cpp $(MAINOBJS)
	$(CXX) $^ -o $@ $(CPPFLAGS)

cubetest: $(SRC)/cubetest.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS)

cubetest2: $(SRC)/cubetest2.cpp $(MAINOBJS)
	$(CXX) $^ -o $@ $(CPPFLAGS)

libtest:
	-rm libtest
	clear
	$(CXX) $(SRC)/libtest.cpp -o libtest $(CPPFLAGS)
	./libtest

deps:
	#Build GLEW
	make -C $(GLEWDIR) -j $(MAKETHREADS)
	#Build GLFW
	cmake -B $(GLFWDIR)/build -S $(GLFWDIR) -DBUILD_SHARED_LIBS=ON
	make -C $(GLFWDIR)/build -j $(MAKETHREADS)
	#Build glm
	cmake -B $(GLMDIR)/glm/build -S $(GLMDIR)/glm
	make -C $(GLMDIR)/glm/build -j $(MAKETHREADS)
	echo "#warning glm dummy file found" > $(GLMDIR)/glm/dummy.hpp
	#Build Lua
	make -C $(LUADIR)/src -j $(MAKETHREADS)
	make -C $(LUADIR_EM)/src -j $(MAKETHREADS) CC=" emcc -std=gnu99"
	#Build stb_image
	make -C $(STBDIR) -j $(MAKETHREADS) CXX=$(CXX) CXX_EM=$(CXX_EM) CPPFLAGS="$(CPPFLAGSBASE)" CPPFLAGS_EM="$(CPPFLAGSBASE_EM)"
	#Done
	echo "Done building LPGL dependencies"

cleandeps:
	-make -C $(GLEWDIR) clean
	-make -C $(GLFWDIR)/build clean
	-make -C $(GLMDIR)/glm clean
	-make -C $(LUADIR)/src clean
	-make -C $(LUADIR_EM)/src clean
	-make -C $(STBDIR) clean
	-rm -rf $(GLFWDIR)/build $(GLMDIR)/glm/build

compdb:
	-rm compile_commands.json
	bear -- make cxxtargets

clean:
	-rm -rf libtest cubetest cubetest2 main $(SRC)/*.o $(SRC)/*.oe *.data *.wasm *.js *.html

cleanall: cleandeps clean
