CC = clang
CFLAGS = -std=c99 -g3 -Wall -Wextra -pedantic -Iinclude
LDFLAGS = -fsanitize=address

# Dossiers
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Fichiers sources
MAIN_SRC = $(SRCDIR)/main.c
CHEAT_SRC = $(SRCDIR)/cheat.c
MEMORY_SRC = $(SRCDIR)/memoryManagement.c
MATH_TOOL_SRC = $(SRCDIR)/mathTool.c

# Fichiers objets
MAIN_OBJ = $(OBJDIR)/main.o
CHEAT_OBJ = $(OBJDIR)/cheat.o
MEMORY_OBJ = $(OBJDIR)/memoryManagement.o
MATH_TOOL_OBJ = $(OBJDIR)/mathTool.o

# Créer le dossier obj s'il n'existe pas
$(OBJDIR):
	mkdir -p $(OBJDIR)

all: cheat main

# Compilation de main
$(MAIN_OBJ): $(MAIN_SRC) | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(MAIN_SRC) -o $(MAIN_OBJ)

main: $(MAIN_OBJ) $(MEMORY_OBJ)
	$(CC) -o main $(MAIN_OBJ) $(MEMORY_OBJ) $(LDFLAGS)

# Compilation de cheat
$(CHEAT_OBJ): $(CHEAT_SRC) | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(CHEAT_SRC) -o $(CHEAT_OBJ)

# Compilation de memory
$(MEMORY_OBJ): $(MEMORY_SRC) | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(MEMORY_SRC) -o $(MEMORY_OBJ)

# Compilation de math_tool
$(MATH_TOOL_OBJ): $(MATH_TOOL_SRC) | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(MATH_TOOL_SRC) -o $(MATH_TOOL_OBJ)

cheat: $(CHEAT_OBJ) $(MEMORY_OBJ) $(MATH_TOOL_OBJ)
	$(CC) -o cheat $(CHEAT_OBJ) $(MEMORY_OBJ) $(MATH_TOOL_OBJ) $(LDFLAGS)

# Règles d'exécution
run: main
	@echo "Running memory debug tool..."
	./main

run_cheat: cheat
	@echo "Starting aimbot..."
	./cheat

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build both executables"
	@echo "  cheat     - Build aimbot only"
	@echo "  main      - Build debug tool only"
	@echo "  run       - Run debug tool"
	@echo "  run_cheat - Run aimbot"
	@echo "  clean     - Remove build artifacts"
	@echo "  info      - Show project structure"
	@echo "  help      - Show this help"

# Nettoyage
clean:
	rm -rf $(OBJDIR) main cheat

# Nettoyage complet
distclean: clean
	rm -rf *~ $(INCDIR)/*~ $(SRCDIR)/*~

# Afficher la structure du projet
info:
	@echo "Structure du projet:"
	@echo "├── include/     (fichiers .h)"
	@echo "├── src/         (fichiers .c)"
	@echo "├── obj/         (fichiers .o générés)"
	@echo "├── main         (exécutable)"
	@echo "└── cheat        (exécutable)"

# Règles pour forcer la recompilation
.PHONY: all clean distclean run run_cheat info help