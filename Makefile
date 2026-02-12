# Build file for Pixel-Ripper.

CC = gcc
# -O3: optimize for throughput
# -march=native: enable host CPU instruction set
# -Wall/-Wextra: strict warning baseline
CFLAGS = -O3 -march=native -Wall -Wextra -I./include

# Required FFmpeg + math libs
LIBS = -lavcodec -lavformat -lswscale -lavutil -lm

# Source files
SRCS = src/main.c src/ascii_engine.c src/engine_haruhi.c
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = ascii_engine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Generic compile rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean
