# -----------------------------
# Simple Shell – Makefile
# Author: Eduardo Muñoz
# Description: Stand-alone build script (no instructor scaffolding)
# -----------------------------

# === Config ===
# Executable name
PROG      := sfshell
# All C source files (comma-separated list or wildcard)
SRCS      := $(wildcard *.c)
# Object files generated from SRCS
OBJS      := $(SRCS:.c=.o)

# Compiler & flags
CC        ?= gcc
CFLAGS    ?= -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS   ?=
LIBS      :=

# === Targets ===

# Default rule – build everything
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Pattern rule – compile .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run with optional arguments, e.g. `make run ARGS="-c 'ls -l'"`
ARGS ?=
run: $(PROG)
	./$(PROG) $(ARGS)

# Valgrind helper (Linux/macOS)
vrun: $(PROG)
	valgrind --leak-check=full ./$(PROG) $(ARGS)

# Remove objects & binary
clean:
	rm -f $(OBJS) $(PROG)

.PHONY: all run vrun clean
