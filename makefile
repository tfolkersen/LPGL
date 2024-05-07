.PHONY: deps clean libtest cleanall compdb run

MAKETHREADS := 11
CXX := g++

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
LLUA := 

STBDIR := $(LIBDIR)/stb
ISTB := -isystem $(STBDIR)
LSTB := -L $(STBDIR) -lstb_image

IFLAGS := $(IGLEW) $(IGLFW) $(IGLM) $(ISTB)
LFLAGS := $(LGLEW) $(LGLFW) $(LGLM) $(LSTB)

CPPFLAGSBASE := --std=c++17 -O3 -Wall -Wextra
CPPFLAGS := $(CPPFLAGSBASE) $(IFLAGS) $(LFLAGS) -lGL

SRC := src

MAINOBJS := $(SRC)/glshader.o $(SRC)/utils.o

run: main
	./main

%.o: %.cpp %.h
	$(CXX) $< -o $@ $(CPPFLAGS) -c
	echo

%.o: %.cpp
	$(CXX) $< -o $@ $(CPPFLAGS) -c
	echo

main: $(SRC)/main.cpp $(MAINOBJS)
	$(CXX) $^ -o $@ $(CPPFLAGS)

cubetest: $(SRC)/cubetest.cpp
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
	make -C $(LUADIR) -j $(MAKETHREADS)
	#Build stb_image
	make -C $(STBDIR) -j $(MAKETHREADS) CXX=$(CXX) CPPFLAGS="$(CPPFLAGSBASE)"
	#Done
	echo "Done building LPGL dependencies"

cleandeps:
	-make -C $(GLEWDIR) clean
	-make -C $(GLFWDIR)/build clean
	-make -C $(GLMDIR)/glm clean
	-make -C $(LUADIR) clean
	-make -C $(STBDIR) clean
	-rm -rf $(GLFWDIR)/build $(GLMDIR)/glm/build

compdb:
	-rm compile_commands.json
	bear -- make main

clean:
	-rm -rf libtest cubetest $(SRC)/*.o

cleanall: cleandeps clean
