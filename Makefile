CC = gcc
CFLAGS = -std=c99 -O2 -Wall -Iinclude

SRCS = src/main.c src/global.c src/queues.c src/memory.c src/filesys.c src/processes.c src/utils.c src/dispatcher.c

OBJDIR = bin
OBJS = $(SRCS:src/%.c=$(OBJDIR)/%.o)

TARGET = dispatcher

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)
