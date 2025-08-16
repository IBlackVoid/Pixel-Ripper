# =====================================================================================
#
# Makefile for the Unified ASCII Rendering Engine
#
# =====================================================================================

CC = gcc
# Why these flags? -O3 for max optimization. -march=native tells gcc to use all
# instruction sets your specific CPU supports (like AVX2). -Wall and -Wextra
# are non-negotiable for clean code. -I./include tells it to look for headers.
CFLAGS = -O3 -march=native -Wall -Wextra -I./include

# Why these libs? These are the sacred texts of FFmpeg we must link against.
LIBS = -lavcodec -lavformat -lswscale -lavutil -lm

# All source files in the src/ directory
SRCS = src/main.c src/ascii_engine.c
OBJS = $(SRCS:.c=.o)

# The final executable
TARGET = ascii_engine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Why %.o: %.c? This is a pattern rule. It tells make how to build any .o
# file from its corresponding .c file, so we don't have to write a rule
# for every single source file. It's clean. It's efficient.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean

