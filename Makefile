CXX       = g++
CXXFLAGS  = -std=c++23 -Wall -Wextra
BUILDDIR  = build
SFML_INC  = -I C:/msys64/mingw64/include
SFML_LIBS = -L C:/msys64/mingw64/lib -lsfml-graphics -lsfml-window -lsfml-system

MODELS_SRC   = common/models/Lectie.cpp common/models/Curs.cpp common/models/Inscriere.cpp
SERVICES_SRC = common/services/CursService.cpp common/services/InscriereService.cpp
COMMON_SRC   = $(MODELS_SRC) $(SERVICES_SRC)

MODELS_OBJ   = $(patsubst common/models/%.cpp,   $(BUILDDIR)/%.o, $(MODELS_SRC))
SERVICES_OBJ = $(patsubst common/services/%.cpp, $(BUILDDIR)/%.o, $(SERVICES_SRC))
COMMON_OBJ   = $(MODELS_OBJ) $(SERVICES_OBJ)

all: $(BUILDDIR) app1 app2 app_gui

# Create build directory if it doesn't exist
$(BUILDDIR):
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)

# Compile model .cpp files
$(BUILDDIR)/%.o: common/models/%.cpp common/models/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile service .cpp files
$(BUILDDIR)/%.o: common/services/%.cpp common/services/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

app1: src/app1.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o app1 src/app1.cpp $(COMMON_OBJ)

app2: src/app2.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o app2 src/app2.cpp $(COMMON_OBJ)

app_gui: src/app_gui.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(SFML_INC) -o app_gui src/app_gui.cpp $(COMMON_OBJ) $(SFML_LIBS)

clean:
	if exist app1.exe del app1.exe
	if exist app2.exe del app2.exe
	if exist app_gui.exe del app_gui.exe
	if exist $(BUILDDIR) rmdir /s /q $(BUILDDIR)
