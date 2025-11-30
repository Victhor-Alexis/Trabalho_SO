# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinterfaces

# Diretórios
SRC_DIR = src
BIN_DIR = bin

# Nome do executável
TARGET = $(BIN_DIR)/dispatcher

# Lista de todos os .c dentro de src/
SRC = $(wildcard $(SRC_DIR)/*.c)

# Objetos correspondentes
OBJ = $(SRC:.c=.o)

# Regra principal
all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Link final
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compilar cada arquivo .c em .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar objetos e binário
clean:
	rm -f $(SRC_DIR)/*.o
	rm -f $(TARGET)

cleanall:
	rm -rf $(BIN_DIR)
	rm -f $(SRC_DIR)/*.o

run: all
	./$(TARGET) processes.txt files.txt

.PHONY: all clean cleanall run
