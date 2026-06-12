# Run executables from the project root so data/ and assets/ paths resolve.
# Example: ./bin/app1.exe adaugare_curs "Titlu" "Desc" 99

CXX      = g++
CXXFLAGS = -std=c++23 -Wall -Wextra
BUILDDIR = build
BINDIR   = bin

IMGUI_DIR = imgui
IMGUI_SRC = $(wildcard $(IMGUI_DIR)/imgui*.cpp)
IMGUI_OBJ = $(patsubst $(IMGUI_DIR)/%.cpp, $(BUILDDIR)/imgui_%.o, $(IMGUI_SRC))
IMGUI_INC = -I $(IMGUI_DIR) -I C:/msys64/mingw64/include -I C:/msys64/mingw64/include/SDL2
GUI_LIBS  = -L C:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32

CORE_OBJ = $(patsubst core/models/%.cpp,   $(BUILDDIR)/%.o, $(wildcard core/models/*.cpp)) \
           $(patsubst core/services/%.cpp, $(BUILDDIR)/%.o, $(wildcard core/services/*.cpp))

all: $(BUILDDIR) $(BINDIR) app1 app2 app_gui

$(BUILDDIR) $(BINDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: core/models/%.cpp core/models/%.h | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: core/services/%.cpp core/services/%.h | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/imgui_%.o: $(IMGUI_DIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) -c $< -o $@

app1: src/app1.cpp $(CORE_OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/app1 $< $(CORE_OBJ)

app2: src/app2.cpp $(CORE_OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/app2 $< $(CORE_OBJ)

app_gui: src/app_gui.cpp $(CORE_OBJ) $(IMGUI_OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) -o $(BINDIR)/app_gui $< $(CORE_OBJ) $(IMGUI_OBJ) $(GUI_LIBS)

clean:
	rm -rf $(BUILDDIR) $(BINDIR)
