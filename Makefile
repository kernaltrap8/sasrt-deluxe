# Compiler and flags
CXX = i686-w64-mingw32-g++
CXXFLAGS = -shared -static-libgcc -static-libstdc++ -Wl,--kill-at

# Libraries
LIBS = -Llib -ld3d9 -lpsapi -limagehlp

# Directories
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
OUT_DIR = out

GAME_DIR = ~/.local/share/Steam/steamapps/common/Sonic\ \&\ All-Stars\ Racing\ Transformed/

# Sources and objects
SOURCES = $(SRC_DIR)/dllmain.cpp \
          $(SRC_DIR)/common.cpp \
          $(SRC_DIR)/memory_patcher.cpp \
          $(SRC_DIR)/window_hooks.cpp \
          $(SRC_DIR)/debug_hooks.cpp \
          $(SRC_DIR)/d3d9_proxy.cpp \
		  $(SRC_DIR)/file_hooks.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = $(OUT_DIR)/d3d9.dll
DEF_FILE = $(LIB_DIR)/d3d9.def
VERSION_HEADER = $(SRC_DIR)/version.h

# Default target
#all: $(VERSION_HEADER) $(TARGET) deploy
all: $(VERSION_HEADER) $(TARGET)

# Generate version header
$(VERSION_HEADER):
	@mkdir -p $(dir $(VERSION_HEADER))
	@echo "// Auto-generated file" > $(VERSION_HEADER)
	@echo "#pragma once" >> $(VERSION_HEADER)
	@COMMIT=$$(git rev-parse --short HEAD 2>/dev/null || echo unknown); \
	TAG=$$(git describe --tags --always --dirty 2>/dev/null || echo unknown); \
	echo "#define GIT_COMMIT \"$$COMMIT\"" >> $(VERSION_HEADER); \
	echo "#define GIT_TAG \"$$TAG\"" >> $(VERSION_HEADER)

# Ensure output directory exists before linking
$(TARGET): $(OBJECTS) $(DEF_FILE) | $(OUT_DIR)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(DEF_FILE) $(LIBS)

# Create output directory
$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# Create .def file
$(DEF_FILE):
	@mkdir -p $(LIB_DIR)
	@echo "LIBRARY \"D3D9.DLL\"" > $(DEF_FILE)
	@echo "EXPORTS" >> $(DEF_FILE)
	@echo "Direct3DCreate9@4" >> $(DEF_FILE)

# Compile .cpp -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) -c $< -o $@

# Ensure object directory exists
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Deploy DLL to game directory
#deploy: $(TARGET)
#	@echo "Deploying to game directory..."
#	cp $(TARGET) $(GAME_DIR)/d3d9.dll
#	sha256sum $(TARGET) $(GAME_DIR)/d3d9.dll

# Clean build
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(OUT_DIR)
	rm -f $(DEF_FILE) $(VERSION_HEADER)

rebuild: clean all

# Show build info
info:
	@echo "Source files: $(SOURCES)"
	@echo "Object files: $(OBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Game directory: $(GAME_DIR)"
	@echo "Version header: $(VERSION_HEADER)"

#.PHONY: all deploy clean rebuild info
.PHONY: all clean rebuild info
