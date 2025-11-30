# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinterfaces

# Diretórios
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

# Nome do executável
TARGET = $(BIN_DIR)/dispatcher

# Lista de arquivos fonte
SRC = $(wildcard $(SRC_DIR)/*.c)

# Para cada src/foo.c gera build/foo.o
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Regra padrão
all: $(OBJ_DIR) $(BIN_DIR) $(TARGET)

# Cria pastas se não existirem
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Regras para gerar o binário final
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Regra para compilar .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpezas
clean:
	rm -f $(OBJ_DIR)/*.o

cleanall:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

run: all
	./$(TARGET) processes.txt files.txt

.PHONY: all clean cleanall run
