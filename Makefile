OUT = hash.out

CC = gcc
FLAGS = -Wall -Wextra -g -std=c17 -Iinclude -O3 -march=native
LDFLAGS =

SRCDIR = src
OBJDIR = obj
BINDIR = bin


SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/$(OUT)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC)  $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(EXECUTABLE)

.PHONY: all clean
