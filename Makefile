# ==================== STRUCTURES DE DONNÉES - RAYLIB ====================
# Makefile pour Windows avec MinGW-w64

# Compilateur
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Wno-unused-parameter

# Raylib - Modifier ces chemins selon votre installation
# Option 1: Raylib installé via w64devkit ou dans un dossier custom
# RAYLIB_PATH = C:/raylib/raylib
# Option 2: Raylib téléchargé et extrait localement
RAYLIB_PATH = raylib

# Includes et libs
INCLUDES = -I$(RAYLIB_PATH)/include -Isrc
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32

# Alternative si raylib est dans le PATH système
# INCLUDES = -Isrc
# LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Fichiers source
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/ui_common.c \
          $(SRC_DIR)/welcome.c \
          $(SRC_DIR)/tableaux/sorting.c \
          $(SRC_DIR)/tableaux/tableaux.c \
          $(SRC_DIR)/listes/listes.c \
          $(SRC_DIR)/arbres/arbres.c \
          $(SRC_DIR)/graphes/graphes.c

# Fichiers objets
OBJECTS = $(SOURCES:.c=.o)

# Exécutable
TARGET = structures_donnees.exe

# ==================== RÈGLES ====================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)
	@echo "=========================================="
	@echo "Build terminee: $(TARGET)"
	@echo "Lancez avec: ./$(TARGET) ou make run"
	@echo "=========================================="

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Nettoyage..."
	-del /Q src\*.o 2>nul
	-del /Q src\tableaux\*.o 2>nul
	-del /Q src\listes\*.o 2>nul
	-del /Q src\arbres\*.o 2>nul
	-del /Q src\graphes\*.o 2>nul
	-del /Q $(TARGET) 2>nul
	@echo "Nettoyage termine"

run: $(TARGET)
	./$(TARGET)

# ==================== INSTALLATION RAYLIB ====================

# Télécharge raylib 5.0 automatiquement (nécessite curl)
setup-raylib:
	@echo "Telechargement de raylib 5.0..."
	curl -L -o raylib.zip https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip
	@echo "Extraction..."
	powershell -command "Expand-Archive -Force raylib.zip ."
	@if exist raylib rmdir /S /Q raylib
	move raylib-5.0_win64_mingw-w64 raylib
	del raylib.zip
	@echo "=========================================="
	@echo "Raylib installe dans ./raylib"
	@echo "Vous pouvez maintenant compiler avec: make"
	@echo "=========================================="

# ==================== AIDE ====================

help:
	@echo "=========================================="
	@echo "STRUCTURES DE DONNEES - RAYLIB C"
	@echo "=========================================="
	@echo ""
	@echo "Commandes disponibles:"
	@echo "  make              - Compile le projet"
	@echo "  make run          - Compile et lance"
	@echo "  make clean        - Supprime les fichiers compiles"
	@echo "  make setup-raylib - Telecharge et installe raylib 5.0"
	@echo "  make help         - Affiche cette aide"
	@echo ""
	@echo "Prerequis:"
	@echo "  - MinGW-w64 (gcc, make)"
	@echo "  - Raylib 5.0 (make setup-raylib pour installer)"
	@echo ""
	@echo "Si raylib est installe ailleurs, modifiez RAYLIB_PATH"

.PHONY: all clean run setup-raylib help
