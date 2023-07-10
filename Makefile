OUT = hashguesser

CC = gcc
# -O3 and -march=native is highly recommended (approx x2 faster)
FLAGS = -Wall -Wextra -g -std=c17 -Iinclude -O3 -march=native

SRCDIR = src
OBJDIR = obj
BINDIR = bin


SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/$(OUT)

all: dirs $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC)  $(CFLAGS) -c $< -o $@

clean:
	rm -rvf obj/ bin/

dirs:
	mkdir -p obj
	mkdir -p bin

.PHONY: all clean
