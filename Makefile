# LAKY'S RAYLIB BUILDER MAKEFILE v1.0.0
# -------------------------------------

# Define variables
CXX = g++
INCLUDE = -Iinclude -Llib

SRC = $(wildcard src/*.cpp src/glad.c src/imgui/*.cpp src/imgui/backends/imgui_impl_raylib.cpp)

LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 
DEBUG_FLAGS = -g -std=c++17  -static-libgcc -static-libstdc++ -static -DCPPHTTPLIB_OPENSSL_SUPPORT # These will be used for the debug build (-g for debugging, -std=c++17 for C++17 standard)
RELEASE_FLAGS = -O2 -std=c++17 -static-libgcc -static-libstdc++ -static -DCPPHTTPLIB_OPENSSL_SUPPORT # These will be used for the release build (-O2 for optimization, -std=c++17 for C++17 standard)
DEBUG_OUT = minipuszta_debug.exe
RELEASE_OUT = minipuszta.exe

# Debug target
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: LDFLAGS = 
debug: $(DEBUG_OUT)

$(DEBUG_OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) $(LIBPATH) $(LIBS)  $(LDFLAGS) -o $(DEBUG_OUT)

# Release target
release: CXXFLAGS = $(RELEASE_FLAGS)
release: LDFLAGS = -mwindows
release: $(RELEASE_OUT)

$(RELEASE_OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SRC) $(LIBPATH) $(LIBS)  $(LDFLAGS) -o $(RELEASE_OUT)

# Clean target
clean:
	rm -f $(DEBUG_OUT) $(RELEASE_OUT)
