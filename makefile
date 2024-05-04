.PHONY: deps clean libtest cleanall

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

IFLAGS := $(IGLEW) $(IGLFW) $(IGLM)
LFLAGS := $(LGLEW) $(LGLFW) $(LGLM)

CPPFLAGS := --std=c++17 -O3 -Wall -Wextra $(IFLAGS) $(LFLAGS)

SRC:= src

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
	#Done
	echo "Done building LPGL dependencies"

cleandeps:
	-make -C $(GLEWDIR) clean
	-make -C $(GLFWDIR)/build clean
	-make -C $(GLMDIR)/glm clean
	-make -C $(LUADIR) clean
	-rm -rf $(GLFWDIR)/build $(GLMDIR)/glm/build

clean:
	-rm -rf libtest

cleanall: cleandeps clean
